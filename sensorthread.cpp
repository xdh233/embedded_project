#include "sensorthread.h"
#include <QDebug>
#ifdef __linux__
    #include <sys/ioctl.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

#define TEMP 0
#define HUMI 1

SensorThread::SensorThread(QObject *parent)
    : QThread(parent),
      m_sht11_fd(-1),
      m_smog_fd(-1),
      running(true)
{
}

SensorThread::~SensorThread()
{
    running = false;
    wait();

    if (m_sht11_fd >= 0) {
        #ifdef __linux__
        close(m_sht11_fd);
        #endif

    }

    if (m_smog_fd >= 0) {
        #ifdef __linux__
        close(m_smog_fd);
        #endif
    }
}

void SensorThread::run()
{
    // 初始化传感器
    #ifdef __linux__

    m_sht11_fd = open("/dev/sht11", O_RDWR);
    m_smog_fd = open("/dev/smog", O_RDONLY);

    if (m_sht11_fd < 0 || m_smog_fd < 0) {
        qWarning() << "传感器初始化失败";
        return;
    }

    while (running) {
        float temp = readTemperature();
        float hum = readHumidity();
        float smoke = readSmoke();

        emit dataReceived(temp, hum, smoke);

        msleep(1000); // 1秒采样间隔
    }
#endif
}

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
#endif
}

float SensorThread::readSmoke()
{
#ifdef __linux__

    int smog_cnt = 0;
    if (read(m_smog_fd, &smog_cnt, sizeof(smog_cnt)) < 0) {
        qWarning() << "读取烟雾传感器失败";
        return 0.0f;
    }
    return smog_cnt > 0 ? 1.0f : 0.0f;
#endif
}
