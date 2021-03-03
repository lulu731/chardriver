#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "../cdriver_ioctl.h"

int main(int argc, char** argv) {
    int buffer_size=10;
    int retval = 0;
    int fd=open(argv[1], O_RDWR|O_NONBLOCK);
    if (fd+1)
    {
        int a = atoi(argv[2]);
        retval=ioctl(fd, CDRIVER_IOCSBUFF_SIZE, &a);
        //printf("retval = %d\n", retval);
        buffer_size=ioctl(fd, CDRIVER_IOCQBUFF_SIZE, 0);
        
        //printf("retval = %d\n", retval);
    }
    else
    {
        return -1;
    }
    printf("%d\n", buffer_size);
    return retval;
}