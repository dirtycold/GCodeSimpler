#include "gcodesimpler.h"
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QChar>
#include <QtDebug>

static const QString outSuffix = "xj3dp1";
static const QString movementIndentifier= "G1";
static const quint8 prec = 3;
static const double zRange = 120;

GCodeSimpler::GCodeSimpler(QObject *parent) :
    QObject(parent)
{
}

const QString GCodeSimpler::simplify(const QString &reference)
{
    QString identifier;
    int code = -1;
    QString line;
    QString data;
    QStringList list;
    list.clear();
    list = reference.split(" ", QString::SkipEmptyParts);
    //qDebug() << line;
    identifier = list.front();
    //qDebug() << identifier;
    if (identifier.startsWith("G"))
    {
        data = identifier.remove(0,1);
        code = data.toInt();
        //qDebug() << code;
        if (code==1 || code == 0)
        {
            //treat as a G-line
            //qDebug() << reference;
            for (int i = 1; i < list.count(); ++i)
            {
                //do extraction
                data = list.at(i);
                char c = data.at(0).toLower().toAscii();
                switch (c)
                {
                case 'x':

                    x=data.remove(0,1).toDouble();
                    break;
                case 'y':
                    y=data.remove(0,1).toDouble();
                    break;
                case 'z':
                    z=data.remove(0,1).toDouble();
                    break;
                case 'e':
                    e=data.remove(0,1).toDouble();
                    break;
                case 'f':
                    f=data.remove(0,1).toDouble()/60;
                    break;
                }
                if (z<zRange)
                    line = QString("%1 %2 %3 %4 %5").arg(QString::number(x,'f',prec)).arg(QString::number(y,'f',prec)).arg(QString::number(z,'f',prec)).arg(QString::number(f,'f',prec)).arg(QString::number(e,'f',prec));
                //outStream << outLine << endl;
            }
        }
    }
 
    return line;
}

void GCodeSimpler::processGCode(QString filepath)
{
    QString gCodeFilePath = filepath;
    QFile inFile(gCodeFilePath);
    if (!inFile.open(QIODevice::Text | QIODevice::ReadOnly))
        return finished(false,tr("Source file not accessible."));
    QFileInfo inFileInfo(inFile);
    QDir dir = inFileInfo.absoluteDir();
    QFileInfo inDirInfo(dir.path());
    if (!inDirInfo.isWritable())
        return finished(false,tr("Target folder not writable."));
    QDir::setCurrent(dir.path());
    QString xj3dpFilePath = QString("%1.%2").arg(inFileInfo.baseName()).arg(outSuffix);
    QFile outFile(xj3dpFilePath);
    QFileInfo outFileInfo(outFile);
//    if (outFile.exists())
//        return finished(false,"Target file exists.");
    if (!outFile.open(QIODevice::Text | QIODevice::WriteOnly))
        return finished(false,tr("Target file not writable."));

    //everything seems ready.
    emit processing();

    QTextStream inStream(&inFile);
    QTextStream outStream(&outFile);

    QString inLine;
    //QString outLine;
    QStringList outList;
    QStringList layerList;

    QString line;
    QString data;

    layer_count = 0;
    data_count = 0;
    bool first = true;
    clearPosition();

    while (!inStream.atEnd())
    {
        inLine = inStream.readLine();
        if (inLine.contains("(") || inLine.contains(";") ||inLine.isEmpty())
            continue;
        line = simplify(inLine);
        if (!line.isEmpty())
        {
            data = line;
            //actural line
            if ((z!=last_z) && (!first))
            {
                //layer change
                    //layer++

                line = QString("%1").arg(QString::number(data_count));
                layerList.prepend(line);
                line = QString("layer %1 seg 1").arg(QString::number(layer_count));
                layerList.prepend(line);
                outList<<layerList;
                layerList.clear();

                data_count = 0;
                layer_count++;
                //total_layers++;
                //layer_count=0;
                //last_z=z;
            }
            data_count++;
            layerList.append(data);
            last_z = z;
            first = false;
        }
             //outStream << line << endl;
    }
    //process last layer
    line = QString("%1").arg(QString::number(data_count));
    layerList.prepend(line);
    line = QString("layer %1 seg 1").arg(QString::number(layer_count));
    layerList.prepend(line);
    outList << layerList;
    layerList.clear();
    // no layer_count++ at the very end so we do it right away
    line =  QString("layers=%1").arg(QString::number(layer_count+1));
    outList.prepend(line);
    //outStream << outList;
    //foreach is slow
    QStringList::const_iterator it;
    for (it = outList.begin();it != outList.end();++it)
    {
        outStream << *it;
    }

    inFile.close();
    outFile.close();
    emit finished(true,outFileInfo.absoluteFilePath());
}

void GCodeSimpler::clearPosition()
{
    x=y=z=e=f=0;
}
