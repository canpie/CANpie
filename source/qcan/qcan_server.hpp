#ifndef QCAN_SERVER_HPP_
#define QCAN_SERVER_HPP_

#include <QObject>
#include <QUdpSocket>

#include "qcan_frame.hpp"



class QCanServer : public QObject
{
    Q_OBJECT

public:
    QCanServer(QObject *parent = 0);
    ~QCanServer();

public:

private:

   QUdpSocket *   pclRcvSocketP;
   QUdpSocket *   pclTrmSocketP;


   int32_t  read(QCanFrame & clCanFrameR);
   int32_t  write(const QCanFrame & clCanFrameR);
   
};

#endif // QCAN_SERVER_HPP_
