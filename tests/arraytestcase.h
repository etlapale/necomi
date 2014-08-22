#ifndef __CUILOA_ARRAY_TEST_CASE
#define __CUILOA_ARRAY_TEST_CASE

#include <iostream>
using namespace std;

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include <cuiloa/cuiloa.h>
using namespace cuiloa;

class ArrayTestCase : public CppUnit::TestCase
{
public:
  void
  test_creation()
  {
    Array<double,0> a0;
    CPPUNIT_ASSERT(a0.size() == 1);

    Array<double,1> a1(127);
    CPPUNIT_ASSERT(a1.size() == 127);

    Array<double,2> a2(3,4);
    CPPUNIT_ASSERT(a2.size() == 3*4);

    Array<double,5> a5(8,1,7,2,9);
    CPPUNIT_ASSERT(a5.size() == 8*1*7*2*9);
  }

  void
  test_dimensions()
  {
    Array<double,0> a0;
    CPPUNIT_ASSERT(a0.dimensions().empty());

    Array<double,1> a1(127);
    CPPUNIT_ASSERT(a1.dimensions().size() == 1
        && a1.dimensions()[0] == 127);

    Array<double,2> a2(3,4);
    CPPUNIT_ASSERT(a2.dimensions().size() == 2
        && a2.dimensions()[0] == 3
        && a2.dimensions()[1] == 4);

    Array<double,5> a5(8,1,7,2,9);
    CPPUNIT_ASSERT(a5.dimensions().size() == 5
        && a5.dimensions()[1] == 1
        && a5.dimensions()[4] == 9);
  }

  void
  test_strides()
  {
    Array<double,0> a0;
    CPPUNIT_ASSERT(a0.strides().empty());

    Array<double,1> a1(127);
    CPPUNIT_ASSERT(a1.strides().size() == 1
        && a1.strides()[0] == 1);

    Array<double,2> a2(3,4);
    CPPUNIT_ASSERT(a2.strides().size() == 2
        && a2.strides()[0] == 4
        && a2.strides()[1] == 1);

    Array<double,5> a5(8,1,7,2,9);
    CPPUNIT_ASSERT(a5.strides().size() == 5
        && a5.strides()[1] == 1*7*2*9
        && a5.strides()[4] == 1);
  }

  void
  test_indices()
  {
    Array<double,0> a0;
    CPPUNIT_ASSERT(a0.index() == 0);

    Array<double,1> a1(127);
    CPPUNIT_ASSERT(a1.index(4) == 4
        && a1.index(86) == 86);

    Array<double,2> a2(3,4);
    CPPUNIT_ASSERT(a2.index(0,2) == 2
        && a2.index(1,3) == 7
        && a2.index(2,1) == 9);
  }

  void
  test_data()
  {
    Array<int,0> a0;
    a0() = 123;
    CPPUNIT_ASSERT(a0() == 123
        && a0.data()[0] == 123);

    Array<int,1> a1(127);
    a1(32) = 123;
    a1(69) = 456;
    CPPUNIT_ASSERT(a1(32) == 123
        && a1(69) == 456
        && a1.data()[32] == 123
        && a1.data()[69] == 456);

    Array<double,2> a2(3,4);
    for (unsigned int i = 0; i < a2.size(); i++)
      a2.data()[i] = i;
    CPPUNIT_ASSERT(a2(1,2) == 6
        && a2(2,3) == a2.index(2,3));
  }

  void test_slices()
  {
    Array<int,1> a1(127);
    for (unsigned int i = 0; i < a1.size(); i++)
      a1(i) = i;
    auto a1s = a1[45];
    CPPUNIT_ASSERT(a1s.dimensions().empty()
        && a1s() == 45);

    Array<int,2> a2(3,4);
    for (unsigned int i = 0; i < a2.size(); i++)
      a2.data()[i] = i;
    auto a2s = a2[2];
    CPPUNIT_ASSERT(a2s.dimensions().size() == 1
        && a2s.size() == 4
        && a2s.dimensions()[0] == 4);
    CPPUNIT_ASSERT(a2s.strides().size() == 1
        && a2s.strides()[0] == 1);
    CPPUNIT_ASSERT(a2s(2) == 10);
    CPPUNIT_ASSERT(a2s(3) == 11);
  }

  void test_maps()
  {
    auto incr([](auto& path, auto& val) {
        (void) path;
        return val + 1;
      });

    Array<int,0> a0;
    a0() = 123;
    a0.map(incr);
    CPPUNIT_ASSERT(a0() == 124);
    
    Array<int,1> a1(127);
    for (unsigned int i = 0; i < a1.size(); i++)
      a1(i) = i;
    a1.map(incr);
    CPPUNIT_ASSERT(a1(43) == 44
        && a1(120) == 121);

    Array<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        (void) val;
        return path[0]*12 + path[1]*4 + path[2];
      });
    CPPUNIT_ASSERT(a3(1,2,0) == 20);

    a1.fill(42);
    CPPUNIT_ASSERT(a1(34) == 42 && a1(56) == 42);
  }

  void test_copies()
  {
    Array<int,0> a0;
    a0() = 123;

    auto a0v(a0);
    CPPUNIT_ASSERT(a0v() == 123);
    a0() = 456;
    CPPUNIT_ASSERT(a0v() == 456);
    a0v() = 789;
    CPPUNIT_ASSERT(a0() == 789);

    auto a0c = a0.copy();
    a0() = 456;
    CPPUNIT_ASSERT(a0() == 456
        && a0c() == 789
        && a0v() == 456);
  }

  void test_iterators()
  {
#if 0
    Array<int,0> a0;
    a0() = 123;
    a0.map(incr);
    CPPUNIT_ASSERT(a0() == 124);
#endif
    
    Array<int,1> a1(127);
    for (unsigned int i = 0; i < a1.size(); i++)
      a1(i) = i;

    int i = 0;
    for (auto val : a1) {
      if (i == 67) {
        CPPUNIT_ASSERT(val == 67);
        break;
      }
      i++;
    }
  }
#if 0
  void
  testViews()
  {
    unsigned int dims[] = {3, 2, 4};
    cuiloa::Array<int,3> a(dims);
    int i = 0;
    
    for (auto z = 0; z < 3; z++)
      for (auto y = 0; y < 2; y++)
	for (auto x = 0; x < 4; x++)
	  a(z,y,x) = i++;
    
    unsigned int shape[] = {3, 1, 2};
    unsigned int offset[] = {0, 1, 1};
#if 0
    auto b = a.view(shape, offset);
    CPPUNIT_ASSERT(b.dimensions()[0] == 3);
    CPPUNIT_ASSERT(b.dimensions()[2] == 2);
    
    CPPUNIT_ASSERT(b(0,0,0) == 5);
    CPPUNIT_ASSERT(b(1,0,0) == 13);
    CPPUNIT_ASSERT(b(2,0,1) == 22);
#endif
  }
#endif

#if 0
  void
  testInrCodec()
  {
    cuiloa::Array<double> a = cuiloa::inr_load<double>("scene-bars.inr");
    
    CPPUNIT_ASSERT(a.dimensions()[1] == 3);
    CPPUNIT_ASSERT(a.dimensions()[3] == 50);
    
    CPPUNIT_ASSERT(a(4,0,23,30) == 255.);
    CPPUNIT_ASSERT(a(2,1,21,24) == 43);
  }
#endif

  static CppUnit::Test*
  suite()
  {
    CppUnit::TestSuite* s = new CppUnit::TestSuite("ArrayTestSuite");
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_creation", &ArrayTestCase::test_creation));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_dimensions", &ArrayTestCase::test_dimensions));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_strides", &ArrayTestCase::test_strides));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_indices", &ArrayTestCase::test_indices));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_data", &ArrayTestCase::test_data));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_slices", &ArrayTestCase::test_slices));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_maps", &ArrayTestCase::test_maps));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_copies", &ArrayTestCase::test_copies));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_iterators", &ArrayTestCase::test_iterators));
#if 0
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testViews", &ArrayTestCase::testViews));
    /*s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testInrCodec", &ArrayTestCase::testInrCodec));*/
#endif
    return s;
  }
};

#endif
