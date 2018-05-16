#include "keyboard.h"

void printf(char *str);

KeyboardDriver::KeyboardDriver(InterruptManager *manager) : InterruptHandler(0x21, manager), dataport(0x60), commandport(0x64) {

  while (commandport.read() & 0x01) {
    dataport.read();
  }

  commandport.write(0xAE); // activate interrupts
  commandport.write(0x20); // give current state
  uint8_t status = (commandport.read() | 1) & ~0x10;
  commandport.write(0x60); // set state
  dataport.write(status);

  dataport.write(0xF4); // activate keyboard
  
}

KeyboardDriver::~KeyboardDriver() {}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp) {

  uint8_t key = dataport.read();
  
  char* msg = "Keyboard 0x00";
  char* hex = "01234567890ABCDEF";
  msg[11] = hex[(key >> 4) & 0xF];
  msg[12] = hex[key & 0x0F];
  printf(msg);
    
  return esp;
}
