#include "port.h"

Port::Port(Settings settings) : QObject()
{
    serialPort = nullptr;
    portSettings = settings;
}
Port::~Port()
{
    if(serialPort)
    {
        serialPort->close();
        delete serialPort;
    }
}
/*!
 * \brief Слот запуска порта в потоке
 */
void Port::slotConnection()
{
    serialPort = new QSerialPort();

    //установка начальных настроек
    serialPort->setPortName(portSettings.name);
    serialPort->setBaudRate(portSettings.baudRate);
    serialPort->setDataBits(portSettings.dataBits);
    serialPort->setStopBits(portSettings.stopBits);
    serialPort->setParity(portSettings.parity);
    serialPort->setFlowControl(portSettings.flowControl);

    connect(serialPort, &QSerialPort::errorOccurred,   this,       &Port::slotHandleError);

    serialPort->open(QIODevice::ReadWrite);
    if(serialPort->isOpen())
    {
        emit signalPortIsReady(true);
        emit signalLogFromPort(portSettings.name + " порт открыт.");
    }
    else
    {
        emit signalLogFromPort(tr("Ошибка: открытия порта!\n")+ serialPort->errorString());
    }
}
/*!
 * \brief Слот организации транзакции
 * \param data данные для записи в порт
 */
void Port::slotExecuteTransaction(const QByteArray& data)
{
    if(serialPort == nullptr || !serialPort->isOpen())
        emit signalLogFromPort(tr("Ошибка: Порт не открыт или объект не создан!"));


    //ОТПРАВКА
    //Для корректной установки паритета
    serialPort->setParity(portSettings.parity);
    serialPort->close();
    serialPort->open(QIODevice::ReadWrite);

    if(serialPort->write(data) != data.size())
        emit signalLogFromPort(tr("Ошибка: Отправлены не все данные!"));

    serialPort->waitForBytesWritten();

    //ЧТЕНИЕ
    QByteArray buffer = serialPort->readAll();

    if(!serialPort->waitForReadyRead(1000))  //время ожидания вышло и ничего нет
        emit signalLogFromPort(tr("Ошибка: Вышло время ожидания ответа!"));
    else
    {
        buffer.append(serialPort->readAll());
        if(!buffer.isEmpty())
        {
            emit signalReadFromPort(buffer);
            buffer.clear();
        }
    }
}
/*!
 * \brief Слот обработки ошибок
 * \param error ошибка com-порта
 */
void Port::slotHandleError(QSerialPort::SerialPortError error)
{
    if(serialPort->isOpen() && error == QSerialPort::ResourceError)
    {
        emit signalLogFromPort(tr("Ошибка: Проблема порта! Код ошибки: ") + serialPort->errorString());
    }
}
//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
PortThread::PortThread(Settings settings, QObject* parent) : QObject(parent)
{
    port = new Port(settings);
    port->moveToThread(&thread);

    connect(&thread,                      &QThread::started,                         port,       &Port::slotConnection);
    connect(port,                         &Port::signalPortIsReady,                  this,       &PortThread::signalPortIsReady);
    connect(&thread,                      &QThread::finished,                        port,       &Port::deleteLater);
    connect(this,                         &PortThread::signalDataReadyToPort,        port,       &Port::slotExecuteTransaction);
    connect(port,                         &Port::signalReadFromPort,                 this,       &PortThread::signalReadFromPort);
    connect(port,                         &Port::signalLogFromPort,                  this,       &PortThread::signalLogFromPort);
}
PortThread::~PortThread()
{
    thread.quit();
    thread.wait();
}
/*!
 * \brief Старт потока
 */
void PortThread::slotRun()
{
    if(!thread.isRunning())
        thread.start();
}
