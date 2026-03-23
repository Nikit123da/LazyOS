FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/memory/memory.o ./build/IO/io.asm.o ./build/PIT/pit.o ./build/drivers/VGA/VGA.o ./build/drivers/keyboard/keyboard.o ./build/idt/ISR/isr.o ./build/idt/ISR/isr.asm.o ./build/idt/PIC/pic.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o


INCLUDES = -I./src 
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc 

all: ./bin/boot.bin ./bin/kernel.bin
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=512 count=100 >> ./bin/os.bin

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o


./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/drivers/VGA/VGA.o: ./src/drivers/VGA/VGA.c
	i686-elf-gcc $(INCLUDES) -I./src/drivers/VGA $(FLAGS) -std=gnu99 -c ./src/drivers/VGA/VGA.c -o ./build/drivers/VGA/VGA.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

./build/IO/io.asm.o: ./src/IO/io.asm
	nasm -f elf -g ./src/IO/io.asm -o ./build/IO/io.asm.o

./build/PIT/pit.o: ./src/PIT/pit.c
	i686-elf-gcc $(INCLUDES) -I./src/PIT $(FLAGS) -std=gnu99 -c ./src/PIT/pit.c -o ./build/PIT/pit.o

./build/drivers/keyboard/keyboard.o: ./src/drivers/keyboard/keyboard.c
	i686-elf-gcc $(INCLUDES) -I./src/drivers/keyboard $(FLAGS) -std=gnu99 -c ./src/drivers/keyboard/keyboard.c -o ./build/drivers/keyboard/keyboard.o

./build/idt/ISR/isr.o: ./src/idt/ISR/isr.c
	i686-elf-gcc $(INCLUDES) -I./src/idt/ISR $(FLAGS) -std=gnu99 -c ./src/idt/ISR/isr.c -o ./build/idt/ISR/isr.o

./build/idt/ISR/isr.asm.o: ./src/idt/ISR/isr.asm
	nasm -f elf -g ./src/idt/ISR/isr.asm -o ./build/idt/ISR/isr.asm.o

./build/idt/PIC/pic.o: ./src/idt/PIC/pic.c
	i686-elf-gcc $(INCLUDES) -I./src/idt/PIC $(FLAGS) -std=gnu99 -c ./src/idt/PIC/pic.c -o ./build/idt/PIC/pic.o

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/paging/ $(FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./src/memory/heap $(FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

clean:
	rm -rf ./bin/boot.bin
	rm -rf ./bin/kernel.bin
	rm -rf ./bin/os.bin
	rm -rf ${FILES}
	rm -rf ./build/kernelfull.o
