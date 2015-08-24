/*
 *	Security extensions for the linux kernel, as described in "Special Focus Issue: Security" article,
 *	by Rik Farrow at the MAGAZINE OF USENIX & SAGE. The current project merges the 2 suggestions in
 *	a kernel module that is compatible with the lastest versions of the linux kernel and incorporates
 *	some modern security mechanisms. 
 *	Project for the master course "Advanced Operating Systems", instructed by Mema Rousopoulos <http://cgi.di.uoa.gr/~mema/>
 *
 *	Copyright (C) 2013  Panos Sakkos
 *	
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/notifier.h>
#include "my-kallsyms.h"
#include "sys-call-table-integrity-check.h"
#include "super-fast-hash.h"
#include "logger.h"
#include "carbonite.h"

static struct timer_list timer;
static unsigned long timer_interval;
static char checking_module[MODULE_NAME_LEN];

static uint32_t sys_call_table_hash;
static unsigned int sys_call_table_size;
static unsigned long sys_call_table_address;

static void (*incident_handler) (int);

/* Forward declaration for the register_timer_interrupt function */

void sys_call_table_integrity_checker(unsigned long);

void register_timer_interrupt(void)
{
	int ret;
	
	if(timer_interval <= 0)
	{
		return;
	}
		
	setup_timer(&timer, sys_call_table_integrity_checker, 0);
	
	ret = mod_timer(&timer, jiffies + msecs_to_jiffies(timer_interval));		

	if(ret)
	{
		printk(KERN_ERR "%s:\t[-] Failed to modify timer interval of the timer\n", checking_module);
	}
}

int the_drip_dry_carbonite_module_event(struct notifier_block *this, unsigned long event, void *pointer) 
{
	unsigned long flags;
	struct module *new_module;
	DEFINE_SPINLOCK(module_event_spinlock);

	spin_lock_irqsave(&module_event_spinlock, flags);

	new_module = (struct module *) pointer;	
	
	switch(new_module->state)
	{
		case(MODULE_STATE_COMING):
			break;
		case(MODULE_STATE_LIVE):

			if(new_module == THIS_MODULE)
			{
				/* Skip checking for ourselves */

				break;
			}                        

			incident_handler(AFTER_MODULE_INIT);

                        break;
		case(MODULE_STATE_GOING):
			break;
		default:
			print_log(KERN_ERR MODULE_NAME "%s:\t[-] Unknown module state in notifier chain at module with name: %s\n",
												checking_module, new_module->name);
			break;						
	}

	spin_unlock_irqrestore(&module_event_spinlock, flags);
	
	return NOTIFY_DONE;
}

static struct notifier_block module_notifier_block = 
{
	.notifier_call = the_drip_dry_carbonite_module_event,
	.priority      = INT_MAX
};

void sys_call_table_integrity_checker(unsigned long data)
{
	unsigned long flags;
	int sys_call_table_modified = 0;
	DEFINE_SPINLOCK(integrity_spinlock);

	/* Run non premptive and unscheduled */
	
	spin_lock_irqsave(&integrity_spinlock, flags);

	if(sys_call_table_hash != super_fast_hash((char *) sys_call_table_address, sys_call_table_size))
	{
		sys_call_table_modified = 1;
	}

	spin_unlock_irqrestore(&integrity_spinlock, flags);

	if(sys_call_table_modified)
	{
		printk(KERN_CRIT "%s:\t[-] system call table was modified!\n", checking_module);

		if(incident_handler)
		{
			incident_handler(AFTER_INCIDENT);
			incident_handler = NULL;
		}
	}	
	else
	{
		printk(KERN_INFO "%s:\t[+] system call table is not modified\n", checking_module);
	}

	register_timer_interrupt();
}

int init_sys_call_table_integrity_checker(const char *module_name, unsigned long every, void (*handler) (int))
{
	unsigned long next_symbol;

	if(module_name == NULL)
	{
		printk(KERN_ERR "init_sys_call_table_integrity_checker:\t[-] NULL module name passed as argument\n");

		return 0;
	}

	if(every <= 0)
	{
		printk(KERN_ERR "%s:\t[-] Negative time interval was given to initialization of sys_call_table_integrity_checker\n", module_name);

		return 0;
	}

	timer_interval = every;
	register_timer_interrupt();

	register_module_notifier(&module_notifier_block);

	sys_call_table_address = my_kallsyms_lookup_name("sys_call_table");

	if(sys_call_table_address == 0)
	{
		printk(KERN_ERR "%s:\t[-] Failed to get address of sys_call_table symbol\n", module_name);

		return 0;
	}	
	
	next_symbol = find_next_symbol(sys_call_table_address);

	if(next_symbol == 0)
	{
		printk(KERN_ERR "%s:\t[-] Failed to get address of sys_call_table's next symbol\n", module_name);

		return 0;
	}
	
	/* The distance between the addresses of two exported symbols, it the size of the first */
	
	sys_call_table_size = next_symbol - sys_call_table_address;
	sys_call_table_hash = super_fast_hash((char *) sys_call_table_address, sys_call_table_size);

	strncpy(checking_module, module_name, MODULE_NAME_LEN);

	incident_handler = handler;
	
	return 1;
}

void destroy_sys_call_table_integrity_checker(void)
{
	int ret;
	
	unregister_module_notifier(&module_notifier_block);
	
	ret = del_timer(&timer);

	if(!ret)
	{
		printk("%s:\t[-] Failed to delete timer\n", checking_module);
	}
	
	timer_interval = sys_call_table_address = sys_call_table_size = sys_call_table_hash = checking_module[0] = 0;
}

