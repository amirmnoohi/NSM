#include "nsm_dev.h"

/* File operations structure. Defined in linux/fs.h */
static struct file_operations nsm_fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

/* major number assigned to our device driver */
static int major;

/* Device Section */
static struct class *cls;

/* Called when a process tries to open the device file, like
 * "sudo cat /dev/chardev"
 */
int device_open(struct inode *inode, struct file *file)
{
    pr_notice("Message status: %d" ,nmsg_send_message("lock"));
    pr_info("Open Called\n");
    return SUCCESS;
}

/* Called when a process closes the device file. */
int device_release(struct inode *inode, struct file *file)
{
    pr_notice("Message status: %d" ,nmsg_send_message("unlock"));
    pr_info("Release Called\n");
    return SUCCESS;
}

/* Called when a process, which already opened the dev file, attempts to read from it.*/
ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    /* Number of bytes actually written to the buffer */ 
    int bytes_read = 0; 
    const char *msg_ptr;

    // wait until memory->pages[0].state become __shared
    while(memory->pages[0].state != __SHARED);

    msg_ptr = page_address(memory->pages[0].__page);
    
    pr_info("Read Called\n");

    if (!*(msg_ptr + *offset)) { /* we are at the end of message */ 
        *offset = 0; /* reset the offset */ 
        
        pr_info("Device Read End of Message\n");

        return 0; /* signify end of file */ 
    } 
 
    msg_ptr += *offset; 
 
    /* Actually put the data into the buffer */ 
    while (length && *msg_ptr) { 
        /* The buffer is in the user data segment, not the kernel 
         * segment so "*" assignment won't work.  We have to use 
         * put_user which copies data from the kernel data segment to 
         * the user data segment. 
         */ 
        put_user(*(msg_ptr++), buffer++); 
        length--; 
        bytes_read++; 
    } 
    
    /* Add a null byte to the end of the message data */
    if (*offset + bytes_read >= BUF_LEN) {
        if (put_user('\0', buffer)) {
            return -EFAULT; /* return error code on buffer overflow */
        }
        bytes_read++;
    }
 
    *offset += bytes_read; 

    return SUCCESS;
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello */
ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
    char *kernel_buff = NULL;
    int ret;

    pr_info("Write Called\n");

    if (len == 0) {
        /* Nothing to do */
        ret = 0;
        goto out;
    }

    /* Allocate a kernel buffer to hold the data from user space */
    kernel_buff = kmalloc(len + 1, GFP_KERNEL);
    if (!kernel_buff) {
        ret = -ENOMEM;
        goto out;
    }

    /* Copy data from user space to kernel buffer */
    if (copy_from_user(kernel_buff, buff, len)) {
        ret = -EFAULT;
        goto out;
    }

    /* Null-terminate the kernel buffer */
    kernel_buff[len] = '\0';

    // write to memory
    strcpy(page_address(memory->pages[0].__page), kernel_buff);

    nmsg_send_message(kernel_buff);

    /* Do something with the data in kernel buffer */
    printk(KERN_INFO "Received data from user space: %s\n", kernel_buff);

    /* Return the number of bytes that were written */
    ret = len;

out:
    /* Free the kernel buffer, if it was allocated */
    if (kernel_buff) {
        kfree(kernel_buff);
    }
    return ret;
}

void nsm_build_device(void)
{
    // Register the character device driver with the operating system and receive a major device number
    major = register_chrdev(0, DEVICE_NAME, &nsm_fops);

    // Check if the registration was successful
    if (major < 0)
    {
        // Print an error message to the system log if the registration failed
        pr_alert("Registering char device failed with %d\n", major);
    }

    // Create a class in the file system with the name of the device
    cls = class_create(THIS_MODULE, DEVICE_NAME);

    // Create a device node in the file system with the major and minor device numbers
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Device Created at /dev/%s", DEVICE_NAME);
}


void nsm_destroy_device(void)
{
    // Remove the device node from the file system
    device_destroy(cls, MKDEV(major, 0));

    // Remove the class from the file system
    class_destroy(cls);

    // Unregister the character device driver from the operating system
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("Device Destroyed");
}
