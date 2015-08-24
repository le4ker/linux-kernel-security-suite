/*
* Reveal hidden kernel modules by brute forcing the Virtual Memory
* Copyright (C) 2013  Panos Sakkos
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/jiffies.h>
#include <linux/stddef.h>
#include "module-utils.h"
#include "vm-utils.h"

int looks_like_module(struct module *module)
{
	/* TODO Implement smart euristic in order to recognize a module in the memory.
	 * By just checking the validity of the name of the module it can by bypassed by
	 * erasing the attacking's module name.
	 */

	/* Check if name member of the candidate module is mapped */

	if(mapped_address((unsigned long) module + offsetof(struct module, name)))
	{
		if(is_mapped_string(module->name, MODULE_NAME_LEN))
		{
			return 1;
		}
	}

	return 0;
}

int modules_are_the_same(struct module *module1, struct module *module2)
{
	return (memcmp( (void *) module1, (void *) module2, sizeof(struct module)) == 0);
}

int exists_in_the_modules_list(struct module *target_module)
{
	struct module *module;
	struct list_head *head;

	list_for_each(head, &THIS_MODULE->list)
	{
		module = list_entry(head, struct module, list);
		
		if(modules_are_the_same(target_module, module))
		{
			return 1;
		}
	}

	return 0;
}

