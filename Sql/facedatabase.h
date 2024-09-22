#include <iostream>
#include <mysql/mysql.h>
#include <vector>

class FaceDatabase {
public:
    FaceDatabase(const std::string& host, const std::string& user, const std::string& password, const std::string& db_name) {
        conn = mysql_init(nullptr);
        if (conn == nullptr) {
            throw std::runtime_error("mysql_init() failed");
        }
        if (mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), db_name.c_str(), 0, nullptr, 0) == nullptr) {
            mysql_close(conn);
            throw std::runtime_error("mysql_real_connect() failed");
        }
    }

    ~FaceDatabase() {
        mysql_close(conn);
    }

    void insertFeature(const std::string& user_id, const std::vector<float>& feature_vector) {
        // Convert feature_vector to a string for storage, or use BLOB
        std::string vector_str(reinterpret_cast<const char*>(feature_vector.data()), feature_vector.size() * sizeof(float));
        std::string query = "INSERT INTO FaceFeatures (user_id, feature_vector) VALUES ('" + user_id + "', ?)";

        MYSQL_STMT *stmt = mysql_stmt_init(conn);
        mysql_stmt_prepare(stmt, query.c_str(), query.size());
        
        MYSQL_BIND bind[2];
        memset(bind, 0, sizeof(bind));

        // Set user_id
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)user_id.c_str();
        bind[0].buffer_length = user_id.size();

        // Set feature_vector as a BLOB
        bind[1].buffer_type = MYSQL_TYPE_BLOB;
        bind[1].buffer = (char*)vector_str.data();
        bind[1].buffer_length = vector_str.size();

        mysql_stmt_bind_param(stmt, bind);
        mysql_stmt_execute(stmt);
        mysql_stmt_close(stmt);
    }

    std::vector<float> getFeature(const std::string& user_id) {
        std::string query = "SELECT feature_vector FROM FaceFeatures WHERE user_id = '" + user_id + "'";
        mysql_query(conn, query.c_str());
        MYSQL_RES *result = mysql_store_result(conn);
        MYSQL_ROW row;

        std::vector<float> feature_vector;

        if ((row = mysql_fetch_row(result))) {
            unsigned long *lengths = mysql_fetch_lengths(result);
            feature_vector.resize(lengths[0] / sizeof(float));
            std::memcpy(feature_vector.data(), row[0], lengths[0]);
        }

        mysql_free_result(result);
        return feature_vector;
    }

private:
    MYSQL *conn;
};

int main() {
    FaceDatabase db("127.0.0.1", "root", "root", "FaceDB");

    // 示例：插入特征向量
    std::string user_id = "user1";
    std::vector<float> feature_vector = {0.1f, 0.2f, 0.3f}; // 示例特征向量
    db.insertFeature(user_id, feature_vector);

    // 示例：获取特征向量
    std::vector<float> retrieved_vector = db.getFeature(user_id);
    std::cout << "Retrieved feature vector for " << user_id << ": ";
    for (float value : retrieved_vector) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}
