#include "gmessagepoller.h"
#include "gclib_errors.h"

#include <QDebug>

GMessagePoller::GMessagePoller(QObject *parent):
    QThread(parent)
{

}

GMessagePoller::~GMessagePoller()
{  
    qDebug() << "ending handler";
    stop();
    wait();
    qDebug() << "destroyed";
}

void GMessagePoller::connect_to_controller(std::string_view IPAddress)
{
    quit_ = false;

    std::string stringIn = IPAddress.data();
    stringIn += " --subscribe MG";

    // don't do anything if it can't connect
    if (GOpen(stringIn.c_str(), &g_) != G_NO_ERROR)
    {
        qDebug() << "Could not connect to controller for messages";
        return;
    }

    GCmd(g_, "TR0"); // Make sure trace is off
    GTimeout(g_, 0); // set timeout to 0 for non-blocking read

    start();
}

void GMessagePoller::stop()
{
    mutex_.lock();
    quit_ = true;
    mutex_.unlock();
}

void GMessagePoller::run()
{

    GReturn rc = 0;
    int b = 0; //iterator for buf
    int m = 0; //iterator for message

    qDebug() << "start message handler";

    char buf[G_SMALL_BUFFER]; //read buffer
    char messageBuf[G_SMALL_BUFFER];

    while (true)
    {
        mutex_.lock();
        if (quit_)
        {
            mutex_.unlock();
            break;
        }
        mutex_.unlock();

        //While still receiving messages
        while ((rc = GMessage(g_, buf, G_SMALL_BUFFER)) == G_NO_ERROR)
        {
            b = 0; //reset buffer index

            while (buf[b] != '\0') //While message characters are in the buffer
            {
                messageBuf[m] = buf[b]; //Copy chars from buffer to message

                //If the message ends in "\r\n" its ready to be terminated
                if (m > 0 && messageBuf[m] == '\n' && messageBuf[m - 1] == '\r')
                {
                    messageBuf[m - 1] = '\0'; //Null terminate the message (strip \r\n)

                    // handle the complete message here
                    emit message(QString(messageBuf));

                    m = 0;  //Reset message index
                }
                else
                {
                    m++; //Increment message index
                }

                b++; //Increment buf index
            }
        }

        QThread::msleep(sleepTime_ms_);

//        if ((rc = GMessage(g_, buf, sizeof(buf))) == G_GCLIB_NON_BLOCKING_READ_EMPTY)
//        {
//            QThread::msleep(sleepTime_ms_);
//            // could also use usleep
//            continue;
//        }

//        if (rc != G_NO_ERROR)
//        {
//            emit error();
//            qDebug() << "GMessage read error";
//        }
    }

    GClose(g_);
    g_ = 0;
}

#include "moc_gmessagepoller.cpp"
