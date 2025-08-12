#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud115200));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud57600));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud38400));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud19200));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud9600));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud4800));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud2400));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::Baud1200));
    ui->baudRateComboBox->addItem(QString::number(QSerialPort::BaudRate::UnknownBaud));

    ui->dataBitsComboBox->addItem(QString::number(QSerialPort::DataBits::Data5));
    ui->dataBitsComboBox->addItem(QString::number(QSerialPort::DataBits::Data6));
    ui->dataBitsComboBox->addItem(QString::number(QSerialPort::DataBits::Data7));
    ui->dataBitsComboBox->addItem(QString::number(QSerialPort::DataBits::Data8));
    ui->dataBitsComboBox->addItem(QString::number(QSerialPort::DataBits::UnknownDataBits));

    ui->stopBitsComboBox->addItem(QString::number(QSerialPort::StopBits::OneStop));
    ui->stopBitsComboBox->addItem(QString::number(QSerialPort::StopBits::OneAndHalfStop));
    ui->stopBitsComboBox->addItem(QString::number(QSerialPort::StopBits::TwoStop));
    ui->stopBitsComboBox->addItem(QString::number(QSerialPort::StopBits::UnknownStopBits));

    ui->parityComboBox->addItem(QString::number(QSerialPort::Parity::NoParity));
    ui->parityComboBox->addItem(QString::number(QSerialPort::Parity::EvenParity));
    ui->parityComboBox->addItem(QString::number(QSerialPort::Parity::OddParity));
    ui->parityComboBox->addItem(QString::number(QSerialPort::Parity::SpaceParity));
    ui->parityComboBox->addItem(QString::number(QSerialPort::Parity::MarkParity));
    ui->parityComboBox->addItem(QString::number(QSerialPort::Parity::UnknownParity));

    ui->flowControlComboBox->addItem(QString::number(QSerialPort::FlowControl::NoFlowControl));
    ui->flowControlComboBox->addItem(QString::number(QSerialPort::FlowControl::HardwareControl));
    ui->flowControlComboBox->addItem(QString::number(QSerialPort::FlowControl::SoftwareControl));
    ui->flowControlComboBox->addItem(QString::number(QSerialPort::FlowControl::UnknownFlowControl));

    //установка значений по умолчанию
    ui->baudRateComboBox->setCurrentIndex(4);
    ui->dataBitsComboBox->setCurrentIndex(3);
    ui->stopBitsComboBox->setCurrentIndex(0);
    ui->parityComboBox->setCurrentIndex(0);
    ui->flowControlComboBox->setCurrentIndex(0);

    ui->buttonBox->buttons().last()->setText(tr("Отмена"));
    ui->buttonBox->buttons().first()->setText(tr("Сохранить"));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/*!
 * \brief Слот обработки нажатия кнопки
 * \param button Кнопка сохранить или отмена
 */
void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button == ui->buttonBox->button(QDialogButtonBox::Save))
    {
        emit signalSaveSettings(ui->baudRateComboBox->currentText().toInt(),
                                ui->dataBitsComboBox->currentText().toInt(),
                                ui->stopBitsComboBox->currentText().toInt(),
                                ui->parityComboBox->currentText().toInt(),
                                ui->flowControlComboBox->currentText().toInt());
    }
    else
        this->reject();
}
