#ifndef __MYOS_DRIVERS_MOUSE_H
#define __MYOS_DRIVERS_MOUSE_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

using namespace myos::common;

namespace myos {
  namespace drivers{
class MouseEventHandler
{
 public:
  MouseEventHandler();

  virtual void onActivate();
  virtual void onMouseDown(uint8_t button);
  virtual void onMouseUp(uint8_t button);
  virtual void onMouseMove(int x, int y);
};

class MouseToConsole : public MouseEventHandler
{
  int8_t x,y;
 public:
  MouseToConsole();
  void onMouseMove(int x, int y) override;
};

class MouseDriver: public myos::hardwarecommunication::InterruptHandler, public myos::drivers::Driver
{
  myos::hardwarecommunication::Port8Bit dataport;
  myos::hardwarecommunication::Port8Bit commandport;

  uint8_t buffer[3];
  uint8_t offset;
  uint8_t buttons;
  MouseEventHandler *handler;
  
 public:
  MouseDriver(myos::hardwarecommunication::InterruptManager* manager, MouseEventHandler *handler);
  ~MouseDriver();
  virtual uint32_t handleInterrupt(uint32_t esp);
  virtual void activate();
};
  }
}
#endif
