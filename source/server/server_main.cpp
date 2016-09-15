

#include <QApplication>
#include <QDebug>

#include "qcan_server_dialog.hpp"



int main(int argc, char *argv[])
{

   Q_INIT_RESOURCE(qcanserver);

   QApplication app(argc, argv);

   QApplication::setQuitOnLastWindowClosed(false);

   QCanServerDialog  clCanServerDlgT;
   clCanServerDlgT.hide();
   return app.exec();
}

