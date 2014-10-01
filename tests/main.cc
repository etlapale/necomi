#include <cppunit/ui/text/TestRunner.h>

#include "arraytestcase.h"
#include "delayedtestcase.h"
#include "randomtestcase.h"

#ifdef HAVE_HDF5
#include "hdf5testcase.h"
#endif

int main()
{
  CppUnit::TextUi::TestRunner runner;

  runner.addTest(ArrayTestCase::suite());
  runner.addTest(DelayedTestCase::suite());
  runner.addTest(RandomTestCase::suite());
#ifdef HAVE_HDF5
  runner.addTest(HDF5TestCase::suite());
#endif

  return !runner.run();
}
