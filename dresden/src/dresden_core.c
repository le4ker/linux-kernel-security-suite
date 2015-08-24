#include <linux/kernel.h>
#include <linux/module.h>
#include "module_notifier_event.h"
#include "proc_entry.h"

#define MODULE_NAME "dresden"
#define DRESDEN_OK 0

int show_proc_entry = 0;
module_param(show_proc_entry, int, 0000);
MODULE_PARM_DESC(show_proc_entry, "Set to non zero value if you want to create the /proc/dresden entry in order to know if dresden is loaded. Default is to 0");

static int __init dresden_engage(void)
{
	register_module_notifier_event_handler();
	
	/* Hide ourselves */
	
	list_del(&THIS_MODULE->list);	

	printk(KERN_INFO MODULE_NAME ": Kernel module insertion blocker and action notifier by CERN Security Team\n");
	printk(KERN_INFO MODULE_NAME ":\t[+] Future loading of kernel modules will be prevented\n");
	printk(KERN_INFO MODULE_NAME ":\t[+] Emergency messages will be logged in case of trying to load or unload a module\n");
	printk(KERN_INFO MODULE_NAME ":\t[+] You are not able to remove this module\n");
	
	if(show_proc_entry)
	{
		int err;
		
		err = init_proc_entry();

		if(err == PROC_CREATED)
		{
			printk(KERN_INFO MODULE_NAME ":\t[+] Created /proc/dresden entry\n");
		}
		else
		{
			printk(KERN_ERR MODULE_NAME ":\t[-] Failed to create /proc/dresden entry\n");		
		}
	}

	return DRESDEN_OK;
}

module_init(dresden_engage);

MODULE_AUTHOR("Panos Sakkos <panos.sakkos@cern.ch>");
MODULE_DESCRIPTION("Block incoming modules and log emergency alerts in case of trying to insert\n"
						"\t\t a new module or removing an existing one");
MODULE_LICENSE("GPL");
