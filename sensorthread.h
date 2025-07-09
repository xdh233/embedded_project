#ifndef SENSORTHREAD_H
#define SENSORTHREAD_H

#include <QThread>
#include <QMutex>

class SensorThread : public QThread
{
    Q_OBJECT
public:
    explicit SensorThread(QObject *parent = 0);
    ~SensorThread();

signals:
    void dataReceived(float temperature, float humidity, float smoke);

protected:
    void run();

private:
    float readTemperature();
    float readHumidity();
    float readSmoke();

    int m_sht11_fd;  // 温湿度传感器文件描述符
    int m_smog_fd;   // 烟雾传感器文件描述符
    QMutex m_mutex;
    bool running;
};

#endif // SENSORTHREAD_H
