#include "types.h"
#include "gdt.h"

void printf(char* str) {

  uint16_t* VideoMemory = (uint16_t*)0xb8000;

  for(uint16_t i = 0; str[i]!= '\0'; ++i) {
    VideoMemory[i] = (VideoMemory[i] & 0xff00) | str[i];
  }
}

extern "C" void kernelMain (void *multiboot_structure, uint32_t magic_number) {

  printf("Hello Witcher!");

  GlobalDescriptorTable gdt;
  
  while(1);
}
