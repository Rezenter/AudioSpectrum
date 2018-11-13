#ifndef COMCHATTER_H
#define COMCHATTER_H

#include <QObject>
#include <QtSerialPort/QserialPort>

class ComChatter : public QObject{
    Q_OBJECT

public:
    ComChatter(QObject *parent = 0);
    ~ComChatter();
    void setDefaults();

private:
    QSerialPort port;

signals:
    void closed();
    void error(QString error);
    void dead();

public slots:
    void connect(QString name);
    void close();
    void send(QByteArray data);
};

#endif // COMCHATTER_H
