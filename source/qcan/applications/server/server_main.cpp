

#include <QApplication>
#include <QStyleFactory>

#include <QDebug>

#include "qcan_server_dialog.hpp"



int main(int argc, char *argv[])
{

   Q_INIT_RESOURCE(server);

   QApplication app(argc, argv);

   QApplication::setStyle(QStyleFactory::create("Macintosh"));
   QApplication::setQuitOnLastWindowClosed(false);

   QCanServerDialog  clCanServerDlgT;
   clCanServerDlgT.hide();
   return app.exec();
}

