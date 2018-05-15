AS = as
CPP = g++
LD = ld

ASPARAMS = --32
CPPPARAMS = -m32 -fno-use-c-cxa-atexit -nostflib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
LDPARAMS = -melf_i386

objects = loader.o kernel.o

%.o: %.cpp
	$(CPP) $(CPPPARAMS) -o $@ -c $<

%.o: %.s
	$(AS) $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	$(LD) $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin
