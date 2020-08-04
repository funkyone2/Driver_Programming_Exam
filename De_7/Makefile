KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd`
clean:
	make -C $(KDIR) M=`pwd` clean
build:
	sudo insmod KiemTra.ko
rebuild:
	sudo rmmod KiemTra
	make clean
	make all
	make build
testfile:
	gcc TestFile.c -o TestFile
clean-test:
	rm ./TestFile
