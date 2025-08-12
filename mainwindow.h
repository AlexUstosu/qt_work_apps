#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QDesktopServices>
#include <QMap>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QQueue>
#include <port.h>

#include <settingsdialog.h>
#include <command.h>
#include <tagshighlighter.h>

namespace Ui {
class MainWindow;
}

enum tag{INFO, WARNING, DATA};      //тэги для подсветки логов

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QAction* settingsAction;            //вызов окна настроек
    QAction* logAction;                 //журнал
    QAction* faqAction;                 //открытие инструкции

    SettingsDialog settingsDialog;      //Окно настроект порта
    Settings settingsFromDialog;        //Структура настроек с окна

    PortThread* portThread;             //Класс инициирующий запуск потока для работы с портом

    QMap<QWidget*, bool> buttonsMap;    //Коллекция кнопок и их значение
    QQueue<QByteArray> queueOfCommand;  //Очередь команд

    Command2Class command2;             //команды
    Command1Class command1;

    QTimer* timer;                      //для отправки команды 1

    QFile* file;                        //файл логов
    TagsHighlighter* highlighter;       //подсветка для журнала

    //Функции
    void showAvailablePorts();          //вывод доступных портов
    void fillMap();                     //заполнения коллекции кнопок
    void changeRadioButtonValueInMap(QAbstractButton* sender); //изменение значения в коллекции для радиокнопок
    void buttonsToCommand();            //преобразование нажатия в команду
    void command1ToButtons();           //разворачивание сырых данных в подсветку индикаторов
    void logsWriteToFile(QString log);  //запись в файл журнала
    QString logsFormatMessage(tag tag, QString string); //добавление к записи тэга

private slots:
    void slotSettingsMenu();
    void slotShowLogsTextEdith(bool flag);
    void slotFaqMenu();
    void slotSaveSettingsFromDialog(int, int, int, int, int);
    void slotStartConnection();
    void slotDisconnection();
    void slotChangeEnableWidgetsFromPortActivity(bool flag);
    void slotTimerStart(bool flag);
    void slotSendCommandFromQueue();
    void slotButtonsClicked();
    void slotWriteLogs(const QString& message);  
    void slotSendStatusOnTimer();
    void slotReadFromPort(QByteArray ba);

signals:
    void signalSaveSettingsFromDialog(Settings settings);
    void signalStartConnection();
    void signalDataReadyToPort(const QByteArray& ba);
    void signalLogFromWindow(QString string);
};

#endif // MAINWINDOW_H
