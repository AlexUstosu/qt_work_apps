#ifndef PORT_H
#define PORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>
#include <QTextCodec>
#include <QTimer>
#include <QDebug>

struct Settings
{
    QString name;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity parity;
    QSerialPort::FlowControl flowControl;
};

class Port : public QObject
{
    Q_OBJECT
public:
    explicit Port(Settings settings);
    ~Port();
    
    Settings portSettings;
    QSerialPort* serialPort;

public slots:
    void slotConnection();
    void slotExecuteTransaction(const QByteArray& data);

signals:
    void signalPortIsReady(bool flag);
    void signalReadFromPort(const QByteArray& data);
    void signalLogFromPort(const QString &log);

private slots:
    void slotHandleError(QSerialPort::SerialPortError error);
};
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
class PortThread : public QObject
{
    Q_OBJECT
public:
    explicit PortThread(Settings settings, QObject* parent);
    ~PortThread();

public slots:
    void slotRun();         //старт потока
signals:
    void signalPortIsReady(bool flag);  //порт готов
    void signalDataReadyToPort(const QByteArray& data);
    void signalReadFromPort(const QByteArray& data);
    void signalLogFromPort(const QString &log);

private:
    mutable Port *port;
    QThread thread;
    Settings settings;
};
#endif // PORT_H
