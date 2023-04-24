obj-m += nsm.o 

# Define variables for each object file
nsm-y := nsm_core.o nsm_dev.o

# Specify the paths to the kernel headers
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


# Define the build target
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o user user-space/user.c

# Define the clean target
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f user
