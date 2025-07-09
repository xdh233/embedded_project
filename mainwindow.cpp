#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUI();
    setupDatabase();

    sensorThread = new SensorThread(this);
    connect(sensorThread, SIGNAL(dataReceived(float,float,float)),
            this, SLOT(onSensorDataReceived(float,float,float)));

    displayTimer = new QTimer(this);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(updateDisplay()));
    displayTimer->start(1000);

    sensorThread->start();
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    // 实时数据显示
    QHBoxLayout *dataLayout = new QHBoxLayout();
    tempLabel = new QLabel("温度: --°C");
    humLabel = new QLabel("湿度: --%");
    smokeLabel = new QLabel("烟雾: --ppm");
    dataLayout->addWidget(tempLabel);
    dataLayout->addWidget(humLabel);
    dataLayout->addWidget(smokeLabel);

    // 图表
    chartWidget = new ChartWidget();

    // 日志
    logDisplay = new QTextEdit();
    logDisplay->setReadOnly(true);

    layout->addLayout(dataLayout);
    layout->addWidget(chartWidget);
    layout->addWidget(logDisplay);

    setCentralWidget(central);
}

void MainWindow::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sensor_data.db");

    if (!db.open()) {
        logDisplay->append("无法打开数据库!");
        return;
    }

    // 创建表
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS sensor_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "timestamp DATETIME, "
               "temperature REAL, "
               "humidity REAL, "
               "smoke REAL)");
}

void MainWindow::saveToDatabase(float temp, float hum, float smoke)
{
    QSqlQuery query;
    query.prepare("INSERT INTO sensor_data (timestamp, temperature, humidity, smoke) "
                  "VALUES (:timestamp, :temp, :hum, :smoke)");
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":temp", temp);
    query.bindValue(":hum", hum);
    query.bindValue(":smoke", smoke);

    if (!query.exec()) {
        logDisplay->append("保存数据失败: " + query.lastError().text());
    }
}

void MainWindow::onSensorDataReceived(float temp, float hum, float smoke)
{
    // 创建一个 SensorData 对象
       SensorData data;
       data.timestamp = QDateTime::currentDateTime();
       data.temperature = temp;
       data.humidity = hum;
       data.smokeLevel = smoke;

    // 更新图表
    chartWidget->addDataPoint(data);

    // 保存到数据库
    saveToDatabase(temp, hum, smoke);

    // 更新显示
    tempLabel->setText(QString("温度: %1°C").arg(temp, 0, 'f', 1));
    humLabel->setText(QString("湿度: %1%").arg(hum, 0, 'f', 1));
    smokeLabel->setText(QString("烟雾: %1ppm").arg(smoke, 0, 'f', 1));
}
void MainWindow::updateDisplay()
{
    // 空实现，仅用于定时触发其他操作
    // 或调用 onSensorDataReceived 的显示逻辑
}
MainWindow::~MainWindow()
{
    sensorThread->quit();
    sensorThread->wait();
    db.close();
}
