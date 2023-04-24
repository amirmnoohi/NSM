#ifndef __NSM_DEV_H
#define __NSM_DEV_H

#include <linux/fs.h>
#include <linux/device.h>

#include "nsm_nmsg.h"

#define SUCCESS 0

/* Dev name as it appears in /proc/devices   */
#define DEVICE_NAME "NSM"

int device_open(struct inode *, struct file *); 
int device_release(struct inode *, struct file *); 
ssize_t device_read(struct file *, char __user *, size_t, loff_t *); 
ssize_t device_write(struct file *, const char __user *, size_t, loff_t *); 

/* Prototypes for device helper functions */
void nsm_build_device(void);
void nsm_destroy_device(void);

#endif