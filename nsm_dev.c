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
    pr_info("Open Called\n");
    return SUCCESS;
}

/* Called when a process closes the device file. */
int device_release(struct inode *inode, struct file *file)
{
    pr_info("Release Called\n");
    return SUCCESS;
}

/* Called when a process, which already opened the dev file, attempts to read from it.*/
ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    pr_info("Read Called\n");
    return SUCCESS;
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello */
ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
    pr_info("Write Called\n");
    return SUCCESS;
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
