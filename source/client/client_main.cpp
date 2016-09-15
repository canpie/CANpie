#include <QApplication>
#include "client_demo.hpp"




int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   ClientDemo w;
   
   w.show();
   a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
   return a.exec();
}
