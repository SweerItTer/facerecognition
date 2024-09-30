#ifndef FACEDATABASE_H
#define FACEDATABASE_H

#define NOMINMAX
#include <iostream>
#include <mysql.h>
#include <vector>
#include <cstring>
#include "dataitem.h"

class FaceDatabase 
{
public:

/** 
 * @brief 读取人脸数据库
 * @param host      端口
 * @param user      用户名
 * @param password  密码
 * @param db_name   数据库名字
 */
    FaceDatabase(const std::string& host, const std::string& user, 
                const std::string& password, const std::string& db_name) 
    {
        conn = mysql_init(nullptr);
        if (!conn) {
            throw std::runtime_error("mysql_init() failed");
        }

        // 连接数据库
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), 
                                      password.c_str(), db_name.c_str(), 0, nullptr, 0)) {
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

/** 
 * @brief 插入特征向量到数据库
 * @param user_id           用户的id
 * @param feature_vectors   人脸特征组
 * @return bool
 */
	// 插入特征向量到数据库
	bool insertFeatures(const std::string& name, const std::vector<std::vector<float>>& feature_vectors) {
		if (feature_vectors.size() != 3) {
			std::cerr << "Error: Exactly 3 feature vectors are required." << std::endl;
			return false;
		}

		std::string query = "INSERT INTO FaceFeatures (name, feature_vector1, feature_vector2, feature_vector3) VALUES (?, ?, ?, ?)";

		// 创建并执行 SQL 语句
		MYSQL_STMT* stmt = mysql_stmt_init(conn);
		if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
			std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
			return false;
		}

		// 绑定参数
		MYSQL_BIND bind[4];
		memset(bind, 0, sizeof(bind));

		bind[0].buffer_type = MYSQL_TYPE_STRING;
		bind[0].buffer = (void*)name.c_str();
		bind[0].buffer_length = static_cast<unsigned long>(name.size());

		std::string vector_str1(reinterpret_cast<const char*>(feature_vectors[0].data()), feature_vectors[0].size() * sizeof(float));
		bind[1].buffer_type = MYSQL_TYPE_BLOB;
		bind[1].buffer = (void*)vector_str1.data();
		bind[1].buffer_length = static_cast<unsigned long>(vector_str1.size());

		std::string vector_str2(reinterpret_cast<const char*>(feature_vectors[1].data()), feature_vectors[1].size() * sizeof(float));
		bind[2].buffer_type = MYSQL_TYPE_BLOB;
		bind[2].buffer = (void*)vector_str2.data();
		bind[2].buffer_length = static_cast<unsigned long>(vector_str2.size());

		std::string vector_str3(reinterpret_cast<const char*>(feature_vectors[2].data()), feature_vectors[2].size() * sizeof(float));
		bind[3].buffer_type = MYSQL_TYPE_BLOB;
		bind[3].buffer = (void*)vector_str3.data();
		bind[3].buffer_length = static_cast<unsigned long>(vector_str3.size());

		if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)) {
			std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
			mysql_stmt_close(stmt);
			return false;
		}

		mysql_stmt_close(stmt);
		return true; // 返回成功
	}

/** 
 * @brief 获取所有用户的特征向量
 * @return all_features     
 */
    // 获取所有用户的特征向量
    std::vector<DataItem> getAllFeatures() {
        std::string query = "SELECT name, feature_vector1, feature_vector2, feature_vector3 FROM FaceFeatures";
        if (mysql_query(conn, query.c_str())) {
            std::cerr << "Query failed: " + std::string(mysql_error(conn)) << std::endl;
            return {};
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (!result) {
            std::cerr << "mysql_store_result() failed: " + std::string(mysql_error(conn)) << std::endl;
        }

        MYSQL_ROW row;
        std::vector<DataItem> all_features;

        while ((row = mysql_fetch_row(result))) {
            DataItem data_item;
            data_item.name = row[0];  // 假设 name 是字符串
            std::vector<float> feature_vectors[3];  // 临时存储三个特征向量
            unsigned long* lengths = mysql_fetch_lengths(result);

            for (int i = 1; i <= 3; ++i) {
                if (lengths[i] % sizeof(float) == 0) { // 检查数据长度是否是 float 的倍数
                    size_t num_floats = lengths[i] / sizeof(float);// 计算 float 数目, 即数据库中存储的特征向量长度
                    // 调整特征向量的大小并复制数据
                    feature_vectors[i-1].resize(num_floats); // 重新分配空间, 将数据库中的数据读入 feature_vectors
                    std::memcpy(feature_vectors[i-1].data(), row[i], lengths[i]); // 分配内存, 将数据库中的数据读入 feature_vectors
                } else {
                    // 长度不是 float 的倍数, 说明数据有问题
                    std::cerr << "Data length is not a multiple of float size!" << std::endl;
                }
            }
/*
内存分配和复制数据,分别为:
    需要复制到的特征向量的指针, 需要被复制的数据库中存储的特征向量指针, 以及特征向量的长度
std::memcpy(feature_vectors[i-1].data(), row[i], lengths[i]);
因为数据库里索引i是特征向量的编号,而对feature_vectors而言,当前位应当是i-1
*/
        
        // 分别赋值给 DataItem 的 vector1, vector2, vector3
        // move避免无用的复制, 节省内存
        data_item.vector1 = std::move(feature_vectors[0]);
        data_item.vector2 = std::move(feature_vectors[1]);
        data_item.vector3 = std::move(feature_vectors[2]);

        all_features.push_back(std::move(data_item));
        }

        mysql_free_result(result);
        return all_features;
    }

private:
    MYSQL*  conn;
};

#endif // FACEDATABASE_H
