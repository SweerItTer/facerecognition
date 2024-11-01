#ifndef ATTENDANCERECORD_H
#define ATTENDANCERECORD_H

#define NOMINMAX
#include <iostream>
#include <mysql.h>
#include <vector>
#include <cstring>
#include <string>

// 定义打卡记录的数据结构
struct AttendanceItem {
    int id;                     // 记录ID
    std::string name;           // 员工姓名
    std::string timestamp;      // 打卡时间戳
    std::string record_type;    // 记录类型（签到/签退）
};

class AttendanceDatabase {
public:
    AttendanceDatabase(const std::string& host, const std::string& user, 
                      const std::string& password, const std::string& db_name, 
                      const unsigned int port = 3306) {
        conn = mysql_init(nullptr);
        if (!conn) {
            throw std::runtime_error("mysql_init() failed");
        }

        // 设置字符集为 utf8mb4
        mysql_options(conn, MYSQL_SET_CHARSET_NAME, "utf8mb4");
        
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), 
                              password.c_str(), db_name.c_str(), port, nullptr, 0)) {
            std::string error_message = "mysql_real_connect() failed: ";
            error_message += mysql_error(conn);
            mysql_close(conn);
            throw std::runtime_error(error_message);
        }

        // 连接后再次确保字符集设置正确
        if (mysql_set_character_set(conn, "utf8mb4")) {
            std::string error_message = "mysql_set_character_set() failed: ";
            error_message += mysql_error(conn);
            mysql_close(conn);
            throw std::runtime_error(error_message);
        }
    }

    ~AttendanceDatabase() {
        mysql_close(conn);
    }

    // 插入打卡记录
    bool insertRecord(const std::string& name, const std::string& record_type) {
        std::string query = "INSERT INTO AttendanceRecords (name, timestamp, record_type) "
                           "VALUES (?, NOW(), ?)";

        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt) {
            std::cerr << "Error: mysql_stmt_init() failed" << std::endl;
            return false;
        }

        if (mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
            std::cerr << "Error: mysql_stmt_prepare() failed: " << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        MYSQL_BIND bind[2];
        memset(bind, 0, sizeof(bind));

        // 为字符串参数添加长度指示器
        unsigned long name_length = static_cast<unsigned long>(name.size());
        unsigned long type_length = static_cast<unsigned long>(record_type.size());

        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)name.c_str();
        bind[0].buffer_length = name_length;
        bind[0].length = &name_length;
        bind[0].is_null = 0;

        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (void*)record_type.c_str();
        bind[1].buffer_length = type_length;
        bind[1].length = &type_length;
        bind[1].is_null = 0;

        if (mysql_stmt_bind_param(stmt, bind)) {
            std::cerr << "Error: mysql_stmt_bind_param() failed: " << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        if (mysql_stmt_execute(stmt)) {
            std::cerr << "Error: mysql_stmt_execute() failed: " << mysql_stmt_error(stmt) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        mysql_stmt_close(stmt);
        return true;
    }

    // 获取指定用户的所有打卡记录
    std::vector<AttendanceItem> getRecordsByName(const std::string& name) {
        std::string query = "SELECT id, name, timestamp, record_type FROM AttendanceRecords "
                           "WHERE name = ? ORDER BY timestamp DESC";
                           
        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
            std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
            return {};
        }

        MYSQL_BIND bind[1];
        memset(bind, 0, sizeof(bind));
        
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)name.c_str();
        bind[0].buffer_length = static_cast<unsigned long>(name.size());

        if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)) {
            std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return {};
        }

        std::vector<AttendanceItem> records;
        MYSQL_RES* result = mysql_stmt_result_metadata(stmt);
        if (!result) {
            mysql_stmt_close(stmt);
            return records;
        }

        // 绑定结果集
        MYSQL_BIND result_bind[4];
        memset(result_bind, 0, sizeof(result_bind));
        
        AttendanceItem item;
        char name_buffer[100];
        char timestamp_buffer[20];
        char type_buffer[20];
        unsigned long name_length, timestamp_length, type_length;

        result_bind[0].buffer_type = MYSQL_TYPE_LONG;
        result_bind[0].buffer = &item.id;

        result_bind[1].buffer_type = MYSQL_TYPE_STRING;
        result_bind[1].buffer = name_buffer;
        result_bind[1].buffer_length = sizeof(name_buffer);
        result_bind[1].length = &name_length;

        result_bind[2].buffer_type = MYSQL_TYPE_STRING;
        result_bind[2].buffer = timestamp_buffer;
        result_bind[2].buffer_length = sizeof(timestamp_buffer);
        result_bind[2].length = &timestamp_length;

        result_bind[3].buffer_type = MYSQL_TYPE_STRING;
        result_bind[3].buffer = type_buffer;
        result_bind[3].buffer_length = sizeof(type_buffer);
        result_bind[3].length = &type_length;

        if (mysql_stmt_bind_result(stmt, result_bind)) {
            mysql_free_result(result);
            mysql_stmt_close(stmt);
            return records;
        }

        while (!mysql_stmt_fetch(stmt)) {
            item.name = std::string(name_buffer, name_length);
            item.timestamp = std::string(timestamp_buffer, timestamp_length);
            item.record_type = std::string(type_buffer, type_length);
            records.push_back(item);
        }

        mysql_free_result(result);
        mysql_stmt_close(stmt);
        return records;
    }

    // 删除指定ID的打卡记录
    bool deleteRecord(int record_id) {
        std::string query = "DELETE FROM AttendanceRecords WHERE id = ?";

        MYSQL_STMT* stmt = mysql_stmt_init(conn);
        if (!stmt || mysql_stmt_prepare(stmt, query.c_str(), static_cast<unsigned long>(query.size()))) {
            std::cerr << "Error: Failed to prepare statement: " << mysql_error(conn) << std::endl;
            return false;
        }

        MYSQL_BIND bind[1];
        memset(bind, 0, sizeof(bind));

        bind[0].buffer_type = MYSQL_TYPE_LONG;
        bind[0].buffer = &record_id;

        if (mysql_stmt_bind_param(stmt, bind) || mysql_stmt_execute(stmt)) {
            std::cerr << "Error: Failed to execute statement: " << mysql_error(conn) << std::endl;
            mysql_stmt_close(stmt);
            return false;
        }

        mysql_stmt_close(stmt);
        return true;
    }

private:
    MYSQL* conn;
};

#endif // ATTENDANCERECORD_H 