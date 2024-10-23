#include "Request.h"

namespace Lovense
{
  void Request::WaitForResult()
  {
    std::unique_lock lk{ _m };
    cv.wait(lk, [this] { return result.has_value() || error.has_value(); });
  }

  void Request::SetResult(const json& a_result)
  {
    std::lock_guard lk{ _m };
    result = a_result;
    cv.notify_all();
  }

  void Request::SetFailure(const std::string& a_str)
  {
    std::lock_guard lk{ _m };
    error = a_str;
    cv.notify_all();
  }

}   // namespace Lovense