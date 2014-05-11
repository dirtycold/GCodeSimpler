#include "gcodesimplerwidget.h"

#include <QLayout>
#include <QMimeData>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QIcon>
#include <QStyle>
#include <QApplication>

static const quint16 notifyDelay = 5000;

GCodeSimplerWidget::GCodeSimplerWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addWidget(&statusLabel);
    hlayout->addWidget(&infoLabel);
    hlayout->addWidget(&aboutButton);
    vlayout->addLayout(hlayout);
    setLayout(vlayout);

    resize(360,360);
    infoLabel.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    aboutButton.setFixedSize(28,28);
    aboutButton.setFlat(true);
    aboutButton.setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation)));

    QString productName = tr("GcodeSimpler");
    setWindowTitle(productName);

    gCodeSimpler.moveToThread(&worker);
    worker.start();

    connect(this,SIGNAL(fileAccepted(QString)),&gCodeSimpler,SLOT(processGCode(QString)));
    connect(&gCodeSimpler,SIGNAL(processing()),this,SLOT(busy()));
    connect(&gCodeSimpler,SIGNAL(finished(bool,QString)),this,SLOT(finished(bool,QString)));
    connect(&aboutButton,SIGNAL(clicked()),this,SLOT(showAbout()));

    ready();
}

GCodeSimplerWidget::~GCodeSimplerWidget()
{
    worker.quit();
    worker.wait();
}

void GCodeSimplerWidget::ready()
{
    infoLabel.setText(tr("Ready."));
    statusLabel.setText(QString("<h2 align=center>%1</h2>").arg(tr("Drop GCode file here.")));
    setAcceptDrops(true);
}

void GCodeSimplerWidget::busy()
{
    setAcceptDrops(false);
    statusLabel.setText(QString("<h2 align=center>%1</h2>").arg(tr("Processing GCode...")));
}

void GCodeSimplerWidget::finished(bool status, QString message)
{
    // if true message is the output file path.
    // else it would be error message
    //process
    if (status)
    {
        //notify
        infoLabel.setText(tr("Destination: %1").arg(message));
        statusLabel.setText(QString("<h2 align=center>%1</h2>\n<p align=center>%2</p>").arg(tr("GCode processing complete.")).arg(tr("The output was in the same folder")));
        //ready
        QTimer::singleShot(notifyDelay * 2,this,SLOT(ready()));
    }
    else
    {
        error(message);
    }
}

void GCodeSimplerWidget::error(QString message)
{
    infoLabel.setText(message);
    statusLabel.setText(QString("<h2 align=center>%1</h2>").arg(tr("Something wrong happens.")));
    QTimer::singleShot(notifyDelay,this,SLOT(ready()));
}

void GCodeSimplerWidget::dragEnterEvent(QDragEnterEvent *e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if (!urls.isEmpty())
    {
        if (urls.count() > 1)
            infoLabel.setText(tr("Multiple files is not supported."));
        else
            infoLabel.setText(tr("Filepath: %1.").arg(urls.front().toLocalFile()));
    }
    e->acceptProposedAction();
}

void GCodeSimplerWidget::dragMoveEvent(QDragMoveEvent *e)
{
    e->acceptProposedAction();
}

void GCodeSimplerWidget::dragLeaveEvent(QDragLeaveEvent *e)
{
    ready();
    e->accept();
}

void GCodeSimplerWidget::dropEvent(QDropEvent *e)
{
    const QMimeData *mimeData = e->mimeData();
    if (mimeData->hasUrls())
    {
        QString path = mimeData->urls().front().toLocalFile();
        QFile file(path);
        QFileInfo fileInfo(file);
        QString suffix = fileInfo.completeSuffix();
        if (suffix.toLower() == "gcode")
        {
            //QMessageBox::information(this,"Accepted File",fileInfo.absoluteFilePath());
            //infoLabel.setText(QString("Processing %1").arg(fileInfo.absoluteFilePath()));
            emit fileAccepted(fileInfo.absoluteFilePath());
        }
        else
        {
            error(tr("Not a GCode file."));
        }
    }
    else
    {
        error(tr("File not accessable."));
    }
}

void GCodeSimplerWidget::showAbout()
{
    QMessageBox::information(this,QString("%1 %2").arg(tr("About")).arg(windowTitle()),QString("<h1 align=center>%1</h1>\n<p align=center>%4</p>\n<a align=center href=%3 align=center>%2</a>").arg(windowTitle()).arg(tr("Shaanxi Hengtong (C)2014")).arg("http://www.china-rpm.com/").arg("20140511"));
}
