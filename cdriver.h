#ifndef CDRIVER_H
#define CDRIVER_H

#include <linux/cdev.h>
#include <linux/mutex.h>

#ifndef CDRIVER_MAJOR
#define CDRIVER_MAJOR 0
#endif

typedef struct cdriver
{
    dev_t dev; //device numbers
    struct mutex mut;    
    struct cdev cdev;
    char *buffer;
    int data_len;
} cdriver;

//IOC implementation
#define CDRIVER_IOC_MAGIC 'k'

/*
 * S = "SET" through pointer
 * Q = "Query" : response on return value
 */
#define CDRIVER_IOCSBUFF_SIZE _IOW(CDRIVER_IOC_MAGIC, 1, int)
#define CDRIVER_IOCQBUFF_SIZE _IOR(CDRIVER_IOC_MAGIC, 2, int)

#endif