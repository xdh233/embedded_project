#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>  // 添加这行
#include <QFont>         // 确保包含QFont
#include <QDebug>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // 在构造函数中添加：
    QFont font;
    font.setPointSize(24);  // 基础字体放大到12pt（默认通常是9pt）
    QApplication::setFont(font);

    // 设置全局样式表（放大所有核心控件）
    this->setStyleSheet(
        "QWidget { font-size: 32px; }"  // 全局字体
        "QTabWidget::pane { border: none; }"
        "QTabBar::tab {"
        "   height: 64px;"      // 标签高度
        "   min-width: 120px;"  // 标签最小宽度
        "   font-size: 32px;"   // 标签字体
        "}"
        "QPushButton {"
        "   min-height: 60px;"  // 按钮高度
        "   min-width: 100px;"  // 按钮宽度
        "   font-size: 32px;"   // 按钮字体
        "}"
        "QDateEdit, QComboBox {"
        "   min-height: 60px;"  // 输入框高度
        "   font-size: 64px;"   // 输入框字体
        "}"
    );
    setupUI();

    setupDatabase();

    sensorThread = new SensorThread(this);
    connect(sensorThread, SIGNAL(dataReceived(float,float)),
            this, SLOT(onSensorDataReceived(float,float)));

    displayTimer = new QTimer(this);
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(update()));
    displayTimer->start(1000);

    sensorThread->start();
}

void MainWindow::setupUI()
{
    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet("QTabBar::tab { font-size: 32px; padding: 10px; }");
    setCentralWidget(tabWidget);

    // 实时监控页面
    setupRealtimeTab();

    // 历史记录页面
    setupHistoryTab();
}

void MainWindow::setupRealtimeTab()
{
    realtimeWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(realtimeWidget);

    // 创建顶部控制区域
    QWidget *controlWidget = new QWidget();
    QHBoxLayout *controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(0, 0, 0, 0); // 移除边距
    controlLayout->setSpacing(20); // 设置控件间距

    // 左侧数据标签
    QWidget *dataWidget = new QWidget();
    QHBoxLayout *dataLayout = new QHBoxLayout(dataWidget);
    dataLayout->setContentsMargins(0, 0, 0, 0);
    tempLabel = new QLabel("温度: --°C");
    humLabel = new QLabel("湿度: --%");
    tempLabel->setStyleSheet("font-size: 32px; font-weight: bold;");
    humLabel->setStyleSheet("font-size: 32px; font-weight: bold;");
    dataLayout->addWidget(tempLabel);
    dataLayout->addWidget(humLabel);

    // 右侧按钮
    collectionButton = new QPushButton(tr("开始收集数据"));
    collectionButton->setStyleSheet("font-size: 32px; min-height: 60px; min-width: 200px;");
    connect(collectionButton, SIGNAL(clicked()), this, SLOT(onToggleCollection()));

    // 添加到控制布局
    controlLayout->addWidget(dataWidget, 1); // 数据标签部分可拉伸
    controlLayout->addWidget(collectionButton, 0, Qt::AlignLeft); // 按钮左对齐

    // 图表和日志
    chartWidget = new ChartWidget();
    logDisplay = new QTextEdit();
    logDisplay->setReadOnly(true);
    logDisplay->setMaximumHeight(100);

    // 主布局
    layout->addWidget(controlWidget);
    layout->addWidget(chartWidget, 1);
    layout->addWidget(logDisplay);

    tabWidget->addTab(realtimeWidget, tr("实时监控"));
}

void MainWindow::setupHistoryTab()
{
    historyWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(historyWidget);
    layout->setSpacing(15);  // 增加布局间距

    // ================= 查询控制区域 =================
    QHBoxLayout *queryLayout = new QHBoxLayout();
    queryLayout->setSpacing(10);  // 控件间间距

    // 日期选择框（放大）
    startDateEdit = new QDateEdit(QDate::currentDate().addDays(-7));
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setMinimumHeight(35);
    startDateEdit->setStyleSheet("font-size: 14px;");

    endDateEdit = new QDateEdit(QDate::currentDate());
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setMinimumHeight(35);
    endDateEdit->setStyleSheet("font-size: 14px;");

    // 按钮（放大并加粗）
    queryButton = new QPushButton(tr("查询"));
    queryButton->setMinimumSize(120, 45);
    queryButton->setStyleSheet("font-size: 16px; font-weight: bold;");

    refreshButton = new QPushButton(tr("刷新"));
    refreshButton->setMinimumSize(120, 45);
    refreshButton->setStyleSheet("font-size: 16px; font-weight: bold;");

    // 连接信号槽
    connect(queryButton, SIGNAL(clicked()), this, SLOT(onQueryHistoryData()));
    connect(refreshButton, SIGNAL(clicked()), this, SLOT(onRefreshHistoryData()));
    // 添加到布局
    queryLayout->addWidget(new QLabel(tr("开始日期:")));
    queryLayout->addWidget(startDateEdit);
    queryLayout->addSpacing(10);  // 增加间距
    queryLayout->addWidget(new QLabel(tr("结束日期:")));
    queryLayout->addWidget(endDateEdit);
    queryLayout->addSpacing(20);  // 增加间距
    queryLayout->addWidget(queryButton);
    queryLayout->addWidget(refreshButton);
    queryLayout->addStretch();

    // ================= 历史数据表格 =================
    historyTable = new QTableWidget();
    historyTable->setColumnCount(3);
    historyTable->setHorizontalHeaderLabels(
        QStringList() << tr("时间") << tr("温度(°C)") << tr("湿度(%)")
    );

    // 列宽策略（重点修改）
    historyTable->horizontalHeader()->setStretchLastSection(false);
    historyTable->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);  // ✅ Qt4 使用 setResizeMode
    historyTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    historyTable->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    historyTable->setColumnWidth(0, 220);
    // 表格样式
    historyTable->setStyleSheet(
        "QTableWidget { font-size: 16px; } "
        "QHeaderView::section { font-size: 16px; padding: 8px; }"  // 表头字体加大
    );
    historyTable->verticalHeader()->setDefaultSectionSize(45);  // 行高
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ================= 整合布局 =================
    layout->addLayout(queryLayout);
    layout->addWidget(historyTable, 1);  // 表格占据剩余空间

    // 添加到Tab
    tabWidget->addTab(historyWidget, tr("历史记录"));
    tabWidget->setStyleSheet("QTabBar::tab { height: 30px; font-size: 18px; }");  // 放大Tab标签
}

void MainWindow::setupDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sensor_data.db");

    if (!db.open()) {
        logDisplay->append(tr("无法打开数据库!"));
        return;
    }

    // 创建表
    QSqlQuery query;
    query.exec("PRAGMA encoding = 'UTF-8';");  // 关键语句
    query.exec("CREATE TABLE IF NOT EXISTS sensor_data ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "timestamp DATETIME, "
               "temperature REAL, "
               "humidity REAL)");
}

void MainWindow::saveToDatabase(float temp, float hum)
{
    QSqlQuery query;
    query.prepare("INSERT INTO sensor_data (timestamp, temperature, humidity) "
                  "VALUES (:timestamp, :temp, :hum)");
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":temp", temp);
    query.bindValue(":hum", hum);

    if (!query.exec()) {
        logDisplay->append(tr("保存数据失败: ") + query.lastError().text());
    }
}

void MainWindow::onSensorDataReceived(float temp, float hum)
{
    SensorData data(temp, hum);

    // 更新图表
    chartWidget->addDataPoint(data);

    // 保存到数据库
    saveToDatabase(temp, hum);

    // 更新显示
    tempLabel->setText(QString(tr("温度: %1°C")).arg(temp, 0, 'f', 1));
    humLabel->setText(QString(tr("湿度: %1%")).arg(hum, 0, 'f', 1));

    // 记录日志
    QString logEntry = QString("[%1] T:%2°C H:%3%")
                      .arg(QTime::currentTime().toString("hh:mm:ss"))
                      .arg(temp, 0, 'f', 1)
                      .arg(hum, 0, 'f', 1);
    logDisplay->append(logEntry);
}

void MainWindow::loadHistoryData()
{
    QSqlQuery query;
    query.prepare("SELECT timestamp, temperature, humidity FROM sensor_data "
                  "WHERE DATE(timestamp) BETWEEN :start AND :end "
                  "ORDER BY timestamp DESC");
    query.bindValue(":start", startDateEdit->date());
    query.bindValue(":end", endDateEdit->date());

    if (!query.exec()) {
        QMessageBox::warning(this, tr("查询失败"), tr("无法查询历史数据: ") + query.lastError().text());
        return;
    }

    historyData.clear();
    while (query.next()) {
        SensorData data;
        data.timestamp = query.value(0).toDateTime();
        data.temperature = query.value(1).toDouble();
        data.humidity = query.value(2).toDouble();
        historyData.append(data);
    }

    updateHistoryTable();
}

void MainWindow::updateHistoryTable()
{
    historyTable->setRowCount(historyData.size());

    for (int i = 0; i < historyData.size(); ++i) {
        const SensorData &data = historyData[i];

        QTableWidgetItem *timeItem = new QTableWidgetItem(data.timestamp.toString("yyyy-MM-dd hh:mm:ss"));
        QTableWidgetItem *tempItem = new QTableWidgetItem(QString::number(data.temperature, 'f', 1));
        QTableWidgetItem *humItem = new QTableWidgetItem(QString::number(data.humidity, 'f', 1));

        historyTable->setItem(i, 0, timeItem);
        historyTable->setItem(i, 1, tempItem);
        historyTable->setItem(i, 2, humItem);
    }
}

void MainWindow::onQueryHistoryData()
{
    loadHistoryData();
}

void MainWindow::onRefreshHistoryData()
{
    loadHistoryData();
}

void MainWindow::updateDisplay()
{
    // 可以在这里添加周期性更新显示的逻辑
}

MainWindow::~MainWindow()
{
    if (sensorThread) {
        sensorThread->quit();
        sensorThread->wait(1000); // 等待最多1秒
        if (sensorThread->isRunning()) {
            sensorThread->terminate();
        }
        delete sensorThread;
    }
}
void MainWindow::onToggleCollection()
{
    qDebug() << "Toggle collection clicked";
    if (sensorThread->isCollecting()) {
        qDebug() << "Stopping collection";
        sensorThread->stopCollection();
        collectionButton->setText(tr("开始收集数据"));
    } else {
        qDebug() << "Starting collection";
        sensorThread->startCollection();
        collectionButton->setText(tr("停止收集数据"));
    }
}
