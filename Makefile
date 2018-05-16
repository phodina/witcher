AS = as
CPP = g++
LD = ld

ASPARAMS = --32
CPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector
LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o kernel.o interrupts.o interruptstubs.o

all: mykernel.bin

%.o: %.cpp
	$(CPP) $(CPPPARAMS) -o $@ -c $<

%.o: %.s
	$(AS) $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
	$(LD) $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

iso: mykernel.iso

mykernel.iso: mykernel.bin
	mkdir -p iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My operating System"' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

qemu: mykernel.iso
	(killall qemu-system-i386 && sleep 1) || true
	qemu-system-i386 -boot d -cdrom $< -m 256 &
qemu64: mykernel.iso
	(killall qemu-system-x86_64 && sleep 1) || true
	qemu-system-x86_64 -display curses -boot d -cdrom $< -m 512 &
.PHONY: clean
clean:
	rm -f $(objects) mykernel.bin mykernel.iso
