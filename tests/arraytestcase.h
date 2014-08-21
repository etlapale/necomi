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
#if 0
  void test_indexing()
  {
    unsigned int dims[] = {3, 4};
    Array<double,2> a(dims);

    a(1,1) = 124;

    CPPUNIT_ASSERT(true);
  }

  void
  testStrides()
  {
    unsigned int dims[] = {4, 5, 6};
    cuiloa::Array<double,3> a(dims);
    CPPUNIT_ASSERT(a.strides()[0] == 30);
  }

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
#if 0
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("test_indexing", &ArrayTestCase::test_indexing));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testStrides", &ArrayTestCase::testStrides));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testViews", &ArrayTestCase::testViews));
    /*s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testInrCodec", &ArrayTestCase::testInrCodec));*/
#endif
    return s;
  }
};

#endif
