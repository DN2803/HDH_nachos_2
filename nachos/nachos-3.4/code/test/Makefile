# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# if you are cross-compiling, you need to point to the right executables
# and change the flags to ld and the build procedure for as

GCCDIR = ../../../gnu-decstation-ultrix/decstation-ultrix/2.95.3/

LDFLAGS = -T script -N
ASFLAGS = -mips2
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)

PATH = $(GCCDIR):/lib:/usr/bin:/bin

CC = $(GCCDIR)gcc -B../../../gnu-decstation-ultrix/
AS = $(GCCDIR)as
LD = $(GCCDIR)ld

CPP = gcc -E
INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt shell matmult sort ping pong scheduler testingfile main sinhvien voinuoc

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.c > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

shell.o: shell.c
	$(CC) $(CFLAGS) -c shell.c
shell: shell.o start.o
	$(LD) $(LDFLAGS) start.o shell.o -o shell.coff
	../bin/coff2noff shell.coff shell

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

testingSC_int.o: testingSC_int.c
	$(CC) $(CFLAGS) -c testingSC_int.c
testingSC_int: testingSC_int.o start.o
	$(LD) $(LDFLAGS) start.o testingSC_int.o -o testingSC_int.coff
	../bin/coff2noff testingSC_int.coff testingSC_int

testingSC_char.o: testingSC_char.c
	$(CC) $(CFLAGS) -c testingSC_char.c
testingSC_char: testingSC_char.o start.o
	$(LD) $(LDFLAGS) start.o testingSC_char.o -o testingSC_char.coff
	../bin/coff2noff testingSC_char.coff testingSC_char

testingSC_string.o: testingSC_string.c
	$(CC) $(CFLAGS) -c testingSC_string.c
testingSC_string: testingSC_string.o start.o
	$(LD) $(LDFLAGS) start.o testingSC_string.o -o testingSC_string.coff
	../bin/coff2noff testingSC_string.coff testingSC_string

help.o: help.c
	$(CC) $(CFLAGS) -c help.c
help: help.o help.o
	$(LD) $(LDFLAGS) start.o help.o -o help.coff
	../bin/coff2noff help.coff help

ascii.o: ascii.c
	$(CC) $(CFLAGS) -c ascii.c
ascii: ascii.o start.o
	$(LD) $(LDFLAGS) start.o ascii.o -o ascii.coff
	../bin/coff2noff ascii.coff ascii

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

createfile.o: createfile.c
	$(CC) $(CFLAGS) -c createfile.c
createfile: createfile.o start.o
	$(LD) $(LDFLAGS) start.o createfile.o -o createfile.coff
	../bin/coff2noff createfile.coff createfile

testingfile.o: testingfile.c
	$(CC) $(CFLAGS) -c testingfile.c
testingfile: testingfile.o start.o
	$(LD) $(LDFLAGS) start.o testingfile.o -o testingfile.coff
	../bin/coff2noff testingfile.coff testingfile

ping.o: ping.c
	$(CC) $(CFLAGS) -c ping.c
ping: ping.o start.o
	$(LD) $(LDFLAGS) start.o ping.o -o ping.coff
	../bin/coff2noff ping.coff ping

pong.o: pong.c
	$(CC) $(CFLAGS) -c pong.c
pong: pong.o start.o
	$(LD) $(LDFLAGS) start.o pong.o -o pong.coff
	../bin/coff2noff pong.coff pong

scheduler.o: scheduler.c
	$(CC) $(CFLAGS) -c scheduler.c
scheduler: scheduler.o start.o
	$(LD) $(LDFLAGS) start.o scheduler.o -o scheduler.coff
	../bin/coff2noff scheduler.coff scheduler


#main
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
main: main.o start.o
	$(LD) $(LDFLAGS) start.o main.o -o main.coff
	../bin/coff2noff main.coff main

#sinhvien
sinhvien.o: sinhvien.c
	$(CC) $(CFLAGS) -c sinhvien.c
sinhvien: sinhvien.o start.o
	$(LD) $(LDFLAGS) start.o sinhvien.o -o sinhvien.coff
	../bin/coff2noff sinhvien.coff sinhvien

#voinuoc
voinuoc.o: voinuoc.c
	$(CC) $(CFLAGS) -c voinuoc.c
voinuoc: voinuoc.o start.o
	$(LD) $(LDFLAGS) start.o voinuoc.o -o voinuoc.coff
	../bin/coff2noff voinuoc.coff voinuoc


testingfile.o: testingfile.c
	$(CC) $(CFLAGS) -c testingfile.c
testingfile: testingfile.o start.o
	$(LD) $(LDFLAGS) start.o testingfile.o -o testingfile.coff
	../bin/coff2noff testingfile.coff testingfile
