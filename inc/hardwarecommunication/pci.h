#ifndef __MYOS_HARDWARECOMMUNICATION_PCI_H
#define __MYOS_HARDWARECOMMUNICATION_PCI_H

#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>

using namespace myos::drivers;
using namespace myos::common;

namespace myos {
  namespace hardwarecommunication{

    enum BaseAddressRegisterType
    {
      MemoryMapping = 0,
      InputOutput = 1
    };

    class BaseAddressRegister
    {
    public:
      bool prefetchable;
      uint8_t *address;
      uint32_t size;
      BaseAddressRegisterType type;
    };
    
    class PeripheralComponentInterconnectDeviceDescriptor
    {
    public:
      PeripheralComponentInterconnectDeviceDescriptor();
      ~PeripheralComponentInterconnectDeviceDescriptor();

      uint32_t portBase;
      uint32_t interrupt;

      uint16_t bus;
      uint16_t device;
      uint16_t function;

      uint16_t vendor_id;
      uint16_t device_id;

      uint8_t class_id;
      uint8_t subclass_id;
      uint8_t interface_id;

      uint8_t revision;
    };

    class PeripheralComponentInterconnectController
    {
      Port32Bit dataPort;
      Port32Bit commandPort;
    public:
      PeripheralComponentInterconnectController();
      ~PeripheralComponentInterconnectController();

      uint32_t read(uint16_t bus, uint16_t device, uint16_t function, uint32_t register_offset);
      void write(uint16_t bus, uint16_t device, uint16_t function, uint32_t register_offset, uint32_t value);

      bool detectHasFunctions(uint16_t bus, uint16_t device);

      void selectDrivers(DriverManager *driverManager, InterruptManager *interrupts);
      PeripheralComponentInterconnectDeviceDescriptor getDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function);

      BaseAddressRegister getBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);

      Driver* getDriver(PeripheralComponentInterconnectDeviceDescriptor *dev,InterruptManager *interrupts);
    };
  }
}
#endif
