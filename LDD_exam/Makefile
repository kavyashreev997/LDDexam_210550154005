obj-m += char.o

KDIR=/lib/modules/$(shell uname  -r)/build/
PWD:=$(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
	gcc read1.c -o read1
	gcc write1.c -o write1
	gcc read2.c -o read2
	gcc write2.c -o write2

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm read1 write1 read2 write2 

