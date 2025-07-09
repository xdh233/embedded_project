#include "sensorthread.h"
#include <QDebug>
#ifdef __linux__
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
#endif

#define TEMP 0
#define HUMI 1

SensorThread::SensorThread(QObject *parent)
    : QThread(parent),
      m_collecting(false),
      m_running(true),
      m_sht11_fd(-1)
{
}

SensorThread::~SensorThread()
{
    requestStop();  // 使用新命名的方法
}

void SensorThread::startCollection()
{
    QMutexLocker locker(&m_mutex);
    m_collecting = true;
}

void SensorThread::stopCollection()
{
    QMutexLocker locker(&m_mutex);
    m_collecting = false;
}

bool SensorThread::isCollecting() const
{
    QMutexLocker locker(&m_mutex);
    return m_collecting;
}

void SensorThread::requestStop()  // 实现请求停止方法
{
    {
        QMutexLocker locker(&m_mutex);
        m_running = false;
    }
    wait();  // 等待线程结束
}

void SensorThread::run()
{
#ifdef __linux__
    m_sht11_fd = open("/dev/sht11", O_RDWR | O_NONBLOCK);
    if (m_sht11_fd < 0) {
        qWarning() << "Initialize failed:" << strerror(errno);
        return;
    }
#endif

    qDebug() << "SensorThread started";

    while (true) {
        // 检查是否应该退出
        {
            QMutexLocker locker(&m_mutex);
            if (!m_running) break;
        }

        // 检查是否应该收集数据
        bool collecting = isCollecting();

        if (collecting) {
            float temp = readTemperature();
            float hum = readHumidity();
            emit dataReceived(temp, hum);
        }

        msleep(1000);
    }

#ifdef __linux__
    if (m_sht11_fd >= 0) {
        close(m_sht11_fd);
    }
#endif

    qDebug() << "SensorThread finished";
}

// 保持 readTemperature() 和 readHumidity() 不变
float SensorThread::readTemperature()
{
    unsigned int value_t = 0;
#ifdef __linux__
    ioctl(m_sht11_fd, TEMP);
    if (read(m_sht11_fd, &value_t, sizeof(value_t)) < 0) {
        qWarning() << "读取温度失败";
        return 0.0f;
    }

    value_t &= 0x3fff; // 14位数据
    float temp = value_t * 0.01f - 40.0f; // 转换为实际温度
    return temp;
#else
    return 20.0f + (qrand() % 100) / 10.0f;
#endif
}

float SensorThread::readHumidity()
{
#ifdef __linux__
    unsigned int value_h = 0;
    ioctl(m_sht11_fd, HUMI);
    if (read(m_sht11_fd, &value_h, sizeof(value_h)) < 0) {
        qWarning() << "读取湿度失败";
        return 0.0f;
    }

    value_h &= 0xfff; // 12位数据
    float hum = -0.40f + 0.0405f * value_h - 0.0000028f * value_h * value_h; // 简化计算公式
    return qBound(0.1f, hum, 100.0f); // 限制在0.1-100%范围内
#else
    return 40.0f + (qrand() % 400) / 10.0f;
#endif
}
