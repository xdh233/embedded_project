#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置中文编码支持
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    // 设置应用程序样式
    app.setStyle("Fusion");

    // 创建并显示主窗口
    MainWindow window;
    window.setWindowTitle(QObject::tr("智能家居监控系统"));
    window.resize(800, 600);
    window.show();

    return app.exec();
}
