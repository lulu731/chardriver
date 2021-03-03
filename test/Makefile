objects = test_ioctl.o

default: $(objects)
	gcc -o test_ioctl $(objects)

$(objects): ../cdriver.h

.PHONY: clean
clean:
	-rm test_ioctl $(objects)