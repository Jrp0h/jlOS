.PHONY: all

PREFIX=i686-elf
CC=$(PREFIX)-gcc
AS=$(PREFIX)-as

CFLAGS=-std=gnu99 -ffreestanding -O2 -nostdlib -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib -lgcc

CSRC=$(wildcard src/*.c)
ASSRC=$(wildcard src/*.s)
HEADERS=$(wildcard src/*.h)

COBJS=$(patsubst src/%.c, obj/%.o, $(CSRC))
ASOBJS=$(patsubst src/%.s, obj/%.o, $(ASSRC))

all: iso

obj/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(ASOBJS): $(ASSRC)
	$(AS) $< -o $@

jlos.bin: $(ASOBJS) $(COBJS)
	$(CC) -T linker.ld -o build/jlos.bin obj/boot.o obj/kernel.o obj/terminal.o $(LDFLAGS)


iso: jlos.bin
	mkdir -p isodir/boot/grub obj build
	cp build/jlos.bin isodir/boot/
	cp grub.cfg isodir/boot/grub

	grub-mkrescue -o build/jlos.iso isodir

run: iso
	qemu-system-i386 -cdrom build/jlos.iso

clean:
	rm -rf build obj isodir
	mkdir -p isodir/boot/grub obj build
	cp grub.cfg isodir/boot/grub