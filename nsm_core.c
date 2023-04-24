#include "nsm_core.h"

// Define a static function named "nsm_init" that returns an integer value
static int __init nsm_init(void)
{
    // create all resources of this device
    nsm_build_device();

    // allocate memory
    __memory_init();
    pr_notice("memory size: %ld", memory->size);

    
    // start server code

    pr_notice("NSM Fully Loaded\n");
    return 0;
}

// Define a static function named "nsm_exit" that doesn't return anything
static void __exit nsm_exit(void)
{
    // destroy server

    // destroy memory
    __memory_destroy();

    pr_notice("memory size: %ld", memory->size);

    // destroy all resources of this device
    nsm_destroy_device();

    pr_notice("NSM Fully Unloaded\n");
}

module_init(nsm_init);
module_exit(nsm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMIRMNOOHI");
MODULE_DESCRIPTION("NSM: Transparent Distributed Shared Memory");
