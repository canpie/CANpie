
#include <iostream>

using namespace std;

#include <QCoreApplication>
#include <QDebug>
#include "qcan_client.hpp"

int main(int argc, char *argv[])
{
   QCoreApplication a(argc, argv);

   cout << "Hello world\n";

   QCanFrame   clFrame1T;
   QCanFrame   clFrame2T;
   QString     clMessageT;
   
   clFrame1T.setExtId(0x12AABB01);
   clFrame1T.setDlc(2);
   clFrame1T.setData(0, 0x01);
   clFrame1T.setData(1, 0x22);
   
   clMessageT = clFrame1T.toString();
   
   cout << "CAN: " << clMessageT.toUtf8().constData();
   cout << "\n";
   
      //return a.exec();
}

