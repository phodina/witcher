#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"


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


class KeyboardDriver: public InterruptHandler, public Driver
{
  Port8Bit dataport;
  Port8Bit commandport;
  KeyboardEventHandler* handler;
 public:
  KeyboardDriver(InterruptManager* manager, KeyboardEventHandler *handler);
  ~KeyboardDriver();
  virtual uint32_t handleInterrupt(uint32_t esp);
  virtual void activate();
};
#endif
