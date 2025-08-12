#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //---------------------------------------------------------МЕНЮ----------------------------------------------------------------
    settingsAction = new QAction("Настройки", this);
    settingsAction->setShortcut(QKeySequence("Alt+S"));
    ui->menu->addAction(settingsAction);

    logAction = new QAction("Журнал", this);
    logAction->setCheckable(true);
    logAction->setShortcut(QKeySequence("Alt+J"));
    ui->menu->addAction(logAction);

    ui->menu->addSeparator();

    faqAction = new QAction("Справка", this);
    faqAction->setShortcut(QKeySequence("Alt+F"));
    ui->menu->addAction(faqAction);

    connect(settingsAction,              &QAction::triggered,                       this,       &MainWindow::slotSettingsMenu);
    connect(logAction,                   &QAction::triggered,                       this,       &MainWindow::slotShowLogsTextEdith);
    connect(faqAction,                   &QAction::triggered,                       this,       &MainWindow::slotFaqMenu);
    //------------------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------------ПОРТ-----------------------------------------------------------------
    connect(ui->portConnectPushButton,   &QPushButton::clicked,                     this,       &MainWindow::slotStartConnection);
    connect(&settingsDialog,             &SettingsDialog::signalSaveSettings,       this,       &MainWindow::slotSaveSettingsFromDialog);
    connect(ui->portDisconnectPushButton,&QPushButton::clicked,                     this,             &MainWindow::slotDisconnection);

    slotSaveSettingsFromDialog(19200, 8,1,0,0); //настройки по умолчанию
    //------------------------------------------------------------------------------------------------------------------------------
    //-------------------------------------------------------КНОПКИ-----------------------------------------------------------------
    connect(ui->e1RadioButton,    &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->e2RadioButton,    &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->e3RadioButton,    &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->aPushButton,      &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->cPushButton,      &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->dPushButton,          &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->PushButton_1,         &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->PushButton_2,         &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->PushButton_3,         &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->zPushButton,       &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->oPushButton,            &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->yPushButton,      &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    connect(ui->rPushButton,         &QPushButton::clicked,                     this,       &MainWindow::slotButtonsClicked);
    //------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------ЖУРНАЛ----------------------------------------------------------------
    //ОЧИСТКА ФАЙЛА
    file = new QFile("log.txt");
    if (file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(file);
        out << "";
        file->close();
    }
    highlighter = new TagsHighlighter(ui->logTextEdit->document());     //подсветка журнала
    logAction->setChecked(true);
    connect(this,                        &MainWindow::signalLogFromWindow,          this,       &MainWindow::slotWriteLogs);
    //------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------ТАЙМЕР----------------------------------------------------------------
    timer = new QTimer(this); 
    connect(timer,                       &QTimer::timeout,                          this,       &MainWindow::slotSendStatusOnTimer);
    connect(timer,                       &QTimer::timeout,                          this,       &MainWindow::slotSendCommandFromQueue);
    //------------------------------------------------------------------------------------------------------------------------------
    //--------------------------------------------------------КОМАНДЫ---------------------------------------------------------------
    connect(&command2,       &Command::signalLogFromCommand,            this,       &MainWindow::slotWriteLogs);
    connect(&command1,              &Command::signalLogFromCommand,            this,       &MainWindow::slotWriteLogs);

    fillMap();
    //------------------------------------------------------------------------------------------------------------------------------
    slotChangeEnableWidgetsFromPortActivity(false);     //установка активности виджетов в начальное положение
    showAvailablePorts();
}
MainWindow::~MainWindow()
{
    delete ui;
}
//---------------------------------------------------------МЕНЮ----------------------------------------------------------------
/*!
 * \brief Открытые окна настроек
 */
void MainWindow::slotSettingsMenu()
{
    emit signalLogFromWindow(tr("Открытие окна настроек..."));
    settingsDialog.exec();   
}
/*!
 * \brief Открытие журнала событий
 * \param flag параметр из сигнала нажатия кнопки
 */
void MainWindow::slotShowLogsTextEdith(bool flag)
{
    if(flag)
        ui->logTextEdit->show();
    else
        ui->logTextEdit->hide();
}
/*!
 * \brief Вызов справки из главного окна меню
 */
void MainWindow::slotFaqMenu()
{
    emit signalLogFromWindow(tr("Открытие окна справки..."));
    QDesktopServices::openUrl(QUrl("help.pdf"));
}
//------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------ПОРТ-----------------------------------------------------------------
/*!
 * \brief Функция для обнаружения доступных портов
 */
void MainWindow::showAvailablePorts()
{
    ui->portsAvailableComboBox->clear();
    foreach (const QSerialPortInfo &port, QSerialPortInfo::availablePorts()) {
        ui->portsAvailableComboBox->addItem(port.portName());
    }
}
/*!
 * \brief Слот для сохранения значений с окна настроек
 * \param baudRate  скорость передачи данных
 * \param dataBits  бит данных
 * \param stopBits  стоповый бит
 * \param parity    бит четности
 * \param flowControl   управление потоком
 */
void MainWindow::slotSaveSettingsFromDialog(int baudRate,
                                            int dataBits, int stopBits,
                                            int parity, int flowControl)
{
    settingsFromDialog.baudRate = static_cast<QSerialPort::BaudRate>(baudRate);
    settingsFromDialog.dataBits = static_cast<QSerialPort::DataBits>(dataBits);
    settingsFromDialog.stopBits = static_cast<QSerialPort::StopBits>(stopBits);
    settingsFromDialog.parity = static_cast<QSerialPort::Parity>(parity);
    settingsFromDialog.flowControl = static_cast<QSerialPort::FlowControl>(flowControl);
}

void MainWindow::slotStartConnection()
{
    //создания объекта только по нажатию кнопки подключить
    settingsFromDialog.name = ui->portsAvailableComboBox->currentText();
    portThread = new PortThread(settingsFromDialog,this);

    connect(this,                        &MainWindow::signalStartConnection,        portThread,       &PortThread::slotRun);
    connect(portThread,                  &PortThread::signalPortIsReady,            this,             &MainWindow::slotChangeEnableWidgetsFromPortActivity);
    connect(ui->portDisconnectPushButton,&QPushButton::clicked,                     portThread,       &PortThread::deleteLater);
    connect(portThread,                  &PortThread::signalPortIsReady,            this,             &MainWindow::slotTimerStart);
    connect(this,                        &MainWindow::signalDataReadyToPort,        portThread,       &PortThread::signalDataReadyToPort);
    connect(portThread,                  &PortThread::signalReadFromPort,           this,             &MainWindow::slotReadFromPort);
    connect(portThread,                  &PortThread::signalLogFromPort,            this,             &MainWindow::slotWriteLogs);

    emit signalStartConnection();
}
/*!
 * \brief Слот отключения порта
 */
void MainWindow::slotDisconnection()
{
    portThread->signalPortIsReady(false);
    slotWriteLogs(settingsFromDialog.name + tr(" порт закрыт."));
}
/*!
 * \brief Слот смены активности виджетов по результатам подключения к классу port
 * \param flag состояние порта
 */
void MainWindow::slotChangeEnableWidgetsFromPortActivity(bool flag)
{
    ui->portConnectPushButton->setEnabled(!flag);
    ui->portDisconnectPushButton->setEnabled(flag);

    settingsAction->setEnabled(!flag);
    ui->portsAvailableComboBox->setEnabled(!flag);

    if(flag)
        ui->connectionPushButton->setStyleSheet("background-color:#97CC94;color:#000000;");
    else
    {
        //индикаторы
        ui->connectionPushButton->setStyleSheet("background-color:#95124F;color:#FFFFFF;");
        ui->coPushButton->setStyleSheet("background-color:rgb(150, 180, 204);");
        ui->kPushButton->setStyleSheet("background-color:rgb(150, 180, 204);");
        ui->vPushButton->setStyleSheet("background-color:rgb(150, 180, 204);");
        ui->bPushButton->setStyleSheet("background-color:rgb(150, 180, 204);");

        //значения
        ui->electricity1Label->setText(QString::number(0));
        ui->electricity2Label->setText(QString::number(0));
        ui->temperature1Label->setText(QString::number(0) + QChar(0x2103));
        ui->temperature2Label->setText(QString::number(0) + QChar(0x2103));

        //активность кнопок
        ui->e1RadioButton->setChecked(false);
        ui->e2RadioButton->setChecked(false);
        ui->e3RadioButton->setChecked(false);
        ui->aPushButton->setChecked(false);
        ui->cPushButton->setChecked(false);
        ui->dPushButton->setChecked(false);
        ui->PushButton_1->setChecked(false);
        ui->PushButton_2->setChecked(false);
        ui->PushButton_3->setChecked(false);
        ui->rPushButton->setChecked(false);
        ui->yPushButton->setChecked(false);
        ui->oPushButton->setChecked(false);
        ui->zPushButton->setChecked(false);

        //журнал
        ui->logTextEdit->clear();
    }
    //кнопки соединения
    ui->GroupBox_1->setEnabled(flag);
    ui->GroupBox_2->setEnabled(flag);
    ui->GroupBox_4->setEnabled(flag);
    ui->GroupBox_5->setEnabled(flag);
}
/*!
 * \brief Слот запуска таймера
 * \param flag состояние порта
 */
void MainWindow::slotTimerStart(bool flag)
{
    if(flag)
        timer->start(1000);         //таймер для формирования команды
    else
        timer->stop();
}
/*!
 * \brief Слот отсылки команды из очереди
 */
void MainWindow::slotSendCommandFromQueue()
{
    if(!queueOfCommand.isEmpty())
        emit signalDataReadyToPort(queueOfCommand.dequeue());
}
//------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------КОМАНДЫ--------------------------------------------------------------
/*!
 * \brief Заполнение коллекции кнопками
 */
void MainWindow::fillMap()
{
    buttonsMap.insert(ui->e1RadioButton, false);
    buttonsMap.insert(ui->e2RadioButton, false);
    buttonsMap.insert(ui->e3RadioButton, false);
    buttonsMap.insert(ui->aPushButton, false);
    buttonsMap.insert(ui->cPushButton, false);
    buttonsMap.insert(ui->dPushButton, false);
    buttonsMap.insert(ui->PushButton_1, false);
    buttonsMap.insert(ui->PushButton_2, false);
    buttonsMap.insert(ui->PushButton_3, false);
    buttonsMap.insert(ui->zPushButton, false);
    buttonsMap.insert(ui->oPushButton, false);
    buttonsMap.insert(ui->yPushButton, false);
    buttonsMap.insert(ui->rPushButton, false);
}
/*!
 * \brief Изменение флага в коллекции кнопок
 */
void MainWindow::slotButtonsClicked()
{
    QMap<QWidget*, bool>::iterator i = buttonsMap.find(qobject_cast<QWidget*>(this->sender()));

    while (i != buttonsMap.end() && i.key() == qobject_cast<QWidget*>(this->sender()))
    {
        buttonsMap.insert(i.key(),qobject_cast<QAbstractButton*>(this->sender())->isChecked());
        if(qobject_cast<QAbstractButton*>(this->sender()) == ui->e1RadioButton or
           qobject_cast<QAbstractButton*>(this->sender()) == ui->e2RadioButton or
           qobject_cast<QAbstractButton*>(this->sender()) == ui->e3RadioButton)
            changeRadioButtonValueInMap(qobject_cast<QAbstractButton*>(this->sender()));
        ++i;
    }
    buttonsToCommand();
}
/*!
 * \brief Функция для изменения значения для невыбранных радиокнопок в коллекции
 * \param sender выбранная радиокнопка
 */
void MainWindow::changeRadioButtonValueInMap(QAbstractButton *sender)
{
    if(sender == ui->e1RadioButton)
    {
        buttonsMap.insert(ui->e2RadioButton,false);
        buttonsMap.insert(ui->e3RadioButton,false);
    }
    else if(sender == ui->e2RadioButton)
    {
        buttonsMap.insert(ui->e1RadioButton,false);
        buttonsMap.insert(ui->e3RadioButton,false);
    }
    else
    {
        buttonsMap.insert(ui->e1RadioButton,false);
        buttonsMap.insert(ui->e2RadioButton,false);
    }
}
/*!
 * \brief Слот для отправки команды 1
 */
void MainWindow::slotSendStatusOnTimer()
{
    slotTimerStart(false);                              //остановка таймера
    command1.clear();                                   //очистка объекта команды
    QByteArray ba = command1.assembleCommand();         //сборка команды
    queueOfCommand.enqueue(ba);                         //добавляем команду в очередь
    slotTimerStart(true);
}
/*!
 * \brief Сборка и отправка команды 2
 */
void MainWindow::buttonsToCommand()
{
    //формирование команды
    command2.clear();

    command2.setParametr1(buttonsMap.value(ui->e1RadioButton),
                                                  buttonsMap.value(ui->e2RadioButton),
                                                  buttonsMap.value(ui->e3RadioButton));
    command2.setParametr2(buttonsMap.value(ui->aPushButton),
                                        buttonsMap.value(ui->cPushButton),
                                        buttonsMap.value(ui->dPushButton));
    command2.setParametr3(buttonsMap.value(ui->PushButton_1),
                                                buttonsMap.value(ui->PushButton_2),
                                                buttonsMap.value(ui->PushButton_3));
    command2.setParametr4(buttonsMap.value(ui->zPushButton),
                                         buttonsMap.value(ui->oPushButton),
                                         buttonsMap.value(ui->yPushButton));
    command2.setParametr5(buttonsMap.value(ui->rPushButton));

    QByteArray ba = command2.assembleCommand();

    queueOfCommand.enqueue(ba); //добавляем команду в очередь
}
/*!
 * \brief Слот чтения команды 2
 * \param ba сырые данные
 */
void MainWindow::slotReadFromPort(QByteArray ba)
{
    QDataStream stream(&ba, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);

    quint8 var;
    stream >> var;

    if(var == 17)      //команда 1
    {
        if(command1.isParam1Compare(var))
        {
            command1.disAssembleCommand(ba);
            command1ToButtons();
        }
        else
        {
            emit signalLogFromWindow(tr("Ошибка: код 1."));
            emit signalLogFromWindow(tr("Данные: ") + ba.toHex());
        }
    }
    else if(var == 18) //команда 2
    {
        Command2Class cmdr;
        cmdr.setCheckParam1();
        if(command2.isParam1Compare(var))
        {
            cmdr.disAssembleCommand(ba);
            command2.setCheckParam1();
            if(command2 == cmdr)
            {
                command2 = cmdr;
                emit signalLogFromWindow(tr("______Данные совпадают_______"));
            }
            else
                emit signalLogFromWindow(tr("Ошибка: данные не соответвуют."));
        }
        else
        {
            emit signalLogFromWindow(tr("Ошибка: код 2."));
            emit signalLogFromWindow(tr("Данные: ") + ba.toHex());
        }
    }
    else
    {
        emit signalLogFromWindow(tr("Ошибка: код 3!"));
    }
}
/*!
 * \brief Подсветка индикаторов в соответствии с полученными данными
 */
void MainWindow::command1ToButtons()
{
    SomeStruct status = command1.disAssembleSomeStruct();
    if(status._field1)
        ui->coPushButton->setStyleSheet("background-color:#97CC94;color:#000000;");
    else
        ui->coPushButton->setStyleSheet("background-color:#95124F;color:#FFFFFF;");

    if(status._field2)
        ui->bPushButton->setStyleSheet("background-color:#97CC94;color:#000000;");
    else
        ui->bPushButton->setStyleSheet("background-color:#95124F;color:#FFFFFF;");

    if(status._field3)
    {
        ui->kPushButton->setStyleSheet("background-color:#97CC94;color:#000000;");
        ui->vPushButton->setStyleSheet("background-color:#95124F;color:#FFFFFF;");
    }
    else
    {
        ui->kPushButton->setStyleSheet("background-color:#95124F;color:#FFFFFF;");
        ui->vPushButton->setStyleSheet("background-color:#97CC94;color:#000000;");
    }

    ui->electricity1Label->setText(QString::number(command1.getParametr1()));
    ui->electricity2Label->setText(QString::number(command1.getParametr2()));
    ui->temperature1Label->setText(QString::number(command1.getOtherParametr1()) + QChar(0x2103));
    ui->temperature2Label->setText(QString::number(command1.getOtherParametr2()) + QChar(0x2103));
}
//------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------ЛОГИРОВАНИЕ---------------------------------------------------------------
/*!
 * \brief Функция для вывода данных в журнал
 * \param log Строковая перемення для хранения данных
 */
void MainWindow::slotWriteLogs(const QString& message)
{
    //анализируем строку для классификации записи
    tag thisTag;
    if(message.section(' ',0,0) == tr("Данные:"))
        thisTag = DATA;
    else if(message.section(' ',0,0) == tr("Ошибка:"))
        thisTag = WARNING;
    else
        thisTag = INFO;

    QString log = logsFormatMessage(thisTag, message);
    logsWriteToFile(log);
    ui->logTextEdit->append(log);
}
/*!
 * \brief Функция записи всех логов в файл
 * \param log строка логов
 */
void MainWindow::logsWriteToFile(QString log)
{
    if(file->open(QIODevice::Append |QIODevice::Text))
    {
        QTextStream stream(file);
        stream << log << "\n";
        file->close();
    }
}
/*!
 * \brief Функция сборки лога
 * \param tag тип сообщения Информационное/ ошибочное / сырые данные
 * \param string само сообщение
 * \return сборную строку нужного формата: [время][тег]Информация
 */
QString MainWindow::logsFormatMessage(tag tag, QString string)
{
    QString result;
    result = "[" + QTime::currentTime().toString() + "]";

    if(tag == INFO)
        result += "[INFO]";
    else if(tag == WARNING)
        result += "[WARNING]";
    else
        result += "[DATA]";

    result += "  " + string;
    return result;
}
//------------------------------------------------------------------------------------------------------------------------------
