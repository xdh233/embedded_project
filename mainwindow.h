#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTextEdit>
#include <QTableWidget>
#include <QTabWidget>
#include <QDateEdit>
#include <QTimer>
#include <QLabel>
#include "sensorthread.h"
#include "chartwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onSensorDataReceived(float temperature, float humidity);
    void updateDisplay();
    void onQueryHistoryData();
    void onRefreshHistoryData();
    void onToggleCollection();
private:
    void setupUI();
    void setupDatabase();
    void saveToDatabase(float temp, float hum);
    void loadHistoryData();
    void setupRealtimeTab();    // 声明实时监控页面初始化
    void setupHistoryTab();     // 声明历史记录页面初始化
    void updateHistoryTable();

    QPushButton *collectionButton; // 添加这个按钮

    SensorThread *sensorThread;
    QTimer *displayTimer;

    // UI组件
    QTabWidget *tabWidget;

    // 实时监控页面
    QWidget *realtimeWidget;
    ChartWidget *chartWidget;
    QLabel *tempLabel, *humLabel;
    QTextEdit *logDisplay;

    // 历史记录页面
    QWidget *historyWidget;
    QTableWidget *historyTable;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QPushButton *queryButton;
    QPushButton *refreshButton;

    QSqlDatabase db;
    QList<SensorData> historyData;


};

#endif
