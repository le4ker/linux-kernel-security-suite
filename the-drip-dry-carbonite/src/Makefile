#
# Variables needed to build the kernel module
#
name      = the-drip-dry-carbonite
src_files = carbonite.c command-chrdev.c super-fast-hash.c sys-call-table-integrity-check.c proc-entry.c task-dump.c task-dump-list.c task-dump-seq-file.c task-utils.c my-kallsyms.c logger.c

obj-m += $(name).o
$(name)-objs := $(src_files:.c=.o)

all: clean build

.PHONY: build install clean

build: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules CONFIG_DEBUG_SECTION_MISMATCH=y

install: build
	-mkdir -p /lib/modules/`uname -r`/kernel/arch/x86/kernel/
	cp $(name).ko /lib/modules/`uname -r`/kernel/arch/x86/kernel/
	depmod /lib/modules/`uname -r`/kernel/arch/x86/kernel/$(name).ko

clean:
	[ -d /lib/modules/$(shell uname -r)/build ] && \
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

