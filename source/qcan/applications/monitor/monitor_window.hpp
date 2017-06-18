#ifndef MONITOR_WINDOW_HPP_
#define MONITOR_WINDOW_HPP_


#include <QMainWindow>
#include <QObject>

#include <QCanSocket>

#include "ui_MonitorWindow.h"

class MonitorWindow : public QMainWindow
{
    Q_OBJECT

public:
    MonitorWindow(QWidget *parent = 0);
    ~MonitorWindow();

public slots:

   void  onSocketConnect(bool);
   
   void  onClientConnect(void);
   void  onClientDisconnect(void);
   void  onClientReceive(uint32_t ulFramesReceivedV);
   void  onSocketError(QAbstractSocket::SocketError teSockErrorV);

   
private slots:
   void  onSocketSelect(QCanSocket * pclCanSocketV);
   
private:
   Ui::MonitorWindow    ui;
   QPointer<QCanSocket> pclCanSocketP;
    
};

#endif // MONITOR_WINDOW_HPP_
