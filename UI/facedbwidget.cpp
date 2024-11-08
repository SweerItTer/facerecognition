#include "facedbwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <iostream>

FaceDBWidget::FaceDBWidget(QWidget *parent)
    : QWidget(parent), conn(mysql_init(nullptr))
{
    
    // 初始化表格
    attendanceTable = new QTableWidget(this);
    faceFeaturesTable = new QTableWidget(this);
    idInformationTable = new QTableWidget(this);

    // 初始化刷新按钮
    refreshButton = new QPushButton("Refresh", this);
    connect(refreshButton, &QPushButton::clicked, this, &FaceDBWidget::refresh);

    // 设置布局，先放按钮，然后是表格
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(refreshButton);  // 添加刷新按钮
    layout->addWidget(attendanceTable);
    layout->addWidget(faceFeaturesTable);
    layout->addWidget(idInformationTable);
    setLayout(layout);

}

FaceDBWidget::~FaceDBWidget()
{
    if (conn) {
        mysql_close(conn);
    }
    delete dbInfo;
    delete attendanceTable;
    delete faceFeaturesTable;
    delete idInformationTable;
    delete refreshButton;
}

void FaceDBWidget::setDBInfo(DataDBInfo *dbinfo)
{
    dbInfo = dbinfo;
    if (!connectToDatabase(dbInfo)) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the FaceDB database.");
        return;
    }
    // 初次加载数据
    refresh();
}

bool FaceDBWidget::connectToDatabase(const DataDBInfo *dbInfo)
{
    conn = mysql_real_connect(conn, 
                              dbInfo->host.c_str(), 
                              dbInfo->user.c_str(), 
                              dbInfo->password.c_str(), 
                              dbInfo->database.c_str(), 
                              dbInfo->port, nullptr, 0);
    if (!conn) {
        std::cerr << "Database connection failed: " << mysql_error(conn) << std::endl;
        ConncetStatus = false;
        return ConncetStatus;
    }
    ConncetStatus = true;
    return ConncetStatus;
}

void FaceDBWidget::refresh()
{
    if(!ConncetStatus) {
        QMessageBox::critical(this, "Database Error", "Failed to connect to the FaceDB database.");
        return;
    }
    // 清空表格内容
    attendanceTable->clearContents();
    faceFeaturesTable->clearContents();
    idInformationTable->clearContents();

    // 清空表格行数
    attendanceTable->setRowCount(0);
    faceFeaturesTable->setRowCount(0);
    idInformationTable->setRowCount(0);

    // 重新加载数据库数据
    loadTableData("AttendanceRecords", attendanceTable);
    loadTableData("FaceFeatures", faceFeaturesTable);
    loadTableData("IDinformation", idInformationTable);
}

void FaceDBWidget::loadTableData(const QString &tableName, QTableWidget *tableWidget)
{
    QString queryStr = QString("SELECT * FROM %1").arg(tableName);
    if (mysql_query(conn, queryStr.toStdString().c_str())) {
        std::cerr << "Query execution failed: " << mysql_error(conn) << std::endl;
        return;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    if (!result) {
        std::cerr << "Result retrieval failed: " << mysql_error(conn) << std::endl;
        return;
    }

    int numFields = mysql_num_fields(result);
    tableWidget->setColumnCount(numFields);

    MYSQL_FIELD *fields = mysql_fetch_fields(result);
    for (int i = 0; i < numFields; i++) {
        tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(fields[i].name));
    }

    int row = 0;
    MYSQL_ROW dbRow;
    while ((dbRow = mysql_fetch_row(result))) {
        tableWidget->insertRow(row);
        for (int col = 0; col < numFields; col++) {
            tableWidget->setItem(row, col, new QTableWidgetItem(dbRow[col] ? dbRow[col] : "NULL"));
        }
        row++;
    }

    mysql_free_result(result);
}
