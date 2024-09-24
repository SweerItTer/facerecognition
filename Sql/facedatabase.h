#include <iostream>
#include <mysql.h>
#include <vector>
#include <cstring>

class FaceDatabase {
public:
    FaceDatabase(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name) {
        conn = mysql_init(nullptr);
        if (!conn) {
            throw std::runtime_error("mysql_init() failed");
        }

        // 连接数据库
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), db_name.c_str(), 0, nullptr, 0)) {
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
    void insertFeatures(const std::string& user_id, const std::vector<std::vector<float>>& feature_vectors) {
        if (feature_vectors.size() != 3) {
            throw std::runtime_error("Exactly 3 feature vectors are required.");
        }

        std::string query = "INSERT INTO FaceFeatures (user_id, feature_vector1, feature_vector2, feature_vector3) VALUES (?, ?, ?, ?)";

        // 创建并执行 SQL 语句
        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), query.size())) {
            throw std::runtime_error("Failed to prepare statement: " + std::string(mysql_error(conn)));
        }

        // 绑定参数
        MYSQL_BIND bind[4];
        memset(bind, 0, sizeof(bind));

        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)user_id.c_str();
        bind[0].buffer_length = user_id.size();

        std::string vector_str1(reinterpret_cast<const char*>(feature_vectors[0].data()), feature_vectors[0].size() * sizeof(float));
        bind[1].buffer_type = MYSQL_TYPE_BLOB;
        bind[1].buffer = (void*)vector_str1.data();
        bind[1].buffer_length = vector_str1.size();

        std::string vector_str2(reinterpret_cast<const char*>(feature_vectors[1].data()), feature_vectors[1].size() * sizeof(float));
        bind[2].buffer_type = MYSQL_TYPE_BLOB;
        bind[2].buffer = (void*)vector_str2.data();
        bind[2].buffer_length = vector_str2.size();

        std::string vector_str3(reinterpret_cast<const char*>(feature_vectors[2].data()), feature_vectors[2].size() * sizeof(float));
        bind[3].buffer_type = MYSQL_TYPE_BLOB;
        bind[3].buffer = (void*)vector_str3.data();
        bind[3].buffer_length = vector_str3.size();

        if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)) {
            throw std::runtime_error("Failed to execute statement: " + std::string(mysql_error(conn)));
        }

        mysql_stmt_close(stmt);
    }

    // 获取所有用户的特征向量
    std::vector<std::pair<std::string, std::vector<std::vector<float>>>> getAllFeatures() {
        std::string query = "SELECT user_id, feature_vector1, feature_vector2, feature_vector3 FROM FaceFeatures";
        if (mysql_query(conn, query.c_str())) {
            throw std::runtime_error("Query failed: " + std::string(mysql_error(conn)));
        }

        MYSQL_RES* result = mysql_store_result(conn);
        if (!result) {
            throw std::runtime_error("mysql_store_result() failed: " + std::string(mysql_error(conn)));
        }

        MYSQL_ROW row;
        std::vector<std::pair<std::string, std::vector<std::vector<float>>>> all_features;

        while ((row = mysql_fetch_row(result))) {
            std::string user_id = row[0];
            std::vector<std::vector<float>> feature_vectors(3);
            unsigned long* lengths = mysql_fetch_lengths(result);

            for (int i = 1; i <= 3; ++i) {
                if (lengths[i] % sizeof(float) == 0) {
                    size_t num_floats = lengths[i] / sizeof(float);
                    feature_vectors[i-1].resize(num_floats);
                    std::memcpy(feature_vectors[i-1].data(), row[i], lengths[i]);
                } else {
                    std::cerr << "Data length is not a multiple of float size!" << std::endl;
                }
            }

            all_features.push_back({user_id, feature_vectors});
        }

        mysql_free_result(result);
        return all_features;
    }

private:
    MYSQL* conn;
};
