#ifndef __CUILOA_ARRAY_TEST_CASE
#define __CUILOA_ARRAY_TEST_CASE

#include <iostream>

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include <cuiloa.h>


using namespace std;


class ArrayTestCase : public CppUnit::TestCase
{
public:
  void
  testCreation()
  {
    int dims[] = {100, 100, 25};
    cuiloa::Array<double> a(3, dims);
    CPPUNIT_ASSERT(a.size() == 100*100*25);
  }

  void
  testStrides()
  {
    int dims[] = {4, 5, 6};
    cuiloa::Array<double> a(3, dims);
    CPPUNIT_ASSERT(a.strides()[0] == 30);
  }

  void
  testViews()
  {
    int dims[] = {3, 2, 4};
    cuiloa::Array<int> a(3, dims);
    int i = 0;
    
    for (size_t z = 0; z < 3; z++)
      for (size_t y = 0; y < 2; y++)
	for (size_t x = 0; x < 4; x++)
	  a(z,y,x) = i++;
    
    int shape[] = {3, 1, 2};
    int offset[] = {0, 1, 1};
    cuiloa::Array<int> b = a.view(shape, offset);
    CPPUNIT_ASSERT(b.dimensions()[0] == 3);
    CPPUNIT_ASSERT(b.dimensions()[2] == 2);
    
    CPPUNIT_ASSERT(b(0,0,0) == 5);
    CPPUNIT_ASSERT(b(1,0,0) == 13);
    CPPUNIT_ASSERT(b(2,0,1) == 22);
  }

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
	       ("testCreation", &ArrayTestCase::testCreation));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testStrides", &ArrayTestCase::testStrides));
    s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testViews", &ArrayTestCase::testViews));
    /*s->addTest(new CppUnit::TestCaller<ArrayTestCase>
	       ("testInrCodec", &ArrayTestCase::testInrCodec));*/
    return s;
  }
};

#endif
