#ifndef CLIENT_DEMO_HPP_
#define CLIENT_DEMO_HPP_


#include <QDialog>
#include "ui_ClientDemo.h"
#include <QCanSocket>


class ClientDemo : public QDialog
{
    Q_OBJECT

public:
    ClientDemo(QWidget *parent = 0);
    ~ClientDemo();

public slots:

   void  onClientConnect(void);
   void  onClientDisconnect(void);
   void  onClientNmtStart(void);
   void  onClientSync(void);
   void  onClientReceive(uint32_t ulFramesReceivedV);


private:
    Ui::CpClientDlg  ui;
    QCanSocket *     pclCanSocketP;
    
};

#endif // CLIENT_DEMO_HPP_
