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

  void test_memory_refs()
  {
    // Return an expression whose array dependencies are stack allocated
    auto fun = [](int x, int y) {
      Array<double,1> a(5);
      a.fill(x);
      Array<double,1> b(5);
      b.fill(y);
      return a * b;
    };

    // Try smashing the stack by successive calls
    auto d = fun(3, 4);
    auto e = fun(7, 8);
    CPPUNIT_ASSERT(d(0) == 12 && d(4) == 12
      && e(0) == 56 && e(4) == 56);
  }

  void test_copy_into_array()
  {
    Array<int,2> a(3,4);
    a.fill(8);

    Array<int,1> b(4);
    b.fill(7);

    a[1] = b * b;

    CPPUNIT_ASSERT(a(0,0) == 8 && a(2,2) == 8
		   && a(1,0) == 49 && a(1,1) == 49);
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
    s->addTest(new CppUnit::TestCaller<DelayedTestCase>
	       ("memory management of referenced arrays",
		&DelayedTestCase::test_memory_refs));
    s->addTest(new CppUnit::TestCaller<DelayedTestCase>
	       ("copy into an existing array",
		&DelayedTestCase::test_copy_into_array));
    return s;
  }
};

// Local variables:
// mode: c++
// End:
