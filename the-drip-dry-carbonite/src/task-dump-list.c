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

#include "task-dump-list.h"
#include "task-utils.h"
#include <linux/slab.h>
#include <linux/cred.h>

struct task_dump_node {
	struct task_dump *task_dump;
	struct task_dump_node *next;
};

struct task_dump_list {
	long size;
	struct task_dump_node *head;
	struct task_dump_node *tail;
};

struct task_dump *init_task_dump_from_task(struct task_struct *task)
{
	struct task_dump *dump = NULL;
	
	if(!task)
	{
		return dump;
	}
	
	dump = kmalloc(sizeof(struct task_dump), GFP_ATOMIC);
	
	if(!dump)
	{
		return dump;
	}

	dump->pid = task->pid;
	
	/* Caller has "stopped the world", so there's no need for locking the cred structure */
	
	dump->uid = task_uid(task);
	dump->gid = __task_cred(task)->gid;
	get_task_state(task, dump->state);
	//TODO arguments
	//TODO enviroment
	//TODO executables
	strncpy(dump->comm, task->comm, TASK_COMM_LEN);
	dump->start_time = timespec_to_ns((const struct timespec *) &(task->start_time));	

	return dump;
}

struct task_dump_list *init_task_dump_list(void)
{
	struct task_dump_list *list;

	list = kmalloc(sizeof(struct task_dump_list), GFP_ATOMIC);
	
	if(!list)
	{
		return list;
	}
	
	list->size = 0;
	list-> head = list->tail = NULL;

	return list;
}

struct task_dump_node *init_task_dump_node(struct task_dump *task_dump)
{
	struct task_dump_node *node = kmalloc(sizeof(struct task_dump_node), GFP_ATOMIC);
	
	if(!node)
	{
		return node;
	}
	
	node->task_dump = task_dump;
	node->next = NULL;
	
	return node;
}

int add_task_dump_to_list(struct task_dump_list *list, struct task_dump *dump)
{
	struct task_dump_node *node;

	if(!list)
	{
		return 0;
	}

	node = init_task_dump_node(dump);
	
	if(!node)
	{
		return 0;
	}
	
	if(!list->head)
	{
		
		list->head = list->tail = node;		
	}
	else
	{
		list->tail->next = node;
		list->tail = node;
	}

	list->size++;

	return 1;
}

long get_size(struct task_dump_list *list)
{
	return (list ? list->size : 0);
}


struct task_dump *get_task_dump(struct task_dump_list *list, long n)
{
	struct task_dump_node *node = list->head;

	if(n > list->size)
	{
		return NULL;
	}

	while(n-- && node)
	{
		node = node->next;
	}

	if(node)
	{
		return node->task_dump;
	}

	return NULL;
}


void destroy_task_dump_list(struct task_dump_list *list)
{
	struct task_dump_node *temp, *to_delete;

	if(!list)
	{
		return;
	}

	temp = to_delete = list->head;
	
	while(to_delete)
	{
		temp = to_delete;
		to_delete = to_delete->next;
		
		if(temp->task_dump)
		{
			kfree(temp->task_dump);
		}
		
		kfree(temp);
	}
}

