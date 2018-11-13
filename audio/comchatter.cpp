#include "comchatter.h"
#include <QDebug>

ComChatter::ComChatter(QObject *parent) : QObject(parent){

}

ComChatter::~ComChatter(){
    port.waitForBytesWritten(1000);
    close();
    emit dead();
}

void ComChatter::connect(QString name){
    port.setPortName(name);
    port.setBaudRate(QSerialPort::Baud115200);
    port.setDataBits(QSerialPort::Data8);
    port.setParity(QSerialPort::NoParity);
    port.setStopBits(QSerialPort::OneStop);
    port.setFlowControl(QSerialPort::NoFlowControl);
    if (port.open(QIODevice::WriteOnly)) {
        if (port.isOpen()){
            setDefaults();
        }else{
            emit error(port.errorString());
        }
    }else{
        emit error(port.errorString());
    }
}

void ComChatter::close(){
    qDebug() << "call close";
    if(port.isOpen()){
        setDefaults();
        port.close();
    }
    emit closed();
}

void ComChatter::send(QByteArray data){
    if(!port.isOpen()){
        emit error("port closed");
    }
    port.write(data);
    port.waitForBytesWritten(10);
}

void ComChatter::setDefaults(){
    qDebug() << "set defaults";
    char w(0);
    char c(255);
    QByteArray def;
    def.append(w);
    def.append(3, c);
    port.write(def);
    port.waitForBytesWritten(100);
    qDebug() << "ok";
}
