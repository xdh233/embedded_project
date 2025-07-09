#include "alarmcontroller.h"
#include <QDebug>

AlarmController::AlarmController(QObject *parent)
    : QObject(parent)
    , m_alarming(false)
    , m_alarmDuration(5)
{
    m_alarmTimer = new QTimer(this);
    m_alarmTimer->setSingleShot(true);
    connect(m_alarmTimer, SIGNAL(timeout()), this, SLOT(onAlarmTimeout()));
}

AlarmController::~AlarmController()
{
    stopAlarm();
}

void AlarmController::triggerAlarm()
{
    if (m_alarming) return;

    m_alarming = true;
    ledOn();
    buzzerOn();
    m_alarmTimer->start(m_alarmDuration * 1000);
}

void AlarmController::stopAlarm()
{
    if (!m_alarming) return;

    m_alarming = false;
    m_alarmTimer->stop();
    ledOff();
    buzzerOff();
}

bool AlarmController::isAlarming() const
{
    return m_alarming;
}

void AlarmController::setAlarmDuration(int seconds)
{
    m_alarmDuration = (seconds > 1) ? seconds : 1;
}

void AlarmController::onAlarmTimeout()
{
    stopAlarm();
}

void AlarmController::ledOn()
{
    qDebug() << "LED开启";
}

void AlarmController::ledOff()
{
    qDebug() << "LED关闭";
}

void AlarmController::buzzerOn()
{
    qDebug() << "蜂鸣器开启";
}

void AlarmController::buzzerOff()
{
    qDebug() << "蜂鸣器关闭";
}
