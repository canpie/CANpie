

#include <QApplication>
#include <QStyleFactory>

#include <QDebug>

#include "monitor_window.hpp"



int main(int argc, char *argv[])
{

   QApplication clAppT(argc, argv, 1);

   clAppT.setOrganizationName("MicroControl");
   clAppT.setOrganizationDomain("microcontrol.net");
   clAppT.setApplicationName("CANpieMonitor");
   
   // QApplication::setStyle(QStyleFactory::create("Macintosh"));
   // QApplication::setQuitOnLastWindowClosed(false);

   MonitorWindow  clCanMonitorT;
   clCanMonitorT.show();
   return clAppT.exec();
}

