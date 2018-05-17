#ifndef __MYOS_DRIVERS_KEYBOARD_H
#define __MYOS_DRIVERS_KEYBOARD_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

using namespace myos::common;

namespace myos{
  namespace drivers{
    
class KeyboardEventHandler
{
 public:
  KeyboardEventHandler();

  virtual void onKeyDown(char ch);
  virtual void onKeyUp(char ch);
};

class PrintfKeyboardEventHandler : public KeyboardEventHandler {

 public:
  void onKeyDown(char ch) override;
};


class KeyboardDriver: public myos::hardwarecommunication::InterruptHandler, public myos::drivers::Driver
{
  myos::hardwarecommunication::Port8Bit dataport;
  myos::hardwarecommunication::Port8Bit commandport;
  KeyboardEventHandler* handler;
 public:
  KeyboardDriver(myos::hardwarecommunication::InterruptManager* manager, KeyboardEventHandler *handler);
  ~KeyboardDriver();
  virtual uint32_t handleInterrupt(uint32_t esp);
  virtual void activate();
};
  }
}
#endif
