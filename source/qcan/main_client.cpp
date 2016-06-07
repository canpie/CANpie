
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
   
   for(uint8_t ubCntT = 0; ubCntT < 9; ubCntT++)
   {
      clFrame1T.setStdId(0xB01 + ubCntT);
      clFrame1T.setDlc(ubCntT);
      clFrame1T.setData(0, ubCntT);
      clFrame1T.setData(1, 0x11);
      clFrame1T.setData(2, 0x22);
      clFrame1T.setData(3, 0x33);
      clFrame1T.setData(4, 0x44);
      clFrame1T.setData(5, 0x55);
      clFrame1T.setData(6, 0x66);
      clFrame1T.setData(7, 0x77);
      
      clFrame1T.setMarker(0x22334);
      clFrame1T.setUser(0xAB1023);
      
      clMessageT = clFrame1T.toString();
      
      cout << "CAN1: " << clMessageT.toUtf8().constData();
      cout << "\n";
      
      QByteArray  clByteArrayT;
      clByteArrayT =    clFrame1T.toByteArray();
     
      //qDebug() << "Data in ByteArray:";
      //qDebug() << clByteArrayT;
      
      if(clFrame2T.fromByteArray(clByteArrayT) == false)
      {
         cout << "Copy issue ! \n";
      }
      else
      {
         clMessageT = clFrame2T.toString();
         
         cout << "CAN2: " << clMessageT.toUtf8().constData();
         cout << "\n";
         
      }
      
      
   }
   
      //return a.exec();
}

