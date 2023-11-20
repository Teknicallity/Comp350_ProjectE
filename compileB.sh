#!/bin/bash

# creates diskc.img
dd if=/dev/zero of=diskc.img bs=512 count=1000

#copies bootload to first sector of diskc.img
dd if=bootload of=diskc.img bs=512 count=1 conv=notrunc

#assembles kernel.asm
as86 kernel.asm -o kernel_asm.o
#assemle userlib.asm
as86 userlib.asm -o userlib_asm.o

#assembles kernel.c
bcc -ansi -c -o kernel_c.o kernel.c
bcc -ansi -c -o shell_c.o shell.c

#compile load (originally gcc)
#gcc -o loadFile loadFile.c

#link the kernels
ld86 -o kernel -d kernel_c.o kernel_asm.o
ld86 -o shell -d shell_c.o userlib.o

#copy machin code kernel ont diskc.img sector 3
#commented out in projCs1
#dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3
./loadFile kernel
./loadFile shell

#loads message into img
./loadFile message.txt

#copy message.txt in secotr 30
#dd if=message.txt of=diskc.img bs=512 count=1 seek=30 conv=notrunc

bcc -ansi -c -o tstpr2.o tstpr2.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr2 tstpr2.o userlib.o
./loadFile tstpr2
