#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QTextEdit>
#include "sensorthread.h"
#include "chartwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onSensorDataReceived(float temperature, float humidity, float smoke);
    void updateDisplay();

private:
    void setupUI();
    void setupDatabase();
    void saveToDatabase(float temp, float hum, float smoke);

    SensorThread *sensorThread;
    QTimer *displayTimer;

    // UI组件
    ChartWidget *chartWidget;
    QLabel *tempLabel, *humLabel, *smokeLabel;
    QTextEdit *logDisplay;

    QSqlDatabase db;
};

#endif
