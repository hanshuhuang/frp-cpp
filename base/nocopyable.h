#ifndef FRP_BASE_NONCOPYABLE_H
#define FRP_BASE_NONCOPYABLE_H

namespace FRP
{

class noncopyable
{
 public:
  noncopyable(const noncopyable&) = delete;
  void operator=(const noncopyable&) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

} 

#endif  // FRP_BASE_NONCOPYABLE_H
