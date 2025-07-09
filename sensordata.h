#ifndef SENSORDATA_H
#define SENSORDATA_H

#include <QDateTime>
#include <QString>

// 传感器数据结构
struct SensorData {
    double temperature;     // 温度 (°C)
    double humidity;       // 湿度 (%)
    double smokeLevel;     // 烟雾浓度
    QDateTime timestamp;   // 时间戳

    SensorData() : temperature(0.0), humidity(0.0), smokeLevel(0.0) {
        timestamp = QDateTime::currentDateTime();
    }

    SensorData(double temp, double hum, double smoke)
        : temperature(temp), humidity(hum), smokeLevel(smoke) {
        timestamp = QDateTime::currentDateTime();
    }
};

// 报警信息结构
struct AlarmInfo {
    QString type;          // 报警类型
    QString message;       // 报警信息
    QDateTime timestamp;   // 报警时间
    bool isActive;        // 是否活跃

    AlarmInfo() : isActive(false) {}

    AlarmInfo(const QString &alarmType, const QString &msg)
        : type(alarmType), message(msg), isActive(true) {
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
    SensorStatus smokeStatus;
    bool cloudConnected;

    SystemStatus() : temperatureStatus(SENSOR_NORMAL),
                    humidityStatus(SENSOR_NORMAL),
                    smokeStatus(SENSOR_NORMAL),
                    cloudConnected(false) {}
};

// 报警阈值设置
struct AlarmThresholds {
    double maxTemperature;
    double minTemperature;
    double maxHumidity;
    double minHumidity;
    double maxSmokeLevel;

    AlarmThresholds() : maxTemperature(35.0), minTemperature(10.0),
                       maxHumidity(80.0), minHumidity(20.0),
                       maxSmokeLevel(50.0) {}
};

#endif // SENSORDATA_H
