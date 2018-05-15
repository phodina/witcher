AS = as
CPP = g++
LD = ld

ASPARAMS = --32
CPPPARAMS = -m32
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
