#include <iostream>

#include <CL/opencl.h>

#include "Catch/include/catch.hpp"

namespace necomi {


} // namespace necomi

TEST_CASE( "OpenCL arrays", "[arrays]" ) {
  SECTION( "OpenCL context" ) {
    cl_uint num_platforms;
    auto res = clGetPlatformIDs(0, nullptr, &num_platforms);
    std::cout << num_platforms << " OpenCL platform(s) available" << std::endl;
  }
}
