#include "chartwidget.h"
#include <QPaintEvent>
#include <QDebug>
#include <QApplication>
#include <QGridLayout>
#include <cmath>

// ============== SingleChartWidget 实现 ==============

SingleChartWidget::SingleChartWidget(ChartType type, QWidget *parent)
    : QWidget(parent)
    , m_chartType(type)
    , m_realTimeMode(true)
    , m_maxDataPoints(50)
    , m_marginLeft(60)  // 直接初始化为新值
    , m_marginRight(60)
    , m_marginTop(40)
    , m_marginBottom(60)
    , m_minValue(0)
    , m_maxValue(100)
    , m_gridColor(QColor(220, 220, 220))
    , m_axisColor(Qt::black)
    , m_textColor(Qt::black)
{
    // 根据类型设置颜色
    m_chartColor = (type == TEMPERATURE) ? Qt::red : Qt::blue;

    setupUI();
    setMinimumSize(300, 200);
}
void SingleChartWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题和当前值显示
    m_titleLabel = new QLabel(getTypeString());  // 初始化标题标签
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 24px;");

    m_currentValueLabel = new QLabel("-- " + getUnitString());
    m_currentValueLabel->setStyleSheet("font-size: 24px; color: gray;");
    m_currentValueLabel->setAlignment(Qt::AlignRight);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_currentValueLabel);

    mainLayout->addLayout(headerLayout);
    mainLayout->addStretch();  // 图表绘制区域将占据剩余空间
}

void SingleChartWidget::addDataPoint(const SensorData &data)
{
    m_dataPoints.append(data);

    // 实时模式下限制数据点数量
    if (m_realTimeMode && m_dataPoints.size() > m_maxDataPoints) {
        m_dataPoints.removeFirst();
    }

    // 更新当前值显示
    double currentValue = getValueFromData(data);
    m_currentValueLabel->setText(QString::number(currentValue, 'f', 1) + " " + getUnitString());

    // 根据值的范围设置颜色
    QColor valueColor = m_chartColor;
    if (m_chartType == HUMIDITY) {
        if (currentValue > 70) valueColor = Qt::blue;
        else if (currentValue < 30) valueColor = Qt::red;
    }

    m_currentValueLabel->setStyleSheet(QString("font-size: 24px; color: %1; font-weight: bold;")
                                      .arg(valueColor.name()));

    // 更新显示信息
    m_infoLabel->setText(tr("数据点: %1").arg(m_dataPoints.size()));

    // 更新数据范围
    updateScales();

    // 触发重绘
    update();
}

void SingleChartWidget::clearData()
{
    m_dataPoints.clear();
    m_currentValueLabel->setText("-- " + getUnitString());
    m_currentValueLabel->setStyleSheet("font-size: 12px; color: gray;");
    m_infoLabel->setText(tr("数据点: 0"));
    update();
}

void SingleChartWidget::setRealTimeMode(bool enabled)
{
    m_realTimeMode = enabled;
}

void SingleChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 设置绘图区域
    m_chartRect = QRect(m_marginLeft, m_marginTop,
                       width() - m_marginLeft - m_marginRight,
                       height() - m_marginTop - m_marginBottom);

    // 绘制图表
    drawChart(painter);
}

void SingleChartWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    update();
}

void SingleChartWidget::drawChart(QPainter &painter)
{
    // 绘制背景
    painter.fillRect(m_chartRect, Qt::white);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(m_chartRect);

    if (m_dataPoints.isEmpty()) {
        // 没有数据时显示提示
        painter.setPen(m_textColor);
        painter.drawText(m_chartRect, Qt::AlignCenter, tr("暂无数据"));
        return;
    }

    // 绘制网格
    drawGrid(painter);

    // 绘制坐标轴
    drawAxes(painter);

    // 绘制数据
    drawData(painter);
}

void SingleChartWidget::drawGrid(QPainter &painter)
{
    painter.setPen(QPen(m_gridColor, 1));

    // 垂直网格线
    int gridLines = 8;
    for (int i = 1; i < gridLines; ++i) {
        int x = m_chartRect.left() + (m_chartRect.width() * i) / gridLines;
        painter.drawLine(x, m_chartRect.top(), x, m_chartRect.bottom());
    }

    // 水平网格线
    for (int i = 1; i < gridLines; ++i) {
        int y = m_chartRect.top() + (m_chartRect.height() * i) / gridLines;
        painter.drawLine(m_chartRect.left(), y, m_chartRect.right(), y);
    }
}

void SingleChartWidget::drawAxes(QPainter &painter)
{
    painter.setPen(QPen(m_axisColor, 2));

    // Y轴
    painter.drawLine(m_chartRect.left(), m_chartRect.top(),
                    m_chartRect.left(), m_chartRect.bottom());

    // X轴
    painter.drawLine(m_chartRect.left(), m_chartRect.bottom(),
                    m_chartRect.right(), m_chartRect.bottom());

    // Y轴刻度和标签
    painter.setPen(m_textColor);
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);

    int tickCount = 5;
    for (int i = 0; i <= tickCount; ++i) {
        double value = m_minValue + (m_maxValue - m_minValue) * i / tickCount;
        int y = m_chartRect.bottom() - (m_chartRect.height() * i) / tickCount;

        // 刻度线
        painter.setPen(m_axisColor);
        painter.drawLine(m_chartRect.left() - 5, y, m_chartRect.left(), y);

        // 标签
        painter.setPen(m_textColor);
        QString label = QString::number(value, 'f', 1);
        QRect textRect(0, y - 10, m_marginLeft - 10, 20);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // X轴标签（时间）
    if (!m_dataPoints.isEmpty()) {
        int timeTickCount = 4;
        for (int i = 0; i <= timeTickCount; ++i) {
            int x = m_chartRect.left() + (m_chartRect.width() * i) / timeTickCount;

            // 刻度线
            painter.setPen(m_axisColor);
            painter.drawLine(x, m_chartRect.bottom(), x, m_chartRect.bottom() + 5);

            // 时间标签
            if (i < m_dataPoints.size()) {
                painter.setPen(m_textColor);
                int dataIndex = qMax(0, qMin(m_dataPoints.size() - 1,
                                            (m_dataPoints.size() - 1) * i / timeTickCount));
                QString timeLabel = m_dataPoints[dataIndex].timestamp.toString("hh:mm:ss");
                QRect textRect(x - 55, m_chartRect.bottom() + 10, 120, 25);
                painter.drawText(textRect, Qt::AlignCenter, timeLabel);
            }
        }
    }
}

void SingleChartWidget::drawData(QPainter &painter)
{
    if (m_dataPoints.size() < 2) return;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(m_chartColor, 2));

    QList<QPointF> points;

    for (int i = 0; i < m_dataPoints.size(); ++i) {
        double value = getValueFromData(m_dataPoints[i]);

        // 转换为屏幕坐标
        double x = m_chartRect.left() + (double)(m_chartRect.width() * i) / (m_dataPoints.size() - 1);
        double y = m_chartRect.bottom() - (value - m_minValue) * m_chartRect.height() / (m_maxValue - m_minValue);

        points.append(QPointF(x, y));
    }

    // 绘制曲线
    for (int i = 1; i < points.size(); ++i) {
        painter.drawLine(points[i-1], points[i]);
    }

    // 绘制数据点
    painter.setBrush(m_chartColor);
    for (int i = 0; i < points.size(); ++i) {
        painter.drawEllipse(points[i], 3, 3);
    }

    // 高亮最新数据点
    if (!points.isEmpty()) {
        painter.setPen(QPen(m_chartColor, 3));
        painter.setBrush(Qt::white);
        painter.drawEllipse(points.last(), 5, 5);
    }
}

void SingleChartWidget::updateScales()
{
    if (m_dataPoints.isEmpty()) return;

    m_minValue = 999999;
    m_maxValue = -999999;

    for (int i = 0; i < m_dataPoints.size(); ++i) {
        double value = getValueFromData(m_dataPoints[i]);
        if (value < m_minValue) m_minValue = value;
        if (value > m_maxValue) m_maxValue = value;
    }

    // 添加一些边距
    double range = m_maxValue - m_minValue;
    if (range < 1) range = 1; // 避免除零

    m_minValue -= range * 0.1;
    m_maxValue += range * 0.1;

    // 根据类型设置合理的范围
    if (m_chartType == HUMIDITY) {
        if (m_minValue < 0) m_minValue = 0;
        if (m_maxValue > 100) m_maxValue = 100;
    }
}

double SingleChartWidget::getValueFromData(const SensorData &data) const
{
    switch (m_chartType) {
        case TEMPERATURE: return data.temperature;
        case HUMIDITY: return data.humidity;
        default: return 0;
    }
}

QString SingleChartWidget::getUnitString() const
{
    switch (m_chartType) {
        case TEMPERATURE: return "°C";
        case HUMIDITY: return "%";
        default: return "";
    }
}

QString SingleChartWidget::getTypeString() const
{
    switch (m_chartType) {
        case TEMPERATURE: return tr("温度");
        case HUMIDITY: return tr("湿度");
        default: return "";
    }
}

QColor SingleChartWidget::getChartColor() const
{
    return m_chartColor;
}

// ============== ChartWidget 实现 ==============

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_displayMode(0)
{
    setupUI();
    setMinimumSize(400, 300);
}

void ChartWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 控制面板
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(new QLabel(tr("显示模式:")));

    m_displayModeCombo = new QComboBox();
    m_displayModeCombo->addItem(tr("分离显示（推荐）"));
    m_displayModeCombo->addItem(tr("仅温度"));
    m_displayModeCombo->addItem(tr("仅湿度"));
    m_displayModeCombo->setStyleSheet("...");  // 保留原有样式

    connect(m_displayModeCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onChartTypeChanged()));

    controlLayout->addWidget(m_displayModeCombo);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    // 创建图表区域
    QWidget *chartsContainer = new QWidget();
    QHBoxLayout *chartsLayout = new QHBoxLayout(chartsContainer);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    chartsLayout->setSpacing(10);

    // 创建两个图表
    m_temperatureChart = new SingleChartWidget(SingleChartWidget::TEMPERATURE);
    m_humidityChart = new SingleChartWidget(SingleChartWidget::HUMIDITY);

    // 设置16:9比例 (15.5cm x 9cm)
    const int width = static_cast<int>(15.5 * 96 / 2.54);  // 厘米转像素
    const int height = static_cast<int>(9 * 96 / 2.54);

    m_temperatureChart->setFixedSize(width, height);
    m_humidityChart->setFixedSize(width, height);

    chartsLayout->addWidget(m_temperatureChart);
    chartsLayout->addWidget(m_humidityChart);
    mainLayout->addWidget(chartsContainer);
}

void ChartWidget::addDataPoint(const SensorData &data)
{
    m_temperatureChart->addDataPoint(data);
    m_humidityChart->addDataPoint(data);
}

void ChartWidget::setChartType(int type)
{
    m_displayMode = type;
    if (m_displayMode > 2) m_displayMode = 0;

    m_displayModeCombo->setCurrentIndex(m_displayMode);
    onChartTypeChanged();
}

void ChartWidget::clearData()
{
    m_temperatureChart->clearData();
    m_humidityChart->clearData();
}

void ChartWidget::setRealTimeMode(bool enabled)
{
    m_temperatureChart->setRealTimeMode(enabled);
    m_humidityChart->setRealTimeMode(enabled);
}

void ChartWidget::onChartTypeChanged()
{
    m_displayMode = m_displayModeCombo->currentIndex();

    // 根据显示模式控制图表可见性
    switch (m_displayMode) {
        case 0: // 分离显示所有
            m_temperatureChart->setVisible(true);
            m_humidityChart->setVisible(true);
            break;
        case 1: // 仅温度
            m_temperatureChart->setVisible(true);
            m_humidityChart->setVisible(false);
            break;
        case 2: // 仅湿度
            m_temperatureChart->setVisible(false);
            m_humidityChart->setVisible(true);
            break;
    }
}
