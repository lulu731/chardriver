obj-m += cdriver.o
    
default:
	make -C ../linux M=$(PWD) modules
    
clean:
	make -C ../linux M=`pwd` clean