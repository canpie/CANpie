

#include <QApplication>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

#include "qcan_server_dialog.hpp"



int main(int argc, char *argv[])
{

   Q_INIT_RESOURCE(server);

   //----------------------------------------------------------------
   // create application
   //
   QApplication clAppT(argc, argv);

   
   //----------------------------------------------------------------
   // load translation
   //
   QTranslator clQtTranslatorT;

   if (clQtTranslatorT.load(QLocale(), 
       QLatin1String("server"), 
       QLatin1String("_"), QLatin1String(":/translations")))
   {
      clAppT.installTranslator(&clQtTranslatorT);
   }
   
   QApplication::setQuitOnLastWindowClosed(false);

   QCanServerDialog  clCanServerDlgT;
   clCanServerDlgT.hide();
   return clAppT.exec();
}

