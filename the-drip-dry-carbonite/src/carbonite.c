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
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include "carbonite.h"
#include "command-chrdev.h"
#include "sys-call-table-integrity-check.h"
#include "task-dump.h"
#include "logger.h"
#include "carbonite.h"

unsigned int sys_call_table_check_interval = 1000;
module_param(sys_call_table_check_interval, uint, 0000);
MODULE_PARM_DESC(sys_call_table_check_interval, "The interval (in milliseconds) between the checks of the system call table's integrity.\n" 
													"\t\tDefault value is 1000 (1 second)");

unsigned int sys_call_table_monitoring = 1;
module_param(sys_call_table_monitoring, int, 0000);
MODULE_PARM_DESC(sys_call_table_monitoring, "Variable to designate wether to monitor or not the integrity of the system call table.\n"
									" \t\tDefault is to monitor (1), set to 0 to skip integrity check");

char *remote_log_ip = NULL;
module_param(remote_log_ip, charp, 0000);
MODULE_PARM_DESC(remote_log_ip, "IPv4 address for remote syslog server. If not set, remote logging will be disabled.\n");

unsigned int stealth = 1;
module_param(stealth, int, 0000);
MODULE_PARM_DESC(stealth, "Switch \"Stealth mode\" off, by setting to 0.\n");

int previous_reason = NONE;

void print_dispatch_reason(int why)
{
	switch(why)
	{
		case AFTER_INCIDENT:

			print_log(KERN_INFO MODULE_NAME ":\t[+] Triggering carbonite after detected incident\n");

			break;
		case AFTER_MODULE_INIT:

			print_log(KERN_INFO MODULE_NAME ":\t[+] Triggering carbonite after module initialization\n");

			break;
		case ON_DEMAND:

			print_log(KERN_INFO MODULE_NAME ":\t[+] Triggering carbonite from trigger-carbonite\n");
		
			break;
		default:
			print_log(KERN_ERR MODULE_NAME ":\t[-] Unknown reason of carbonite dispatch\n");
	}
}

void carbonite(int why)
{
	int ret;
	unsigned long flags;
	struct task_struct *task;
	DEFINE_SPINLOCK(carbonite_spinlock);

	if(previous_reason == AFTER_MODULE_INIT && why == AFTER_INCIDENT)
	{
		/* Don't trigger carbonite twice for an attacking module,
		 * because the snapshot of the processes will not be accurate.
		 */
		 
		 previous_reason = why;

		 return;
	}

	print_dispatch_reason(why);

	/* Stop the world */
	
	spin_lock_irqsave(&carbonite_spinlock, flags);
	
	for_each_process(task)
	{
		ret = dump_task(task);
		
		if(!ret)
		{
			print_log(KERN_ERR MODULE_NAME ":\t[-] Failed to dump %s task with PID %d\n", task->comm, task->pid);
		}
	}

	spin_unlock_irqrestore(&carbonite_spinlock, flags);
	
	previous_reason = why;
}
EXPORT_SYMBOL(carbonite);

int __init carbonite_init(void)
{
	int ret;

	print_log(KERN_INFO MODULE_NAME ": Security extension for the linux kernel by Panos Sakkos <panos.sakkos@gmail.com>\n");
	print_log(KERN_INFO MODULE_NAME ":\t[+] Initializing\n");

	ret = init_command_chrdev(MODULE_NAME, &carbonite);

	if(ret)
	{
		print_log(KERN_INFO MODULE_NAME ":\t[+] Initiliazed command character device\n");
	}
	else
	{
		print_log(KERN_ERR MODULE_NAME ":\t[-] Failed to initialize command character device\n");

		return -ECHARDEV;
	}

	ret = init_task_dump();

	if(ret)
	{
		print_log(KERN_INFO MODULE_NAME ":\t[+] Initiliazed task_dump component\n");
	}
	else
	{
		print_log(KERN_ERR MODULE_NAME ":\t[-] Failed to initialize task_dump component\n");

		return -ETASKDUMP;
	}
	
	if(!sys_call_table_monitoring)
	{
		print_log(KERN_INFO MODULE_NAME ":\t[-] Skipping monitoring of the system call table\n");
		
		return 0;
	}
	
	ret = init_sys_call_table_integrity_checker(MODULE_NAME, sys_call_table_check_interval, &carbonite);
	
	if(ret)
	{
		print_log(KERN_INFO MODULE_NAME ":\t[+] Initialized timer for checking the integrity of the system call table (check interval: %u)\n", 
															sys_call_table_check_interval);
	}
	else
	{
		print_log(KERN_ERR MODULE_NAME ":\t[-] Failed to initialize system call table checker\n");

		return -ESYSTABLE;
	}

	ret = init_logger(remote_log_ip);

	if(ret)
	{
		print_log(KERN_INFO MODULE_NAME ":\t[+] Initiliazed logger\n");
	}
	else
	{
		print_log(KERN_ERR MODULE_NAME ":\t[-] Failed to initialize logger\n");

		return -ELOGGER;
	}

	if(stealth)
	{
		/* Delete ourselves from the kernel's modules list
		 * and free our destruction code, in order to avoid 
		 * to be  dispatcehd  by an attacker that is aware of us.
		 */

		list_del(&THIS_MODULE->list);
		THIS_MODULE->exit = NULL;
		
		print_log(KERN_INFO MODULE_NAME ":\t[+] Stealth mode is enabled\n");
	}
	else
	{
		print_log(KERN_INFO MODULE_NAME ":\t[-] Stealth mode is not enabled\n");
	}
	
	return 0;
}

void __exit carbonite_exit(void)
{
	print_log(KERN_INFO MODULE_NAME ":\t[+] Exiting\n");

	destroy_command_chrdev();
	
	if(sys_call_table_monitoring)
	{
		destroy_sys_call_table_integrity_checker();
	}
	
	destroy_task_dump();
	
	destroy_logger();
}

module_init(carbonite_init);
module_exit(carbonite_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Panos Sakkos <panos.sakkos@gmail.com>");
MODULE_DESCRIPTION("Security extensions for the linux kernel, as described in \"Special Focus Issue: Security\"\n"
			"\t\tarticle, by Rik Farrow at the MAGAZINE OF USENIX & SAGE. The current project will merge\n"
			"\t\tthe 2 suggestions in a kernel module that is compatible with the lastest versions of the\n"
			"\t\tlinux kernel and will also include some modern security mechanisms. Project for the master course\n"
			"\t\t\"Advanced Operating Systems\", instructed by Mema Rousopoulos <http://cgi.di.uoa.gr/~mema/>\n");
