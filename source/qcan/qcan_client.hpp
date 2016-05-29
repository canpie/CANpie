#ifndef QCAN_CLIENT_HPP_
#define QCAN_CLIENT_HPP_

#include <QObject>
#include <qcan_frame.hpp>



class QCanClient : public QObject
{
    Q_OBJECT

public:
    QCanClient(QObject *parent = 0);
    ~QCanClient();

public slots:
   int32_t  writeMessage(const QCanFrame & clCanFrameR);
private:
   
    
};

#endif // QCAN_CLIENT_HPP_
