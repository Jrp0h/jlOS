rm -rf isodir
rm -rf build

mkdir -p isodir/boot/grub
mkdir build

i686-elf-as boot.s -o build/boot.o
i686-elf-gcc -c kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
# i686-elf-g++ -c kernel.cpp -o build/kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti
i686-elf-gcc -T linker.ld -o build/jlos.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc

cp build/jlos.bin isodir/boot
cp grub.cfg isodir/boot/grub

grub-mkrescue -o build/jlos.iso isodir
