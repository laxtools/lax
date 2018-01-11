#pragma once

#include <lax/task/task.h>

namespace lax
{
namespace actor
{

/// scene task
class scene : public task::task
{
public:
  scene(const std::string& name)
  : task()
  {
  }

  virtual ~scene()
  {
  }

  const std::string& get_name() const
  {
    return name_;
  }

protected:
  bool on_start() override;

  void on_execute() override;

  void on_finish() override;

protected:
  std::string name_;
};

} // actor
} // lax
