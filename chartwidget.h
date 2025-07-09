#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QList>
#include <QPointF>
#include <QDateTime>
#include "sensordata.h"

class SingleChartWidget : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        TEMPERATURE = 0,
        HUMIDITY = 1
    };

    explicit SingleChartWidget(ChartType type, QWidget *parent = 0);

    // 添加数据点
    void addDataPoint(const SensorData &data);

    // 清空数据
    void clearData();

    // 设置是否实时模式
    void setRealTimeMode(bool enabled);

    // 获取图表类型
    ChartType getChartType() const { return m_chartType; }

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void setupUI();
    void drawChart(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawData(QPainter &painter);
    void updateScales();
    double getValueFromData(const SensorData &data) const;
    QString getUnitString() const;
    QString getTypeString() const;
    QColor getChartColor() const;

    // UI组件
    QLabel *m_titleLabel;
    QLabel *m_currentValueLabel;
    QLabel *m_infoLabel;

    // 数据存储
    QList<SensorData> m_dataPoints;

    // 图表设置
    ChartType m_chartType;
    bool m_realTimeMode;
    int m_maxDataPoints;

    // 绘图区域
    QRect m_chartRect;
    int m_marginLeft;
    int m_marginRight;
    int m_marginTop;
    int m_marginBottom;

    // 数据范围
    double m_minValue;
    double m_maxValue;

    // 颜色配置
    QColor m_chartColor;
    QColor m_gridColor;
    QColor m_axisColor;
    QColor m_textColor;
};

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = 0);

    // 添加数据点
    void addDataPoint(const SensorData &data);

    // 设置图表类型（保持兼容性）
    void setChartType(int type);

    // 清空数据
    void clearData();

    // 设置是否实时模式
    void setRealTimeMode(bool enabled);

private slots:
    void onChartTypeChanged();

private:
    void setupUI();

    // UI组件
    QComboBox *m_displayModeCombo;

    // 两个独立的图表
    SingleChartWidget *m_temperatureChart;
    SingleChartWidget *m_humidityChart;

    // 当前显示模式：0=分离显示, 1=只显示温度, 2=只显示湿度
    int m_displayMode;
};

#endif // CHARTWIDGET_H
