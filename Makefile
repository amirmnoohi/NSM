obj-m += nsm.o 

# Define variables for each object file
nsm-y := nsm_core.o nsm_dev.o nsm_mem.o nsm_nmsg.o

# Specify the paths to the kernel headers
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)


# Define the build target
default: header
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o user user-space/user.c

define HEADER

███╗   ██╗███████╗███╗   ███╗
████╗  ██║██╔════╝████╗ ████║
██╔██╗ ██║███████╗██╔████╔██║
██║╚██╗██║╚════██║██║╚██╔╝██║
██║ ╚████║███████║██║ ╚═╝ ██║
╚═╝  ╚═══╝╚══════╝╚═╝     ╚═╝
Copyright © 2023 by AMIRMNOOHI  


endef
export HEADER

header:
	clear
	@echo "$$HEADER"

# Define the clean target
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f user
