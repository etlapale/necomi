#pragma once

#include <cstdio>

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include <cuiloa/cuiloa.h>
using namespace cuiloa;

static const char* path = "test-hdf5.h5";

class HDF5TestCase : public CppUnit::TestCase
{
public:
  void test_loadsave0() {
    Array<int,0> a0;
    a0() = 94;
    hdf5_save(a0, path, "a0");

    auto b0 = hdf5_load<int,0>(path, "a0");
    CPPUNIT_ASSERT(b0.dimensions() == a0.dimensions());
    CPPUNIT_ASSERT(b0() == a0());

    remove(path);
  }

  void test_loadsave3() {
    Array<int,3> a3(2,3,4);
    a3.map([](auto& path, auto& val) {
        val = path[0]*12 + path[1] * 4 + path[2];
      });
    hdf5_save(a3, path, "a3");
    
    auto b3 = hdf5_load<int,3>(path, "a3");
    CPPUNIT_ASSERT(a3.dimensions() == b3.dimensions());
    CPPUNIT_ASSERT(a3(1,2,3) == b3(1,2,3) && a3(0,0,0) == b3(0,0,0));

    remove(path);
  }

  static CppUnit::Test*
  suite()
  {
    CppUnit::TestSuite* s = new CppUnit::TestSuite("ArrayTestSuite");
    s->addTest(new CppUnit::TestCaller<HDF5TestCase>
	       ("load/save dim0 HDF5", &HDF5TestCase::test_loadsave0));
    s->addTest(new CppUnit::TestCaller<HDF5TestCase>
	       ("load/save dim3 HDF5", &HDF5TestCase::test_loadsave3));
    return s;
  }
};
