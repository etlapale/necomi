#include <cppunit/ui/text/TestRunner.h>
#include "arraytestcase.h"


int
main()
{
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(ArrayTestCase::suite());
  runner.run();
  return 0;
}
