
#include <iostream>

using namespace std;

#include <QCoreApplication>
#include <QDebug>
#include <QTest>


#include "test_qcan_timestamp.hpp"
#include "test_qcan_frame.hpp"
#include "test_qcan_socket.hpp"


int main(int argc, char *argv[])
{
   int32_t  slResultT;

   cout << "#===========================================================\n";
   cout << "# Run test cases for QCan classes                           \n";
   cout << "#                                                           \n";
   cout << "#===========================================================\n";
   cout << "\n";



   //----------------------------------------------------------------
   // test QCanTimestamp
   //
   TestQCanTimestamp  clTestQCanTimestampT;
   slResultT = QTest::qExec(&clTestQCanTimestampT, argc, &argv[0]);

   //----------------------------------------------------------------
   // test QCanFrame
   //
   TestQCanFrame  clTestQCanFrameT;
   slResultT = QTest::qExec(&clTestQCanFrameT, argc, &argv[0]);

   //----------------------------------------------------------------
   // test QCanStub
   //
   TestQCanSocket  clTestQCanSockT;
   slResultT = QTest::qExec(&clTestQCanSockT) + slResultT;

   cout << "\n";
   cout << "#===========================================================\n";
   cout << "# Total result                                              \n";
   cout << "# " << slResultT << " test cases failed                     \n";
   cout << "#===========================================================\n";

}

