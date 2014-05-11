#include "gcodesimplerwidget.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load("GCodeSimpler_zh_CN.qm");
    a.installTranslator(&translator);
    GCodeSimplerWidget w;
    w.show();

    return a.exec();
}
