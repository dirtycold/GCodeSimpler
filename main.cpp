#include "gcodesimplerwidget.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator translator;
    translator.load("GCodeSimpler_zh_CN.qm");
    a.installTranslator(&translator);

	QString titleMessage = QObject::tr("\nGCodeSimpler\n(C)2014 Shaanxi Hengtong\n");
	GCodeSimpler::consoleWrite(titleMessage);
	// if some arguments given
    if (argc > 1)
    {
        if (argc >= 3)
        {
            QString option(argv[1]);
            if (option == "-c" || option == "--convert")
            {
                QStringList filelist;
                for (int i = 2; i < argc; ++i)
                    filelist.append(QString(argv[i]));
                GCodeSimpler simpler;
                simpler.processGCode(filelist);
                return 0;
            }
        }

        //TODO: any better solution to write string to console?
		QString helpMessage = QObject::tr("=======================\n\nUsage: GcodeSimpler [OPTION] [FILE]\nOptions:\n--help\t\t\t-h\t\tDisplay this help\n--convert <filepath>\t-c <filepath>\tProcess GCode file\n");
        GCodeSimpler::consoleWrite(helpMessage);
        return 1;
    }

    // if no extra arguments given, then show the GUI.
    GCodeSimplerWidget w;
    w.show();

    return a.exec();
}
