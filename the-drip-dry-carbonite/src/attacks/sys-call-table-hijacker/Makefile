obj-m := hijacker.o
hijacker-objs := sys-call-table-hijacker.o my-kallsyms.o

all:	clean
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	$(MAKE) -C /lib/modules/`uname -r`/build M=`pwd` clean
	$(RM) Module.markers modules.order
