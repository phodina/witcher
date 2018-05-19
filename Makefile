AS = as
CPP = g++
LD = ld

ASPARAMS = --32 -g
CPPPARAMS = -m32 -g -Wno-write-strings -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector -Iinc
LDPARAMS = -g -melf_i386

objects = obj/loader.o obj/gdt.o obj/hardwarecommunication/port.o obj/kernel.o obj/hardwarecommunication/interrupts.o obj/hardwarecommunication/interruptstubs.o obj/drivers/keyboard.o obj/drivers/mouse.o obj/drivers/driver.o obj/hardwarecommunication/pci.o

all: mykernel.bin
run: qemu

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	$(CPP) $(CPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
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

debug: mykernel.iso
	(killall qemu-system-i386 && sleep 1) || true
	qemu-system-i386 -S -s -boot d -cdrom $< -m 256 &
	gdb --command=gdb_init mykernel.bin
qemu64: mykernel.iso
	(killall qemu-system-x86_64 && sleep 1) || true
	qemu-system-x86_64 -display curses -boot d -cdrom $< -m 512 &
.PHONY: clean
clean:
	rm -rf obj mykernel.bin mykernel.iso
