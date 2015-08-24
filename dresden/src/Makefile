#
# Variables needed to build the kernel module
#
name      = dresden
src_files = dresden_core.c module_notifier_event.c proc_entry.c

obj-m += $(name).o
$(name)-objs := $(src_files:.c=.o)

all: build

.PHONY: build install clean

clean: clean 

build: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CONFIG_DEBUG_SECTION_MISMATCH=y

install: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install CONFIG_DEBUG_SECTION_MISMATCH=y

clean:
	[ -d /lib/modules/$(shell uname -r)/build ] && \
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

