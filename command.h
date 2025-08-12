#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QtGlobal>
#include <QByteArray>
#include <QBitArray>
#include <QDataStream>

/*!
 * \brief Базовый класс команд
 */
class Command : public QObject
{
    Q_OBJECT
public:
    explicit Command(QObject *parent = nullptr);

    virtual QByteArray assembleCommand() = 0;               //сборка команды
    virtual void disAssembleCommand(QByteArray& ba) = 0;     //разборка команды

    virtual void setParam1() = 0;                           //установка параметра 1
    virtual quint8 getParam1() = 0;                         //взятие параметра 1

    virtual void setParam2() = 0;                           //установка параметра 2
    virtual quint8 getParam2() = 0;                         //взятие параметра 2

    virtual void clear() = 0;
    //-----------------------------------------------------------------------

    //функции конвертации
    QBitArray converToBitArray(quint8 param);
    quint8 convertFromBitArray(QBitArray &ba);

    bool isParam1Compare(quint8 param1);                    //функция сравнения параметра 1
    quint8 calculateParam3(const QByteArray& ba);           //функция подсчета параметра 3
    bool isParam3Right(quint8 param3, const QByteArray& ba);//функция сравнения параметра 3
    void clearCommand();                                    //функция очистки команды
signals:   
    void signalCommandReady(const QByteArray& ba);          //Сигнал о готовности команды к отправке
    void signalLogFromCommand(const QString& message);      //Сигнал для логирования

protected:
    //структура сообщения
    quint8 _param1;                 //параметр 1
    quint8 _param2;                 //параметр 2
    QByteArray _collectPart;        //сборная структура
    quint8 _param3;                 //параметр 3

    QByteArray _ba;     //итоговые данные
};
struct SomeStruct
{
    bool _field1;
    bool _field2;
    bool _field3;
};
/*!
 * \brief Класс команды 1
 */
class Command1Class : public Command
{
public:
    QByteArray assembleCommand() override;
    void disAssembleCommand(QByteArray& ba) override;

    void setParam1() override;
    quint8 getParam1() override;

    void setParam2() override;
    quint8 getParam2() override;

    void clear() override;
    //-----------------------------------------------------------------------

    //функции доступа
    void setSomeStruct(bool p1, bool p2, bool p3);
    quint16 getSomeStruct();
    quint16 getParametr1();
    quint16 getParametr2();
    quint16 getOtherParametr1();
    quint16 getOtherParametr2();
    SomeStruct disAssembleSomeStruct();

private:

    quint16  _varForSomeStruct;
    quint16 _parametr1;
    quint16 _parametr2;
    quint16 _otherParametr1;
    quint16 _otherParametr2;

    SomeStruct _someStruct;
};
/*!
 * \brief Класс команды 2
 */
class Command2Class : public Command
{
public:
    QByteArray assembleCommand() override;
    void disAssembleCommand(QByteArray& ba) override;

    void setParam1() override;
    quint8 getParam1() override;

    void setParam2() override;
    quint8 getParam2() override;

    void clear() override;

//-----------------------------------------------------------------------

    bool operator == (const Command2Class& command) const;
    bool operator != (const Command2Class& command) const;
    const Command2Class& operator = (const Command2Class& command);

    //функции доступа
    void setCheckParam1();
    void setParametr1(bool p1, bool p2, bool p3);
    quint8 getParametr1();
    void setParametr2(bool p1, bool p2, bool p3);
    quint8 getParametr2();
    void setParametr3(bool p1, bool p2, bool p3);
    quint8 getParametr3();
    void setParametr4(bool p1, bool p2, bool p3);
    quint8 getParametr4();
    void setParametr5(bool pr);
    quint8 getParametr5();

private:
    quint8 _parametr1;
    quint8 _parametr2;
    quint8 _parametr3;
    quint8 _parametr4;
    quint8 _parametr5;
};
#endif // COMMAND_H
