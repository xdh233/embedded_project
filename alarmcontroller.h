#ifndef ALARMCONTROLLER_H
#define ALARMCONTROLLER_H

#include <QObject>
#include <QTimer>

class AlarmController : public QObject
{
    Q_OBJECT

public:
    explicit AlarmController(QObject *parent = 0);
    ~AlarmController();

    void triggerAlarm();
    void stopAlarm();
    bool isAlarming() const;
    void setAlarmDuration(int seconds);

private slots:
    void onAlarmTimeout();

private:
    void ledOn();
    void ledOff();
    void buzzerOn();
    void buzzerOff();

    QTimer *m_alarmTimer;
    bool m_alarming;
    int m_alarmDuration;
};

#endif // ALARMCONTROLLER_H
