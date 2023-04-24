#include "nsm_core.h"

// Define a static function named "nsm_init" that returns an integer value
static int __init nsm_init(void)
{
    pr_err("NSM Loaded\n");

    // create all resources of this device
    nsm_build_device();

    // allocate memory
    

    // start server code

    return 0;
}

// Define a static function named "nsm_exit" that doesn't return anything
static void __exit nsm_exit(void)
{
    // destroy server

    // destroy memory

    // destroy all resources of this device
    nsm_destroy_device();
    pr_err("NSM Unloaded\n");
}

module_init(nsm_init);
module_exit(nsm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMIRMNOOHI");
MODULE_DESCRIPTION("NSM: Transparent Distributed Shared Memory");
