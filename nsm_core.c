#include "nsm_core.h"

char * src_address = "255.255.255.255";
char * dst_address = "255.255.255.255";
int port = 12345;
 
// Define a static function named "nsm_init" that returns an integer value
static int __init nsm_init(void)
{
    // create all resources of this device
    nsm_build_device();

    // allocate memory
    memory_init();
    pr_notice("memory size: %ld", memory->size);

    printk(KERN_INFO "src_address = %s\n", src_address);
    printk(KERN_INFO "dst_address = %s\n", dst_address);
    printk(KERN_INFO "port_number = %d\n", port);

    // start server code
    nmsg_start_server(src_address, port);

    pr_notice("NSM Fully Loaded\n");
    return 0;
}

// Define a static function named "nsm_exit" that doesn't return anything
static void __exit nsm_exit(void)
{
    // destroy server
    nmsg_stop_server();

    // destroy memory
    memory_destroy();

    pr_notice("memory size: %ld", memory->size);

    // destroy all resources of this device
    nsm_destroy_device();

    pr_notice("NSM Fully Unloaded\n");
}

module_init(nsm_init);
module_exit(nsm_exit);



module_param(src_address, charp, S_IRUGO);
module_param(dst_address, charp, S_IRUGO);
module_param(port, int, S_IRUGO);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMIRMNOOHI");
MODULE_DESCRIPTION("NSM: Transparent Distributed Shared Memory");
