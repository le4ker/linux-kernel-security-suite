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
 
#include "command-chrdev.h"
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include "proc-entry.h"
#include "carbonite.h"

static int command_major;
static char commanding_module[MODULE_NAME_LEN];
static char chrdev_name[MODULE_NAME_LEN + 8]; //+ 8 chars for the "command-" prefix
static char proc_file_name[MODULE_NAME_LEN + 8 + 6]; //+6 for the "-major" ending


static void (*command) (int);

long command_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	switch(ioctl_num)
	{
		case IOCTL_CARBONITE_COMMAND:
			
			if(!capable(CAP_SYS_ADMIN))
			{
				/* Bugger off! */
				
				return -EPERM;
			}
			
			if(command)
			{
				command(ON_DEMAND);
			}
			
			break;
		default:
			printk(KERN_ERR "%s:\t[-] Invalid ioctl command\n", commanding_module);
			return -ENOTTY;
	}
	
	return 0;
}

static const struct file_operations command_fops = {
	.owner   	= THIS_MODULE,
	.unlocked_ioctl	= command_ioctl,
};

int init_command_chrdev(const char *module_name, void (*function) (int))
{
	int ret;
	char major_number_string[10];

	if(module_name == NULL)
	{
		printk(KERN_ERR "init_char_dev:\t[-] NULL module name passed as argument\n");

		return 0;
	}

	strncpy(commanding_module, (char *) module_name, MODULE_NAME_LEN);
	
	if(function == NULL)
	{
		printk(KERN_ERR "%s:\t[-] NULL function pointer passed as argument\n", commanding_module);

		return 0;
	}
	
	command = function;
	strncpy(chrdev_name, "command-", 9);
	strncat(chrdev_name, commanding_module, MODULE_NAME_LEN);

	command_major = register_chrdev(0, chrdev_name, &command_fops);

	if(command_major < 0)
	{
		printk(KERN_ERR "%s:\t[-] Failed to initialize file operations with major number %d\n", 
									commanding_module, command_major);

		return 0;	
	}

	/* Create a proc entry to expose the device's major number */

	sprintf(proc_file_name, "%s%s", chrdev_name, "-major");
	sprintf(major_number_string, "%d", command_major);

	ret = init_proc_entry(proc_file_name, major_number_string);

	if(!ret)
	{
		printk(KERN_ERR "%s:\t[-] Failed to create proc fs entry\n", commanding_module);
		destroy_command_chrdev();

		return 0;
	}

	printk(KERN_INFO "%s:\t[+] Registered successfully character device %s and major number %d\n", 
									commanding_module, chrdev_name, command_major);
	printk(KERN_INFO "%s:\t[+] Create the file node by executing `mknod /dev/%s c %d 0`\n", 
									commanding_module, chrdev_name, command_major);
	
	return 1;
}

void destroy_command_chrdev(void)
{
	destroy_proc_entry();
	unregister_chrdev(command_major, chrdev_name);
	
	command = NULL;
	command_major = commanding_module[0] = chrdev_name[0] = 0;
	
}
