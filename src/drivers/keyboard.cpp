#include <drivers/keyboard.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printfHex(uint8_t val);
void printf(char *str);

KeyboardEventHandler::KeyboardEventHandler() {}

void KeyboardEventHandler::onKeyDown(char ch)
{
}

void KeyboardEventHandler::onKeyUp(char ch)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager *manager, KeyboardEventHandler *handler) : InterruptHandler(0x21, manager), dataport(0x60), commandport(0x64), handler(handler) {}

void KeyboardDriver::activate(){

  while (commandport.read() & 0x01) {
    dataport.read();
  }
  
  commandport.write(0xAE); // activate interrupts
  commandport.write(0x20); // give current state
  uint8_t status = (dataport.read() | 1) & ~0x10;
  commandport.write(0x60); // set state
  dataport.write(status);

  dataport.write(0xF4); // activate keyboard
  
}

KeyboardDriver::~KeyboardDriver() {}

void PrintfKeyboardEventHandler::onKeyDown(char ch)
  {
    char * foo = " ";
    foo[0] = ch;
    printf(foo);
  }


uint32_t KeyboardDriver::handleInterrupt(uint32_t esp) {

  uint8_t key = dataport.read();

  if (handler == 0){

    return esp;
  }
  
  if (key < 0x80) {
    switch(key) {
    case 0xC5:
    case 0xfa:
    case 0x45:
      break;
    default:
      printf("Keyboard 0x");
      printfHex(key);
    };
  }
  return esp;
}
