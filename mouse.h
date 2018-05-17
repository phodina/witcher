#ifndef __MOUSE_H
#define __MOUSE_H

#include "types.h"
#include "driver.h"
#include "interrupts.h"
#include "port.h"

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

class MouseDriver: public InterruptHandler, public Driver
{
  Port8Bit dataport;
  Port8Bit commandport;

  uint8_t buffer[3];
  uint8_t offset;
  uint8_t buttons;
  MouseEventHandler *handler;
  
 public:
  MouseDriver(InterruptManager* manager, MouseEventHandler *handler);
  ~MouseDriver();
  virtual uint32_t handleInterrupt(uint32_t esp);
  virtual void activate();
};
#endif
