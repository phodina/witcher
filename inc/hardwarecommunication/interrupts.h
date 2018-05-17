#ifndef __MYOS_HARDWARECOMMUNICATION_INTERRUPTS_H
#define __MYOS_HARDWARECOMMUNICATION_INTERRUPTS_H

#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <gdt.h>

using namespace myos::common;

namespace myos {
  namespace hardwarecommunication {
    
class InterruptManager;

class InterruptHandler
{
 protected:
  uint8_t interruptNumber;
  InterruptManager *interruptManager;
  InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
  ~InterruptHandler();
 public:
  virtual uint32_t handleInterrupt(uint32_t esp); 
};

class InterruptManager
{
  friend class InterruptHandler;
 protected:

  static InterruptManager *activeInterruptManager;
  InterruptHandler* handlers[256];
  
  struct GateDescriptor
  {
    uint16_t handlerAddress_lo;
    uint16_t gdt_codeSegmentSelector;
    uint8_t reserved;
    uint8_t access;
    uint16_t handlerAddress_ho;
  } __attribute__((packed));

  static GateDescriptor interruptDescriptorTable[256];
  
  struct InterruptDescriptorTablePointer
  {
    uint16_t size;
    uint32_t base;
  } __attribute__((packed));
  
  static void setInterruptDescriptorTableEntry(uint8_t interruptNumber, uint16_t codeSegmentSelectorOffset, void (*handler)(), uint8_t DescriptorPriviledgeLevel, uint8_t DescriptorType);

  myos::hardwarecommunication::Port8BitSlow picMasterCommand;
  myos::hardwarecommunication::Port8BitSlow picMasterData;
  myos::hardwarecommunication::Port8BitSlow picSlaveCommand;
  myos::hardwarecommunication::Port8BitSlow picSlaveData;
 public:
  InterruptManager(myos::GlobalDescriptorTable *gdt);
  ~InterruptManager();
  
  static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);
  uint32_t doHandleInterrupt(uint8_t interruptNumber, uint32_t esp);
  
  void activate();
  void deactivate();
  
  static void IgnoreInterruptRequest();
  static void HandleInterruptRequest0x00();
  static void HandleInterruptRequest0x01();
  static void HandleInterruptRequest0x0C();

};

  }
}
#endif
