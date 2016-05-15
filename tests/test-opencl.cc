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

  const std::string& name()
  {
    return platform_info(m_name, CL_PLATFORM_NAME);
  }

  const std::string& vendor()
  {
    return platform_info(m_vendor, CL_PLATFORM_VENDOR);
  }
  
  const std::string& version()
  {
    return platform_info(m_version, CL_PLATFORM_VERSION);
  }
  
  const std::string& profile()
  {
    return platform_info(m_profile, CL_PLATFORM_PROFILE);
  }
  
  const std::string& extensions()
  {
    return platform_info(m_extensions, CL_PLATFORM_EXTENSIONS);
  }
    
private:
  cl_platform_id m_id;
  std::string m_name;
  std::string m_vendor;
  std::string m_version;
  std::string m_profile;
  std::string m_extensions;

  const std::string& platform_info(std::string& val, cl_platform_info info)
  {
    if (val.empty()) {
      std::size_t len;
      auto res = clGetPlatformInfo(m_id, info, 0, nullptr, &len);
      val.resize(len+1);
      res = clGetPlatformInfo(m_id, info, len, &val[0], &len);
    }
    return val;
  }
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
      std::cout << "Platform ‘" << p.name() << "’ from "
		<< p.vendor() << " supporting " << p.version()
		<< " / " << p.profile()
		<< std::endl;
    }
  }
}
