#include "mouse.h"

void printf(char *str);

MouseEventHandler::MouseEventHandler(){}

void MouseEventHandler::onActivate(){}
void MouseEventHandler::onMouseMove(int dx, int dy){}
void MouseEventHandler::onMouseUp(uint8_t button){}
void MouseEventHandler::onMouseDown(uint8_t button){}

MouseToConsole::MouseToConsole(){
  x= 40;
  y= 12;
  
static uint16_t* VideoMemory = (uint16_t*)0xb8000;

  VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >>4) | ((VideoMemory[80*y+x] & 0x0f00) << 4) | (VideoMemory[80*y+x] & 0x00ff);
}

void MouseToConsole::onMouseMove( int dx, int dy){
static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >>4) | ((VideoMemory[80*y+x] & 0x0f00) << 4) | ((VideoMemory[80*y+x] & 0x00ff));

    x += dx;
    if (x < 0){
      x = 0;
    }
    else if (x>=80) {
      x = 79;
    }

    y +=dy;
    if (y < 0) {
      y = 0;
    }
    else if (y >= 25) {
      y = 24;
    }
    
    VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xf000) >>4) | ((VideoMemory[80*y+x] & 0x0f00) << 4) | ((VideoMemory[80*y+x] & 0x00ff));
}
MouseDriver::MouseDriver(InterruptManager *manager, MouseEventHandler *handler) : InterruptHandler(0x2C, manager), dataport(0x60), commandport(0x64), handler(handler) {}

void MouseDriver::activate(){
  offset = 0;
  buttons = 0;

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
  if (!(status & 0x20) || handler == 0) {
    return esp;
  }
  
  buffer[offset] = dataport.read();
  offset = (offset +1) % 3;

  if (offset == 0) {

    if (buffer[1] != 0 || buffer[2] != 0) {
      handler->onMouseMove(buffer[1],-buffer[2]);
    }

      for(uint8_t i=0; i<3;i++)
	{
	  if ((buffer[0] & (0x1<<i)) != (buttons & (0x01<<i)))
	    {
	      if (buttons & (0x01<<i)) {
		handler->onMouseUp(i+1);
	      }
	      else
		{
		  handler->onMouseDown(i+1);
		}
	    }
	}
      buttons = buffer[0];
    }
  return esp;
}
