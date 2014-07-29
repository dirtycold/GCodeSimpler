#ifndef GCODESIMPLERWIDGET_H
#define GCODESIMPLERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QPushButton>
#include <QThread>

#include "gcodesimpler.h"

class GCodeSimplerWidget : public QWidget
{
    Q_OBJECT

public:
    GCodeSimplerWidget(QWidget *parent = 0);
    ~GCodeSimplerWidget();

signals:
    void filesAccepted(const QStringList & filelist);

public slots:
    void ready();
    void busy(const QString & filepath);
    void finished(bool status, const QString & message);
    void error(QString message);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    void dropEvent(QDropEvent *e);

private:
    QLabel statusLabel;
    QLabel infoLabel;
    QPushButton aboutButton;

    QThread worker;
    GCodeSimpler gCodeSimpler;

private slots:
    void showAbout();
};

#endif // GCODESIMPLERWIDGET_H
