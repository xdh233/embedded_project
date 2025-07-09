#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    // 设置中文编码支持
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    MainWindow window;
    window.show();
    return app.exec();
}
