#ifndef GCODESIMPLER_H
#define GCODESIMPLER_H

#include <QObject>

class GCodeSimpler : public QObject
{
    Q_OBJECT
public:
    explicit GCodeSimpler(QObject *parent = 0);
    const QString simplify(const QString & reference);

signals:
    void processing();
    void finished(bool status, QString filepath);


public slots:
    void processGCode(QString filepath);
    void clearPosition();

private:
    //QString gCodeFilePath;
    //QString xj3dpFilePath;
    double x,y,z,e,f;
    double last_z;
    int data_count;
    int layer_count;
    //int total_layers;

};

#endif // GCODESIMPLER_H
