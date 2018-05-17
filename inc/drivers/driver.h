#ifndef __MYOS_DRIVERS_DRIVER_H
#define __MYOS_DRIVERS_DRIVER_H

#include <common/types.h>

using namespace myos::common;

namespace myos {
  namespace drivers {
class Driver
{
 public:
  Driver();
  ~Driver();

  virtual void activate();
  virtual int32_t reset();
  virtual void deactivate();
};

class DriverManager
{
 private:
  Driver *drivers[255];
  int32_t numDrivers;
  
 public:
  DriverManager();
  ~DriverManager();
  void addDriver(Driver* driver);
  void activateAll();
};
  }
}
#endif
