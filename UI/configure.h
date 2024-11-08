#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QWidget>
#include <QDebug>

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDialog>
#include <QStandardPaths>

#include <unordered_map>

class Configure : public QWidget
{
	Q_OBJECT

public:
	explicit Configure(){}
	virtual ~Configure(){}

	void databaseConfigure(){
		// 创建一个 QDialog 作为弹出窗口
		QDialog dialog(this);
		dialog.setWindowTitle("Modify Database Configuration");

		// 创建布局和控件
		QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
		QFormLayout *formLayout = new QFormLayout();


		password->setEchoMode(QLineEdit::Password);

		// 加载配置
		loadConfig(connectionName, host, port, userName, password);

		// 将控件添加到布局
		formLayout->addRow("Connection Name:", connectionName);
		formLayout->addRow("Host:", host);
		formLayout->addRow("Port:", port);
		formLayout->addRow("User Name:", userName);
		formLayout->addRow("Password:", password);
		mainLayout->addLayout(formLayout);

		// 添加对话框按钮（OK 和 Cancel）
		QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		mainLayout->addWidget(buttonBox);

		// 连接 OK 和 Cancel 按钮的槽函数
		connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
		connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

		// 如果用户点击 OK 按钮
		if (dialog.exec() == QDialog::Accepted) {
			// 保存配置
			saveConfig(connectionName->text(), host->text(), port->text(), userName->text(), password->text());

			// 模拟保存配置，可以替换为实际逻辑
			QMessageBox::information(this, "Configuration Saved", "The database configuration has been saved.");
		}
	}
		
	std::unordered_map<std::string, std::string> getDatabaseConfig() {
		std::unordered_map<std::string, std::string> config;
		loadConfigToStruct(config);
		return config;
	}


private:
	QLineEdit *connectionName = new QLineEdit();
	QLineEdit *host = new QLineEdit();
	QLineEdit *port = new QLineEdit();
	QLineEdit *userName = new QLineEdit();
	QLineEdit *password = new QLineEdit();

	void loadConfigToStruct(std::unordered_map<std::string, std::string>& config) {
		QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
		QDir().mkpath(configPath);
		QFile configFile(configPath + "/db_config.json");

		qDebug() << "Config Path: " << configPath;

		// 打开配置文件
		if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QByteArray jsonData = configFile.readAll();
			configFile.close();

			QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
			if (jsonDoc.isNull()) {
				QMessageBox::warning(nullptr, "Error", "Failed to parse the configuration file.");
				return;
			}

			QJsonObject jsonObj = jsonDoc.object();

			// 设置unordered_map的值
			// 使用 toStdString().c_str() 将 QString 转为 const char* 类型
			config["host"] = jsonObj.value("host").toString("localhost").toStdString();
			config["port"] = jsonObj.value("port").toString("3306").toStdString();
			config["userName"] = jsonObj.value("userName").toString("root").toStdString();
			config["password"] = jsonObj.value("password").toString("").toStdString();
		} else {
			// 如果配置文件不存在，使用默认值
			config["host"] = "localhost";
			config["port"] = "3306";
			config["userName"] = "root";
			config["password"] = "";
		}
	}

	void loadConfig(QLineEdit* connectionName, QLineEdit* host, QLineEdit* port, QLineEdit* userName, QLineEdit* password) {
		QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
		QDir().mkpath(configPath);
		QFile configFile(configPath + "/db_config.json");
		qDebug() << configPath;

		if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QByteArray jsonData = configFile.readAll();
			configFile.close();

			QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
			if (jsonDoc.isNull()) {
				QMessageBox::warning(this, "Error", "Failed to parse the configuration file.");
				return;
			}

			QJsonObject jsonObj = jsonDoc.object();

			// 设置值到输入框，处理缺少字段情况
			connectionName->setText(jsonObj.value("connectionName").toString(""));
			host->setText(jsonObj.value("host").toString("localhost"));
			port->setText(jsonObj.value("port").toString("3306"));
			userName->setText(jsonObj.value("userName").toString("root"));
			password->setText(jsonObj.value("password").toString());
		} else {
			// 如果配置文件不存在，使用默认值
			connectionName->setText("");
			host->setText("localhost");
			port->setText("3306");
			userName->setText("root");
			password->setText("");
		}
	}

	void saveConfig(const QString& connectionName, const QString& host, const QString& port, const QString& userName, const QString& password) {
		QJsonObject jsonObj;
		jsonObj["connectionName"] = connectionName;
		jsonObj["host"] = host;
		jsonObj["port"] = port;
		jsonObj["userName"] = userName;
		jsonObj["password"] = password;  // 可以考虑加密保存

		QJsonDocument jsonDoc(jsonObj);

		QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
		QDir().mkpath(configPath);
		QFile configFile(configPath + "/db_config.json");

		if (configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			configFile.write(jsonDoc.toJson());
			configFile.close();
		} else {
			QMessageBox::warning(this, "Error", "Failed to save configuration.");
		}
	}


};
#endif // CONFIGURE_H
