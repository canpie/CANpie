
#include <iostream>

using namespace std;

#include <QDebug>
#include <QTest>


#include "test_qcan_client.hpp"
#include "test_qcan_frame.hpp"
#include "test_qcan_stub.hpp"



int main(int argc, char *argv[])
{
   int32_t  slResultT;

   cout << "#===========================================================\n";
   cout << "# Run test cases for QCan classes                           \n";
   cout << "#                                                           \n";
   cout << "#===========================================================\n";
   cout << "\n";


   //----------------------------------------------------------------
   // test QCanFrame
   //
   TestQCanFrame  clTestQCanFrameT;
   slResultT = QTest::qExec(&clTestQCanFrameT, argc, &argv[0]);

   //----------------------------------------------------------------
   // test QCanStub
   //
   TestQCanStub  clTestQCanStubT;
   slResultT = QTest::qExec(&clTestQCanStubT) + slResultT;

   //----------------------------------------------------------------
   // test QCanClient
   //
   TestQCanClient  clTestQCanClientT;
   slResultT = QTest::qExec(&clTestQCanClientT) + slResultT;


   cout << "\n";
   cout << "#===========================================================\n";
   cout << "# Total result                                              \n";
   cout << "# " << slResultT << " test cases failed                     \n";
   cout << "#===========================================================\n";

}

