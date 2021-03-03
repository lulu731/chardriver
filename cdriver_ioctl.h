#ifndef CDRIVER_IOCTL_H
#define CDRIVER_IOCTL_H

//IOC implementation
#define CDRIVER_IOC_MAGIC 'k'

/*
 * S = "SET" through pointer
 * Q = "Query" : response on return value
 */
#define CDRIVER_IOCSBUFF_SIZE _IOW(CDRIVER_IOC_MAGIC, 1, int)
#define CDRIVER_IOCQBUFF_SIZE _IOR(CDRIVER_IOC_MAGIC, 2, int)

#endif