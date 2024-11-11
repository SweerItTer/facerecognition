#ifndef FACEDATABASE_H
#define FACEDATABASE_H

#define NOMINMAX
#include <iostream>
#include <mysql.h>
#include <vector>
#include <cstring>
#include <mutex>
#include "dataitem.h"

class FaceDatabase 
{
public:
    FaceDatabase(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name, const unsigned int port = 3306) {
        conn = mysql_init(nullptr);
        if (!conn) {
            throw std::runtime_error("mysql_init() failed");
        }

        // 连接数据库
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), db_name.c_str(), port, nullptr, 0)) {
            std::string error_message = "mysql_real_connect() failed: ";
            error_message += mysql_error(conn);
            mysql_close(conn);
            throw std::runtime_error(error_message);
        }
    }

    ~FaceDatabase() {
        // 关闭连接
        mysql_close(conn);
    }

	// 插入特征向量到数据库
	bool insertFeatures(const std::string& name, const std::vector<std::vector<float>>& feature_vectors) {
        std::lock_guard<std::mutex> lock(db_mutex); // 自动锁定互斥锁，在作用域结束时自动释放

        if (feature_vectors.size() != 10) {
			std::cerr << "Error: Exactly 10 feature vectors are required." << std::endl;
			return false;
		}

        // SQL 查询语句，支持插入或覆盖
        std::string query = "INSERT INTO FaceFeatures (name, "
                           "feature_vector1, feature_vector2, feature_vector3, "
                           "feature_vector4, feature_vector5, feature_vector6, "
                           "feature_vector7, feature_vector8, feature_vector9, "
                           "feature_vector10) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                           "ON DUPLICATE KEY UPDATE "
                           "feature_vector1 = VALUES(feature_vector1), "
                           "feature_vector2 = VALUES(feature_vector2), "
                           "feature_vector3 = VALUES(feature_vector3), "
                           "feature_vector4 = VALUES(feature_vector4), "
                           "feature_vector5 = VALUES(feature_vector5), "
                           "feature_vector6 = VALUES(feature_vector6), "
                           "feature_vector7 = VALUES(feature_vector7), "
                           "feature_vector8 = VALUES(feature_vector8), "
                           "feature_vector9 = VALUES(feature_vector9), "
                           "feature_vector10 = VALUES(feature_vector10)";

		// 创建并执行 SQL 语句
		MYSQL_STMT* stmt = mysql_stmt_init(conn);
		if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
			std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
			return false;
		}

		// 绑定参数
		MYSQL_BIND bind[11];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = (void*)name.c_str();
        bind[0].buffer_length = static_cast<unsigned long>(name.size());

		// 绑定10个特征向量
		std::vector<std::string> vector_strs(10);
		for(int i = 0; i < 10; ++i) {
			vector_strs[i] = std::string(reinterpret_cast<const char*>(feature_vectors[i].data()), 
									   feature_vectors[i].size() * sizeof(float));
			bind[i+1].buffer_type = MYSQL_TYPE_BLOB;
			bind[i+1].buffer = (void*)vector_strs[i].data();
			bind[i+1].buffer_length = static_cast<unsigned long>(vector_strs[i].size());
		}

		if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)) {
			std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
			mysql_stmt_close(stmt);
			return false;
		}

		mysql_stmt_close(stmt);
		return true; // 返回成功
	}

    // 获取所有用户的特征向量
    std::vector<DataItem> getAllFeatures() {
        std::string query = "SELECT name, feature_vector1, feature_vector2, feature_vector3, "
                           "feature_vector4, feature_vector5, feature_vector6, "
                           "feature_vector7, feature_vector8, feature_vector9, "
                           "feature_vector10 FROM FaceFeatures";
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Query failed: " + std::string(mysql_error(conn)) << std::endl;
            return {};
        }
        std::lock_guard<std::mutex> lock(db_mutex); // 自动锁定互斥锁，在作用域结束时自动释放

        MYSQL_RES* result = mysql_store_result(conn);
        if (!result) {
            std::cerr << "mysql_store_result() failed: " + std::string(mysql_error(conn)) << std::endl;
        }

        MYSQL_ROW row;
        std::vector<DataItem> all_features;

        while ((row = mysql_fetch_row(result))) {
            DataItem data_item;
            data_item.name = row[0];  // 假设 name 是字符串
            std::vector<float> feature_vectors[10];  // 修改为10个向量
            unsigned long* lengths = mysql_fetch_lengths(result);

            for (int i = 1; i <= 10; ++i) {
                if (lengths[i] % sizeof(float) == 0) { // 检查数据长度是否是 float 的倍数
                    size_t num_floats = lengths[i] / sizeof(float);// 计算 float 数目, 即数据库中存储的特征向量长度
                    // 调整特征向量的大小并复制数据
                    feature_vectors[i-1].resize(num_floats); // 重新分配空间, 将数据库中的数据读入 feature_vectors
                    std::memcpy(feature_vectors[i-1].data(), row[i], lengths[i]); // 分配内存, 将数据库中的数据读入 feature_vectors
                } else {
                    std::cerr << "Data length is not a multiple of float size!" << std::endl;
                }
            }

            // 移动所有向量到DataItem
            data_item.vectors.reserve(10);  // 预分配空间
            for(int i = 0; i < 10; ++i) {
                data_item.vectors.push_back(std::move(feature_vectors[i]));
            }

            all_features.push_back(std::move(data_item));
        }

        mysql_free_result(result);
        return all_features;
    }

    MYSQL* getConnection() {
        return conn;
    }

//---------------HHL-------------
    // 插入学生信息到数据库 
    bool insertIDinformation(const std::string& name, const std::string& ID,
                             const std::string& major, const std::string& grade, const std::string& college) 
    {
        // SQL查询语句，用于插入或更新学生信息
        std::string query = "INSERT INTO IDinformation"
                    "(name, ID, major, grade, college) "
                    "VALUES (?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "name = VALUES(name),"
                    "major = VALUES(major), "
                    "grade = VALUES(grade), "
                    "college = VALUES(college)";


		// 创建并执行 SQL 语句
		MYSQL_STMT* stmt = mysql_stmt_init(conn);
		if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
			std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
			return false;
		}

        // 绑定参数到预处理语句
        MYSQL_BIND bind[5] = {};
        memset(bind, 0, sizeof(bind));

        // 绑定姓名
		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = (void*)name.c_str();
        bind[0].buffer_length = static_cast<unsigned long>(name.size());

        // 绑定学号
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (char*)ID.c_str();
        bind[1].buffer_length = static_cast<unsigned long>(ID.size());

        // 绑定专业
        bind[2].buffer_type = MYSQL_TYPE_STRING;
        bind[2].buffer = (char*)major.c_str();
        bind[1].buffer_length = static_cast<unsigned long>(major.size());

        // 绑定年级
        bind[3].buffer_type = MYSQL_TYPE_STRING;
        bind[3].buffer = (char*)grade.c_str();
        bind[3].buffer_length = static_cast<unsigned long>(grade.size());

        // 绑定学院
        bind[4].buffer_type = MYSQL_TYPE_STRING;
        bind[4].buffer = (char*)college.c_str();
        bind[4].buffer_length = static_cast<unsigned long>(college.size());

		if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)) {
			std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
			mysql_stmt_close(stmt);
			return false;
		}

		mysql_stmt_close(stmt);
		return true; // 返回成功
    }
    
    // 插入用户账号信息到数据库
    bool insertUserAccount(const std::string& ID, const std::string& password)
    {
        std::string query = "INSERT INTO UserAccount"
                            "(ID, password) "
                            "VALUES (?, ?) "
                            "ON DUPLICATE KEY UPDATE "
                            "ID = VALUES(ID),"
                            "password = VALUES(password) ";

		// 创建并执行 SQL 语句
		MYSQL_STMT* stmt = mysql_stmt_init(conn);
		if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
			std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
			return false;
		}

        // 绑定参数到预处理语句
        MYSQL_BIND bind[2] = {};
        memset(bind, 0, sizeof(bind));

        // 绑定姓名
		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = (void*)ID.c_str();
        bind[0].buffer_length = static_cast<unsigned long>(ID.size());

        // 绑定学号
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (char*)password.c_str();
        bind[1].buffer_length = static_cast<unsigned long>(password.size());

        if (mysql_stmt_bind_param(stmt, bind)) {
            std::cerr << "Error: Failed to bind parameters: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_execute(stmt)) {
            std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

		mysql_stmt_close(stmt);
		return true; // 返回成功
    }

    // 检查用户账号是否重复
    bool checkUserAccount(const std::string& ID)
    {
        std::string query = "SELECT 1 FROM UserAccount WHERE ID = ? LIMIT 1";

        // 创建并执行 SQL 语句
        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
            std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
            return false;
        }

        // 绑定参数到预处理语句
        MYSQL_BIND bind[1] = {};
        memset(bind, 0, sizeof(bind));

        // 绑定账号
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)ID.c_str();
        bind[0].buffer_length = static_cast<unsigned long>(ID.size());

        if (mysql_stmt_bind_param(stmt, bind)) {
            std::cerr << "Error: Failed to bind parameters: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_execute(stmt)) {
            std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // 绑定结果集
        MYSQL_BIND result_bind[1] = {};
        memset(result_bind, 0, sizeof(result_bind));

        // 绑定一个临时变量来接收结果
        int exists = 0;
        result_bind[0].buffer_type = MYSQL_TYPE_LONG;
        result_bind[0].buffer = &exists;
        result_bind[0].buffer_length = sizeof(exists);

        if (mysql_stmt_bind_result(stmt, result_bind)) {
            std::cerr << "Error: Failed to bind result: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // 存储查询结果
        if (mysql_stmt_store_result(stmt)) {
            std::cerr << "Error: Failed to store result: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // 检查是否有结果
        bool account_exists = false;
        if (mysql_stmt_fetch(stmt) == 0) { // 0 表示成功
            account_exists = (exists == 1);
        }

        // 清理资源
        mysql_stmt_free_result(stmt);
        mysql_stmt_close(stmt);

        return !account_exists; // 如果账号存在，则返回false，表示不能注册
    }

    // 检查用户账号密码是否正确
    bool checkUserPassword(const std::string& ID, const std::string& password)
    {
        std::string query = "SELECT 1 FROM UserAccount WHERE ID = ? AND password = ? LIMIT 1";

        // 创建并执行 SQL 语句
        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
            std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
            return false;
        }

        // 绑定参数到预处理语句
        MYSQL_BIND bind[2] = {};
        memset(bind, 0, sizeof(bind));

        // 绑定账号
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)ID.c_str();
        bind[0].buffer_length = static_cast<unsigned long>(ID.size());

        // 绑定密码
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (char*)password.c_str();
        bind[1].buffer_length = static_cast<unsigned long>(password.size());

        if (mysql_stmt_bind_param(stmt, bind)) {
            std::cerr << "Error: Failed to bind parameters: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_execute(stmt)) {
            std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // 绑定结果集
        MYSQL_BIND result_bind[1] = {};
        memset(result_bind, 0, sizeof(result_bind));

        // 绑定一个临时变量来接收结果
        int exists = 0;
        result_bind[0].buffer_type = MYSQL_TYPE_LONG;
        result_bind[0].buffer = &exists;
        result_bind[0].buffer_length = sizeof(exists);

        if (mysql_stmt_bind_result(stmt, result_bind)) {
            std::cerr << "Error: Failed to bind result: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // 存储查询结果
        if (mysql_stmt_store_result(stmt)) {
            std::cerr << "Error: Failed to store result: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        // 检查是否有结果
        bool password_correct = false;
        if (mysql_stmt_fetch(stmt) == 0) { // 0 表示成功
            password_correct = (exists == 1);
        }

        // 清理资源
        mysql_stmt_free_result(stmt);
        mysql_stmt_close(stmt);

        return password_correct; // 返回密码是否正确
    }


//--------------↑↑↑↑↑↑↑↑↑↑↑↑-----------------
private:
    std::mutex db_mutex; // 用于同步数据库访问的互斥锁
    MYSQL*  conn;
};

#endif // FACEDATABASE_H
