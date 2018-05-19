#include <hardwarecommunication/pci.h>

// lspci -n (shows vendor and device id)
// lspci -x (dumps pci config space)

using namespace myos::common;
using namespace myos::hardwarecommunication;

void printf(char *str);
void printfHex(uint8_t hex);

PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor() {}

PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor(){}

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController() : dataPort(0xCFC), commandPort(0xCF8)
{
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
}

uint32_t PeripheralComponentInterconnectController::read(uint16_t bus, uint16_t device, uint16_t function, uint32_t register_offset)
{
  uint32_t id = (0x1 << 31) | ((bus & 0xff) << 16) | ((device & 0x1f) << 11) | ((function & 0x07) << 8) | ((register_offset & 0xfc));

  commandPort.write(id);
  uint32_t result = dataPort.read();

  return result >> (8 * (register_offset % 4));
}

void PeripheralComponentInterconnectController::write(uint16_t bus, uint16_t device, uint16_t function, uint32_t register_offset, uint32_t value)
{
  uint32_t id = (0x1 << 31) | ((bus & 0xff) << 16) | ((device & 0x1f) << 11) | ((function & 0x07) << 8) | ((register_offset & 0xfc));

  commandPort.write(id);
  dataPort.write(value);
}
      
bool PeripheralComponentInterconnectController::detectHasFunctions(uint16_t bus, uint16_t device) {

  return read(bus, device, 0, 0x0E) & (1<<7);
}

void PeripheralComponentInterconnectController::selectDrivers(DriverManager *driverManager, InterruptManager *interrupts){

  for (int bus=0; bus<8; bus++) {

    for (int device=0; device < 32; device++)
      {
	int numFunctions = detectHasFunctions(bus, device)? 8:1;
	for (int function = 0; function < numFunctions; function++) {
	 PeripheralComponentInterconnectDeviceDescriptor dev = getDeviceDescriptor(bus,device, function);

	 if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xffff)
	   {
	     continue;
	   }

	 for (int barNum=0; barNum < 6; barNum++)
	   {
	     BaseAddressRegister bar = getBaseAddressRegister(bus, device, function, barNum);
	     
	     if (bar.address && (bar.type == BaseAddressRegisterType::InputOutput)) {
	       dev.portBase = (uint32_t) bar.address;

	       Driver *driver = getDriver(&dev, interrupts);
	       if (driver != 0) {
		 driverManager->addDriver(driver);
	       }
	     }
	   }
	 
	 printf("PCI BUS ");
	 printfHex(bus & 0xff);

	 printf(", DEVICE ");
	 printfHex(device & 0xff);

	 printf(", FUNC ");
	 printfHex(function & 0xff);

	 printf(", VID ");
	 printfHex((dev.vendor_id & 0xff00) >> 8);
	 printfHex(dev.vendor_id & 0xff);
	 printf(", DID");
	 printfHex((dev.device_id & 0xff00) >> 8);
	 printfHex(dev.device_id & 0xff);
	 printf("\n");
	}
      }
  }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::getDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
  PeripheralComponentInterconnectDeviceDescriptor result;
  result.bus = bus;
  result.device = device;
  result.function = function;

  result.vendor_id = read(bus,device,function, 0x00);
  result.device_id = read(bus,device,function, 0x02);
  
  result.class_id = read(bus,device,function, 0x0b);
  result.subclass_id = read(bus,device,function, 0x0a);
  result.interface_id = read(bus,device,function, 0x09);

  result.revision = read(bus,device,function, 0x08);
  result.interrupt = read(bus,device,function, 0x3c);
  
  return result;
}

BaseAddressRegister PeripheralComponentInterconnectController::getBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar){
  BaseAddressRegister result;

  uint32_t headerType = read(bus, device, function, 0x0E) & 0x7F;
  int maxBARs = 6-(4*headerType);
  if (bar>= maxBARs)
    {
    return result;
    }

  uint32_t bar_value = read(bus, device, function, 0x10 + 4 * bar);
  result.type = (bar & 0x01) ? BaseAddressRegisterType::InputOutput : BaseAddressRegisterType::MemoryMapping;

  uint32_t temp;

  if (result.type == BaseAddressRegisterType::MemoryMapping){

    switch((bar_value >> 1) & 0x3) {
    case 0: // 32bit mode
      break;
    case 1: // 20bit mode
      break;
    case 2: // 64bit mode
      break;
    default:
      break;
    }

    result.prefetchable = ((bar_value >> 3) & 0x01) == 0x01;
  }
  else {
    result.address = (uint8_t*) (bar_value & ~0x03);
    result.prefetchable = false;
  }
  
  return result;
}


Driver* PeripheralComponentInterconnectController::getDriver(PeripheralComponentInterconnectDeviceDescriptor *dev,InterruptManager *interrupts)
{
  switch(dev->vendor_id)
    {
    case 0x1022: // AMD
      switch(dev->device_id)
	{
	case 0x2000: // am79c973
	  break;
	}
      break;
    case 0x8086: // Intel
      printf("Intel VID\n");
      break;
      
    }

  switch(dev->class_id)
    {
    case 0x03: // Graphics
      switch(dev->subclass_id)
	{
	case 0x00: // VGA
	  break;
	}
      break;
    }
  
  return 0;
}
