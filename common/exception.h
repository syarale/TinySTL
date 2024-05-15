#ifndef COMMON_EXCEPTION_H_
#define COMMON_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace sgi {

class invalid_alloc : public std::bad_alloc {
 public:
  invalid_alloc(const std::string& msg) : msg_(msg) {}
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_;
};

}  // namespace sgi

#endif  // COMMON_EXCEPTION_H_