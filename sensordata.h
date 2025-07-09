#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <QDateTime>
#include <QString>

// 传感器数据结构
struct SensorData {
    double temperature;     // 温度 (°C)
    double humidity;       // 湿度 (%)
    QDateTime timestamp;   // 时间戳

    SensorData() : temperature(0.0), humidity(0.0) {
        timestamp = QDateTime::currentDateTime();
    }

    SensorData(double temp, double hum)
        : temperature(temp), humidity(hum) {
        timestamp = QDateTime::currentDateTime();
    }
};

// 传感器状态枚举
enum SensorStatus {
    SENSOR_NORMAL = 0,
    SENSOR_OFFLINE = 1,
    SENSOR_ERROR = 2
};

// 系统状态结构
struct SystemStatus {
    SensorStatus temperatureStatus;
    SensorStatus humidityStatus;

    SystemStatus() : temperatureStatus(SENSOR_NORMAL),
                    humidityStatus(SENSOR_NORMAL) {}
};

// 报警阈值设置
struct AlarmThresholds {
    double maxTemperature;
    double minTemperature;
    double maxHumidity;
    double minHumidity;

    AlarmThresholds() : maxTemperature(35.0), minTemperature(10.0),
                       maxHumidity(80.0), minHumidity(20.0) {}
};

#endif // SENSORDATA_H
