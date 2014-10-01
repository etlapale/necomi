#pragma once

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include <cuiloa/cuiloa.h>
using namespace cuiloa;
using namespace cuiloa::delayed;

class DelayedTestCase : public CppUnit::TestCase
{
public:
  void test_product()
  {
    Array<double,1> a3(4);
    a3.map([](auto& path, auto& val) {
	val = path[0];
      });
    Array<double,1> b3(4);
    b3.map([](auto& path, auto& val) {
	val = 2*path[0];
      });

    auto c3 = a3 * b3;
    CPPUNIT_ASSERT(c3(0) == 0 && c3(3) == 18);
  }
  
  void test_product_bounds()
  {
    Array<double,1> a3(3);
    Array<double,1> a5(5);

    bool exception_thrown = false;
    try {
      a3 * a5;
    } catch (std::length_error& e) {
      exception_thrown = true;
    }
    CPPUNIT_ASSERT(exception_thrown);

    Array<double,1> b3(3);
    a3*b3;
  }

  static CppUnit::Test* suite()
  {
    CppUnit::TestSuite* s = new CppUnit::TestSuite("Delayed arrays");
    s->addTest(new CppUnit::TestCaller<DelayedTestCase>
	       ("element wise multiplication",
		&DelayedTestCase::test_product));
    s->addTest(new CppUnit::TestCaller<DelayedTestCase>
	       ("boundaries checks on element wise multiplication",
		&DelayedTestCase::test_product_bounds));
    return s;
  }
};

// Local variables:
// mode: c++
// End:
