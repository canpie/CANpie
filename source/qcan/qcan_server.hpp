#ifndef QCAN_SERVER_HPP_
#define QCAN_SERVER_HPP_


#include <QObject>

#include "qcan_network.hpp"

class QCanServer : public QObject
{
    Q_OBJECT

public:
    QCanServer( QObject * pclParentV = Q_NULLPTR,
                uint16_t  uwPortStartV = QCAN_TCP_DEFAULT_PORT,
                uint8_t   ubNetworkNumV = QCAN_NETWORK_MAX);

    ~QCanServer();



    QCanNetwork * network(uint8_t ubNetworkIdxV);

    uint8_t maximumNetwork(void) const;


private:

    QVector<QCanNetwork *> *  pclListNetsP;

};

#endif // QCAN_SERVER_HPP_
