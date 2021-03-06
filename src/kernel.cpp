#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <hardwarecommunication/pci.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char *str);

void printfHex(uint8_t key){

  char *foo = "00";
  char *hex = "0123456789ABCDEF";
  foo[0] = hex[(key >> 4) & 0x0f];
  foo[1] = hex[key & 0x0f];
  printf(foo);
}

void printf(char* str) {

  static uint16_t* VideoMemory = (uint16_t*)0xb8000;

  static uint8_t x=0, y=0;

  for(uint16_t i = 0; str[i]!= '\0'; ++i) {
    switch (str[i]) {
    case '\n':
      y++;
      x = 0;
      break;
    default:
      VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xff00) | str[i];
      x++;
      break;

    }
    if (x >= 80) {
      y++;
      x = 0;
    }

    if (y >= 25) {

      for (y=0; y<25; y++) {
	for (x=0;x<80;x++) {
	  VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xff00) | ' ';
	}
      }
      
      x = 0;
      y = 0;
    }
  }
}

extern "C" void kernelMain (void *multiboot_structure, uint32_t magic_number) {

  printf("Hello Witcher!\n");

  printf("Initializing Hardware, Stage 1\n");
  
  GlobalDescriptorTable gdt;
  InterruptManager interrupts(&gdt);

  DriverManager drvManager;

  PrintfKeyboardEventHandler kbhandler;
  KeyboardDriver keyboard(&interrupts, &kbhandler);
  drvManager.addDriver(&keyboard);

  MouseToConsole mousehandler;
  MouseDriver mouse(&interrupts, &mousehandler);
  drvManager.addDriver(&mouse);

  PeripheralComponentInterconnectController pciController;
  pciController.selectDrivers(&drvManager, &interrupts);
  
  printf("Initializing Hardware, Stage 2\n");
  drvManager.activateAll();
  
  printf("Initializing Hardware, Stage 3\n");
  interrupts.activate();

  while(1);
}


