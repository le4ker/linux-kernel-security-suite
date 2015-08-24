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

#include <linux/proc_fs.h>
#include "task-dump.h"
#include "task-dump-list.h"
#include "task-dump-seq-file.h"

static struct task_dump_list *dump_list;

int init_task_dump(void)
{
	if(dump_list)
	{
		destroy_task_dump_list(dump_list);
	}

	dump_list = init_task_dump_list();

	if(dump_list == NULL)
	{
		return 0;
	}

	return init_dump_seq_file(dump_list);
}

int dump_task(struct task_struct *task)
{
	return add_task_dump_to_list(dump_list, init_task_dump_from_task(task));
}

void destroy_task_dump(void)
{
	destroy_dump_seq_file();
	destroy_task_dump_list(dump_list);
}
