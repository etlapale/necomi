#include <iostream>

#include <CL/opencl.h>

#include "Catch/include/catch.hpp"

namespace necomi {


namespace cl {

class Platform
{
public:
  Platform(cl_platform_id id)
    : m_id(id)
  {}
  std::string name()
  {
    if (m_name.empty()) {
      std::size_t len;
      auto res = clGetPlatformInfo(m_id, CL_PLATFORM_NAME, 0, nullptr, &len);
      m_name.resize(len+1);
      res = clGetPlatformInfo(m_id, CL_PLATFORM_NAME, len, &m_name[0], &len);
    }
    return m_name;
  }
private:
  cl_platform_id m_id;
  std::string m_name;
};

std::vector<cl_platform_id> platforms()
{
  cl_uint num_platforms;
  auto res = clGetPlatformIDs(0, nullptr, &num_platforms);
  std::cout << num_platforms << " OpenCL platform(s) available ("
	    << res << ")" << std::endl;
  
  std::vector<cl_platform_id> platforms;
  platforms.resize(num_platforms);
  res = clGetPlatformIDs(num_platforms, &platforms[0], &num_platforms);
  std::cout << num_platforms << " OpenCL platform(s) read ("
	    << res << ")" << std::endl;

  return platforms;
}

} // namespace cl
} // namespace necomi

TEST_CASE( "OpenCL arrays", "[arrays]" ) {
  SECTION( "OpenCL context" ) {
    // Get the platforms
    auto platforms = necomi::cl::platforms();
    for (auto id : platforms) {
      necomi::cl::Platform p(id);
      std::cout << "Platform ‘" << p.name() << "’" << std::endl;
    }
  }
}
