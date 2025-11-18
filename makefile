all:
	nasm -f bin ./boot.asm -o ./boot.bin
	dd if=./messagg.txt >> ./boot.bin