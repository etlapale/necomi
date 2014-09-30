#pragma once

#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>

#include <cuiloa/cuiloa.h>
using namespace cuiloa;

class RandomTestCase : public CppUnit::TestCase
{
public:
  void
  test_mersenne_normal()
  {
    // Setup a random number generator
    RandomDevSeedSequence rdss;
    std::mt19937_64 prng(rdss);

    // Create a array filled from a normal distribution
    ArrayIndex size = 1e6;
    double avg = 95, dev = 4.3;
    auto a = normal_distribution<double>(avg, dev, size, prng);

    // Check the statistics
    CPPUNIT_ASSERT(fabs(a.sum()/size - avg) < 1e-1);
    double std = 0;
    a.map([&std,avg](auto& path, auto& val) {
	(void) path;
	std += (val-avg)*(val-avg);
      });
    std = sqrt(std/size);
    CPPUNIT_ASSERT(fabs(std-dev) < 1e-2);
  }

  static CppUnit::Test*
  suite()
  {
    CppUnit::TestSuite* s = new CppUnit::TestSuite("RandomTestSuite");
    s->addTest(new CppUnit::TestCaller<RandomTestCase>
	       ("Mersenne Twister 64 bits with normal distribution",
		&RandomTestCase::test_mersenne_normal));
    return s;
  }
};

// Local variables:
// mode: c++
// End:
