#ifndef FACEDBWIDGET_H
#define FACEDBWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <mysql.h>

struct DataDBInfo {
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    unsigned int port;
};


class FaceDBWidget : public QWidget
{
    Q_OBJECT

public:
    FaceDBWidget(QWidget *parent = nullptr);
    ~FaceDBWidget();

    void setDBInfo(DataDBInfo *dbinfo);

private slots:
    void refresh(); // 新增槽函数用于刷新

private:
    MYSQL *conn;  // MySQL 连接对象
    QTableWidget *attendanceTable;
    QTableWidget *faceFeaturesTable;
    QTableWidget *idInformationTable;
    QPushButton *refreshButton; // 刷新按钮

    DataDBInfo *dbInfo;

    bool ConncetStatus = false;

    bool connectToDatabase(const DataDBInfo *dbInfo);
    // bool connectToDatabase(const char *host, const char *user, const char *password, const char *database, unsigned int port);
    void loadTableData(const QString &tableName, QTableWidget *tableWidget);
};

#endif // FACEDBWIDGET_H
