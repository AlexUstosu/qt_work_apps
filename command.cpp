#include "command.h"
Command::Command(QObject *parent)
    :QObject(parent)
{
}
/*!
 * \brief Функция перевода quint8 в QBitArray
 * \param param quint8 число
 * \return массив бит
 */
QBitArray Command::converToBitArray(quint8 param)
{
    QBitArray result(1*8);
    const quint8* data = static_cast<const quint8*>(&param);
    for(int byte = 0; byte < 1; ++byte) // кол-во байт в типе = 1
    {
        for(int bit = 0; bit < 8; ++bit)  //кол-во бит
        {
            result.setBit(byte*8 + bit, data[byte] & (1<<bit));
        }
    }
    return result;
}
/*!
 * \brief Функция перевода QBitArray в quint8
 * \param ba массив бит
 * \return quint8 число
 */
quint8 Command::convertFromBitArray(QBitArray &ba)
{
    quint8 result = 0;
    for(int i = 0; i < 8 && i < ba.size(); ++i)
    {
        if(ba.testBit(i))
            result |= (1 << i);
    }
    return result;
}
/*!
 * \brief Функция проверки соответствия
 * \param param1 параметр 1
 * \return да / нет
 */
bool Command::isParam1Compare(quint8 param1)
{
    return (_param1 | 0x10) == param1 ? true : false;
}
/*!
 * \brief Функция подсчета параметра 3
 * \param ba данные
 * \return результат
 */
quint8 Command::calculateParam3(const QByteArray& ba)
{
    quint16 result = 0;
    for(auto byte : ba) {
        result += static_cast<quint8>(byte);
    }
    if(result > 0xFF)
        result &= 0xFF;
    return static_cast<quint8>(result);
}
/*!
 * \brief Функция проверки параметра 3
 * \param param3 парметр 3 для проверки соответствия
 * \param ba данные
 * \return да/нет
 */
bool Command::isParam3Right(quint8 param3, const QByteArray& ba)
{
    quint8 checkParam3 = calculateParam3(ba);
    return checkParam3 == param3 ? true : false;
}
/*!
 * \brief Очистка полей команды
 */
void Command::clearCommand()
{
    _param1 = 0x00;
    _param2 = 0x00;
    _collectPart.clear();
    _collectPart.resize(0);
    _param3 = 0x00;
    _ba.clear();
    _ba.resize(0);
}
/*!
 * \brief Сборка команды 1
 * \return поле данных команды
 */
QByteArray Command1Class::assembleCommand()
{
    emit signalLogFromCommand("Сборка команды 1...");

    setParam1();
    setParam2();

    _ba.append(_param1);
    _ba.append(_param2);
    _ba.append(_collectPart);

    _param3 = calculateParam3(_ba);

    _ba.append(_param3);

    emit signalLogFromCommand("Данные: " + _ba.toHex());

    return _ba;
}
/*!
 * \brief Сериализация команды 1
 * \param ba сырые данные команды
 */
void Command1Class::disAssembleCommand(QByteArray& ba)
{
    emit signalLogFromCommand("Парсинг команды 1...");

    _ba = ba;

    QDataStream stream(&ba, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision); 

    stream >> _param1;
    stream >> _param2;
    _collectPart.clear();
    _collectPart.resize(_param2);
    _collectPart = _ba.mid(2,ba.size() - 3);
    stream >> _varForSomeStruct;
    stream >> _parametr1;
    stream >> _parametr2;
    stream >> _otherParametr1;
    stream >> _otherParametr2;
    stream >> _param3;

    //проверка на получение полных данных
    if(!isParam3Right(_param3, ba.mid(0,ba.size() - 1)))
    {
        emit signalLogFromCommand("Ошибка: Несоответствие параметра 3");
        clear();
    }    
}
/*!
 * \brief Установка парметра 1
 */
void Command1Class::setParam1()
{
    _param1 = 0x01;
}
quint8 Command1Class::getParam1(){return _param1;}
/*!
 * \brief Установка парметра 2
 */
void Command1Class::setParam2()
{
    _param2 = 0x00;
}
quint8 Command1Class::getParam2(){return _param2;}
/*!
 * \brief Очистка объекта команды 1
 */
void Command1Class::clear()
{
    clearCommand();
    _varForSomeStruct = 0x00;
    _parametr1 = 0x00;
    _parametr2 = 0x00;
    _otherParametr1 = 0x00;
    _otherParametr2 = 0x00;
}
/*!
 * \brief Установка какой то структуры
 * \param p1 - параметр 1
 * \param p2 - параметр 2
 * \param p3 - параметр 3
 */
void Command1Class::setSomeStruct(bool p1, bool p2, bool p3)
{
    QBitArray bits(8);
    bits.fill(0);

    _someStruct._field1 = p1;
    _someStruct._field2 = p2;
    _someStruct._field3 = p3;

    bits[0] = p1;
    bits[1] = p2;
    bits[2] = p3;

    _varForSomeStruct = convertFromBitArray(bits);
}
//функции доступа к приватным полям класса
quint16  Command1Class::getSomeStruct()
{
    return _varForSomeStruct;
}
/*!
 * \brief Функция для более удобной работы с составным полем для кнопок
 */
SomeStruct Command1Class::disAssembleSomeStruct()
{
    QBitArray bits(8);
    bits = converToBitArray(_varForSomeStruct);

    _someStruct._field1 = bits[0];
    _someStruct._field2 = bits[1];
    _someStruct._field3 = bits[2];
    return _someStruct;
}
quint16 Command1Class::getParametr1(){return _parametr1;}
quint16 Command1Class::getParametr2(){return _parametr2;}
quint16 Command1Class::getOtherParametr1(){return _otherParametr1;}
quint16 Command1Class::getOtherParametr2(){return _otherParametr2;}
/*!
 * \brief Сборка команды 2
 * \return поле данных команды
 */
QByteArray Command2Class::assembleCommand()
{
    emit signalLogFromCommand("Сборка команды 2...");

    setParam1();
    setParam2();

    //добавление параметров
    _collectPart.append(_parametr1);
    _collectPart.append(_parametr2);
    _collectPart.append(_parametr3);
    _collectPart.append(_parametr4);
    _collectPart.append(_parametr5);

    _ba.append(_param1);
    _ba.append(_param2);
    _ba.append(_collectPart);

    _param3 = calculateParam3(_ba);

    _ba.append(_param3);

    emit signalLogFromCommand("Данные: " + _ba.toHex());

    return _ba;
}
/*!
 * \brief Десериализация команды 2
 * \param ba сырые данные команды
 */
void Command2Class::disAssembleCommand(QByteArray& ba)
{
    emit signalLogFromCommand("Парсинг команды 2...");
    _ba = ba;

    QDataStream stream(&ba, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    stream >> _param1;
    stream >> _param2;
    _collectPart.clear();
    _collectPart.resize(_param2);
    _collectPart = _ba.mid(2,ba.size() - 3);
    stream >> _parametr1;
    stream >> _parametr2;
    stream >> _parametr3;
    stream >> _parametr4;
    stream >> _parametr5;
    stream >> _param3;

    //проверка на получение полных данных
    if(!isParam3Right(_param3, ba.mid(0,ba.size() - 1)))
    {
        emit signalLogFromCommand("Ошибка: Несоответствие параметра 3");
        clear();
    }
}
/*!
 * \brief Установка параметра 1
 */
void Command2Class::setParam1()
{
    _param1 = 0x02;
}
/*!
 * \brief Установка CheckParam1
 */
void Command2Class::setCheckParam1()
{
    _param1 = 0x12;
}
quint8 Command2Class::getParam1(){return _param1;}
/*!
 * \brief Установка параметра 2
 */
void Command2Class::setParam2()
{
    _param2 = 0x05;
}
quint8 Command2Class::getParam2(){return _param2;}
/*!
 * \brief Очистка объекта команды 2
 */
void Command2Class::clear()
{
    clearCommand();
    _parametr1 = 0x00;
    _parametr2 = 0x00;
    _parametr3 = 0x00;
    _parametr4 = 0x00;
    _parametr5 = 0x00;
}
/*!
 * \brief Перегрузка operator ==
 * \param команда
 * \return true / false
 */
bool Command2Class::operator ==(const Command2Class& command) const
{
    if(_param1 == command._param1)
    {
        if(_parametr1 == command._parametr1)
        {
            if(_parametr2 == command._parametr2)
            {
                if(_parametr3 == command._parametr3)
                {
                    if(_parametr4 == command._parametr4)
                    {
                        if(_parametr5 == command._parametr5)
                            return true;
                    }
                }
            }
        }
    }
    return false;
}
/*!
 * \brief Перегрузка operator !=
 * \param команда
 * \return true / false
 */
bool Command2Class::operator !=(const Command2Class& command) const
{
    if(_param1 == command._param1)
    {
        if(_parametr1 == command._parametr1)
        {
            if(_parametr2 == command._parametr2)
            {
                if(_parametr3 == command._parametr3)
                {
                    if(_parametr4 == command._parametr4)
                    {
                        if(_parametr5 == command._parametr5)
                            return false;
                    }
                }
            }
        }
    }
    return true;
}
/*!
 * \brief Перегрузка operator =
 * \param команда
 * \return команда
 */
const Command2Class& Command2Class::operator =(const Command2Class& command)
{
    if(&command != this)
    {
        _param1 = command._param1;
        _parametr1 = command._parametr1;
        _parametr2 = command._parametr2;
        _parametr3 = command._parametr3;
        _parametr4 = command._parametr4;
        _parametr5 = command._parametr5;
    }
    return *this;
}
/*!
 * \brief Установка параметра 1
 * \param p1 - парметр1
 * \param p2 - парметр2
 * \param p3 - парметр3
 */
void Command2Class::setParametr1(bool p1, bool p2, bool p3)
{
    if(p1)
        _parametr1 = 1;
    else if(p2)
        _parametr1 = 2;
    else if(p3)
        _parametr1 = 3;
    else
        _parametr1 = 1;
}
quint8 Command2Class::getParametr1(){return _parametr1;}
/*!
 * \brief Установка параметра 2
 * \param p1 - парметр1
 * \param p2 - парметр2
 * \param p3 - парметр3
 */
void Command2Class::setParametr2(bool p1, bool p2, bool p3)
{
    QBitArray bits(8);
    bits.fill(0);

    bits[0] = p1;
    bits[1] = p2;
    bits[2] = p3;

    _parametr2 = convertFromBitArray(bits);
}
quint8 Command2Class::getParametr2(){return _parametr2;}
/*!
 * \brief Установка параметра 3
 * \param p1 - парметр1
 * \param p2 - парметр2
 * \param p3 - парметр3
 */
void Command2Class::setParametr3(bool p1, bool p2, bool p3)
{
    QBitArray bits(8);
    bits.fill(0);

    bits[0] = p1;
    bits[1] = p2;
    bits[2] = p3;

    _parametr3 = convertFromBitArray(bits);
}
quint8 Command2Class::getParametr3(){return _parametr3;}
/*!
 * \brief Установка параметра 4
 * \param p1 - парметр1
 * \param p2 - парметр2
 * \param p3 - парметр3
 */
void Command2Class::setParametr4(bool p1, bool p2, bool p3)
{
    QBitArray bits(8);
    bits.fill(0);

    bits[0] = p1;
    bits[1] = p2;
    bits[2] = p3;

    _parametr4 = convertFromBitArray(bits);
}
quint8 Command2Class::getParametr4(){return _parametr4;}
/*!
 * \brief Установка парметра 5
 * \param pr - парметр
 */
void Command2Class::setParametr5(bool pr)
{
    QBitArray bits(8);
    bits.fill(0);

    bits[0] = pr;

    _parametr5 = convertFromBitArray(bits);
}
quint8 Command2Class::getParametr5(){return _parametr5;}
