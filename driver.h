#ifndef __DRIVER_H
#define __DRIVER_H

#include "types.h"

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
#endif
