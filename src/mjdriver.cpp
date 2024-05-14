#include "mjdriver.h"

#include <QSerialPort>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

namespace Added_Scientific
{

using json = nlohmann::json;

Controller::Controller(const QString &portName, QObject *parent) :
    AsyncSerialDevice(portName, parent)
{
    name = "MJ_Controller";
    // connect timer for handling timeout errors
    connect(serialPort, &QSerialPort::readyRead, this, &Controller::handle_ready_read);
    connect(timer, &QTimer::timeout, this, &Controller::handle_timeout);
    connect(serialPort, &QSerialPort::errorOccurred, this, &Controller::handle_serial_error);

    serialPort->setBaudRate(1'000'000); // 1 million baud rate.
}

Controller::~Controller()
{
    if (is_connected()) serialPort->close();
}

void Controller::handle_ready_read()
{
    // look at DataRecievedHandler in "MJ Driver Board/Software/DriverBoardDropwatcher/Form1.cs"
    while (serialPort->bytesAvailable())
    {
        auto inData = serialPort->readAll();
        readData.append(inData);
    }

    if (readData[0] == '{')
    {
        json j;
        try
        {
            j = json::parse(readData.toStdString());
            // handle the json here
            emit response( QString::fromStdString(j.dump(4)) ); // this probably doesn't need to be here and clogs the output window

        }
        catch (const nlohmann::json::parse_error &e)
        {
            QString errorMessage = "Failed to parse JSON: ";
            errorMessage += e.what();
            emit error(errorMessage);
        }

    }

    else
    {
        emit response(QString(readData));
    }

    readData.clear();
    write_next();
}

void Controller::handle_timeout()
{
    timer->stop();
    emit error(QString("Serial IO Timeout: No response from %1").arg(name));
    emit error(readData);
    disconnect_serial();
}

// writes to serial device, adding a LF character ('\n')
// the controller expects LF after every command
void Controller::write_line(const QByteArray &data)
{
    write(data + "\n");
}

void Controller::connect_board()
{
    if (is_connected()) // return if already connected
    {
        emit response(QString("Already connected to %1").arg(name));
        return;
    }

    clear_members();

    // attempt to open connection to serial port
    if (!serialPort->open(QIODevice::ReadWrite))
    {
        emit error(QString("Can't open %1 on %2, error code %3")
                   .arg(name, serialPort->portName(), QVariant::fromValue(serialPort->error()).toString()));
        return;
    }

    // connection is open
    emit response(QString("Connected to %1").arg(name));
    return;
}

void Controller::disconnect_serial()
{
    clear_members();
    if (is_connected())
    {
        emit response(QString("Disconnecting %1").arg(name));
        serialPort->close();
        timer->stop();
    }
    else emit response(QString("%1 is already disconnected").arg(name));
}

void Controller::power_on()
{
    write_line("O");
}

void Controller::power_off()
{
    write_line("F");
}

void Controller::report_status()
{
    write_line("b");
}

void Controller::set_printing_frequency(int frequency)
{
    if (frequency <= 0) return;
    QString command = QString("p %1").arg(frequency);
    write_line(command.toUtf8());
}

void Controller::set_head_voltage(HeadIndex idx, double voltage)
{
    // only allow voltage between 15-36 V
    if ((voltage < 15.0) || (voltage > 36.0)) return;
    QString command = "v ";
    command += QString::number(static_cast<int>(idx));
    command += " ";
    command += QString::number(voltage, 'f', 2); // floating with 2 decimals of precision
    write_line(command.toUtf8());
}

void Controller::mode_select(Mode mode)
{
    QString command = QString("M %1").arg(static_cast<int>(mode));
    write_line(command.toUtf8());
}

void Controller::soft_reset_board()
{
    write_line("r");
}

void Controller::report_current_position()
{
    write_line(">");
}

void Controller::report_head_temps()
{
    write_line("t");
}

void Controller::request_status_of_all_heads()
{
    write_line("B");
}

void Controller::clear_all_heads_of_data()
{
    write_line("C");
}

void Controller::clear_members()
{
    clear_command_queue();
}

void Controller::handle_serial_error(
        QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::ReadError)
    {
        emit error(QObject::tr("Read error on port %1, error: %2")
                            .arg(serialPort->portName(),
                                 serialPort->errorString()));
        disconnect_serial();
    }

    if (serialPortError == QSerialPort::OpenError)
    {
        emit error(QObject::tr("Error opening port %1, error: %2")
                            .arg(serialPort->portName(),
                                 serialPort->errorString()));
        disconnect_serial();
    }

    if (serialPortError == QSerialPort::WriteError)
    {
        emit error(QObject::tr("Error writing to port %1, error: %2")
                            .arg(serialPort->portName(),
                                 serialPort->errorString()));
        disconnect_serial();
    }
}



} // namespace Added_Scientific

#include "moc_mjdriver.cpp"