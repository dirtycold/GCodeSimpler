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

static const quint16 notifyDelay = 2500;

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

    setFixedSize(400,360);
    infoLabel.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    aboutButton.setFixedSize(28,28);
    aboutButton.setFlat(true);
    aboutButton.setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation)));

    QString productName = tr("GcodeSimpler");
    setWindowTitle(productName);

    gCodeSimpler.moveToThread(&worker);
    worker.start();

    connect(this,SIGNAL(filesAccepted(QStringList)),&gCodeSimpler,SLOT(processGCode(QStringList)));
    connect(&gCodeSimpler,SIGNAL(processing(QString)),this,SLOT(busy(QString)));
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
    statusLabel.setText(QString("<h2 align=center>%1</h2>").arg(tr("Drag and drop GCode file(s) here.")));
    setAcceptDrops(true);
}

void GCodeSimplerWidget::busy(const QString &filepath)
{
    setAcceptDrops(false);
    statusLabel.setText(QString("<h2 align=center>%1</h2>").arg(tr("Processing GCode file(s)...")));
    infoLabel.setText(tr("Filepath: %1.").arg(filepath));
}

void GCodeSimplerWidget::finished(bool status, const QString &message)
{
    // if true message is the output file path.
    // else it would be error message
    //process
    if (status)
    {
        //notify
        infoLabel.setText(message);
        statusLabel.setText(QString("<h2 align=center>%1</h2>\n<p align=center>%2</p>").arg(tr("GCode file(s) processing complete.")).arg(tr("The output was in the same folder.")));
        //ready
        QTimer::singleShot(notifyDelay,this,SLOT(ready()));
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
        infoLabel.setText(tr("Now drop Gcode file(s) here."));
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
        QStringList filelist;
        QList<QUrl> urllist = mimeData->urls();
        foreach (QUrl url, urllist) {
            filelist.append(url.toLocalFile());
        }
        emit filesAccepted(filelist);
    }
    else
    {
        error(tr("File not accessable."));
    }
}

void GCodeSimplerWidget::showAbout()
{
    QString date = QString::fromLocal8Bit(__DATE__);
	QMessageBox::information(this,QString("%1 %2").arg(tr("About")).arg(windowTitle()),QString("<h1 align=center>%1</h1>\n<p align=center>%4 (%5)</p>\n<a align=center href=%3 align=center>%2</a>").arg(windowTitle()).arg(tr("2014(C) Shaanxi Hengtong ")).arg("http://www.china-rpm.com/").arg("v2.0").arg(date));
}
