#include "mouse.h"

void printf(char *str);

MouseDriver::MouseDriver(InterruptManager *manager) : InterruptHandler(0x2C, manager), dataport(0x60), commandport(0x64) {

  offset = 0;
  buttons = 0;

  static uint16_t* VideoMemory = (uint16_t*)0xb8000;

  VideoMemory[80*12+40] = ((VideoMemory[80*12+40] & 0xf000) >>4) | ((VideoMemory[80*12+40] & 0x0f00) << 4) | (VideoMemory[80*12+40] & 0x00ff);
    
  commandport.write(0xA8); // activate interrupts
  commandport.write(0x20); // give current state
  uint8_t status = dataport.read() | 2;
  commandport.write(0x60); // set state
  dataport.write(status);

  commandport.write(0xD4);
  dataport.write(0xF4); // activate mouse
  dataport.read();
}

MouseDriver::~MouseDriver() {}

uint32_t MouseDriver::handleInterrupt(uint32_t esp) {

  uint8_t status = commandport.read();
  if (!(status & 0x20)) {
    return esp;
  }

  static int8_t x=40, y=12;
  
  buffer[offset] = dataport.read();
  offset = (offset +1) % 3;

  if (offset == 0) {

    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >>4) | ((VideoMemory[80*y+x] & 0x0f00) << 4) | ((VideoMemory[80*y+x] & 0x00ff));
      
    x += buffer[1];

    if (x < 0){
      x = 0;
    }
    else if (x>=80) {
      x = 79;
    }

    y -= buffer[2];

    if (y < 0) {
      y = 0;
    }
    else if (y >= 25) {
      y = 24;
    }
    
    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >>4) | ((VideoMemory[80*y+x] & 0x0f00) << 4) | ((VideoMemory[80*y+x] & 0x00ff)); 
  }

  return esp;
}
