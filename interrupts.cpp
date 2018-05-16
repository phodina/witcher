#include "interrupts.h"

void printf(char *str);


InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager)
{
  this->interruptNumber = interruptNumber;
  this->interruptManager = interruptManager;
  interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler() {
  if (interruptManager->handlers[interruptNumber] == this) {
    interruptManager->handlers[interruptNumber] = 0;
  }
}

uint32_t InterruptHandler::handleInterrupt(uint32_t esp)
{
  return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::activeInterruptManager = 0;

void InterruptManager::setInterruptDescriptorTableEntry(uint8_t interruptNumber, uint16_t codeSegmentSelectorOffset, void (*handler)(), uint8_t DescriptorPriviledgeLevel, uint8_t DescriptorType)
{

  const uint8_t IDT_DESC_PRESENT = 0x80;
    
  interruptDescriptorTable[interruptNumber].handlerAddress_lo = ((uint32_t)handler) & 0xffff;
  interruptDescriptorTable[interruptNumber].handlerAddress_ho = (((uint32_t)handler) >> 16) & 0xffff;
  interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
  interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPriviledgeLevel &0x03) << 5);
  interruptDescriptorTable[interruptNumber].reserved = 0;
    
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt) : picMasterCommand(0x20), picMasterData(0x21),picSlaveCommand(0xA0),picSlaveData(0xA1){

  uint16_t codeSegment = gdt->CodeSegmentSelector();
  const uint8_t IDT_INTERRUPT_GATE = 0x0E;

  for (uint16_t i=0; i < 256; i++)
    {
      handlers[i] = 0;
      setInterruptDescriptorTableEntry(i, codeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

  setInterruptDescriptorTableEntry(0x20, codeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
  setInterruptDescriptorTableEntry(0x21, codeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

  picMasterCommand.write(0x11);
  picSlaveCommand.write(0x11);

  picMasterData.write(0x20);
  picSlaveData.write(0x28);

  picMasterData.write(0x04);
  picSlaveData.write(0x02);

  picMasterData.write(0x01);
  picSlaveData.write(0x01);

  picMasterData.write(0x00);
  picSlaveData.write(0x00);
  
  InterruptDescriptorTablePointer idt;
  idt.size = 256 * sizeof(GateDescriptor) - 1;
  idt.base = (uint32_t)interruptDescriptorTable;
  asm volatile("lidt %0": : "m" (idt));
}

InterruptManager::~InterruptManager() {

}

void InterruptManager::activate() {

  if (activeInterruptManager !=0) {

    activeInterruptManager->deactivate();
  }
  activeInterruptManager = this;
  asm volatile ("sti");
}

void InterruptManager::deactivate(){

  if (activeInterruptManager != this) {
    activeInterruptManager = 0;
    asm volatile ("cli");
  }
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp) {

  if (activeInterruptManager != 0) {
    return activeInterruptManager->doHandleInterrupt(interruptNumber, esp);
  }
  
  return esp;
}

uint32_t InterruptManager::doHandleInterrupt(uint8_t interruptNumber, uint32_t esp) {

  if (handlers[interruptNumber] != 0) {

    esp = handlers[interruptNumber]->handleInterrupt(esp);
  }
  else if (interruptNumber != 0x20) {
    char* msg = "Unhandled Interrupt 0x00";
    char* hex = "01234567890ABCDEF";
    msg[22] = hex[(interruptNumber >> 4) & 0xF];
    msg[23] = hex[interruptNumber & 0x0F];
    printf(msg);
  }

  if (0x20 <= interruptNumber && interruptNumber < 0x30) {

    picMasterCommand.write(0x20);
    if (0x28 <= interruptNumber) {
      picSlaveCommand.write(0x20);
    }
  }
    
  return esp;
}
