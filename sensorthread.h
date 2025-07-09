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

    void startCollection();
    void stopCollection();
    bool isCollecting() const;  // 保持 const 修饰
    void requestStop();
signals:
    void dataReceived(float temperature, float humidity);

protected:
    void run();

private:
    mutable QMutex m_mutex;  // 关键修改：添加 mutable
    volatile bool m_collecting;  // 添加 volatile
    volatile bool m_running;     // 添加 volatile
    int m_sht11_fd;

    float readTemperature();
    float readHumidity();
};

#endif // SENSORTHREAD_H
