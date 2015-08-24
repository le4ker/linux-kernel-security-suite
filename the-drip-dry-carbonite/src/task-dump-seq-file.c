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

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include "task-dump-seq-file.h"
#include "task-dump-list.h"
#include "logger.h"

#define DUMP_PROC_ENTRY_NAME "the-drip-dry-carbonite-dump"

static struct task_dump_list *list;
static struct proc_dir_entry *dump_proc_entry;

void *task_dump_seq_start(struct seq_file *s, loff_t *pos)
{
	if(*pos == 0)
	{
		loff_t *spos = kmalloc(sizeof(loff_t), GFP_KERNEL);

		if(!spos)
		{
			return spos;
		}

		*spos = *pos;
		
		return spos;
	}
	else if(*pos > get_size(list))
	{
		/* Terminate the sequence */

		*pos = 0;
		
		return NULL;
	}
	else 
	{
		return pos;
	}
}

void *task_dump_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos = (loff_t *) v;

	/* size of list + 1, because of the header (printed when the iterator is 0) */

	if(*pos > get_size(list) + 1)
	{
		return NULL;
	}

	*pos = ++(*spos);	
	
	return spos;
}

void task_dump_seq_stop(struct seq_file *s, void *v)
{
	if(v)
	{
		kfree(v);
	}
}

int task_dump_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	struct task_dump *dump;

	if(!list || get_size(list) == 0)
	{	
		/* Don't display the header if the list is empty */
	
		return 0;
	}

	/* First element to print will be the header of the report */

	if(*spos == 0)
	{
		char log[MAX_LOG_LEN];
	
		sprintf(log, "  PID     UID     GID  STATE         NAME       Start Time\n");
	
		/* Log remotely (if possible) and show also the log */
	
		print_log(log);
		seq_printf(s, log);	

		return 0;
	}

	/* Reduce the iterator by 1, because the header that we printed */

	dump = get_task_dump(list, (*spos - 1));
	
	if(dump)
	{
		char log[MAX_LOG_LEN];
	
		sprintf(log, "%4d\t%4d\t%4d\t%s    %15s\t%lld\n", dump->pid, dump->uid, dump->gid, dump->state, 
										dump->comm, dump->start_time);
	
		/* Log remotely (if possible) and show also the log */

		print_log(log);
		seq_printf(s, log);
	}

	return 0;
}

struct seq_operations task_dump_seq_ops = {
	.start = task_dump_seq_start,
	.next  = task_dump_seq_next,
	.stop  = task_dump_seq_stop,
	.show  = task_dump_seq_show
};

int task_dump_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &task_dump_seq_ops);
};

struct file_operations task_dump_file_ops = {
	.owner   = THIS_MODULE,
	.open    = task_dump_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};

int init_dump_seq_file(struct task_dump_list *dump_list)
{
	if(!dump_list)
	{
		return 0;
	}

	list = dump_list;

	if(dump_proc_entry)
	{
		return 1;
	}

	dump_proc_entry = create_proc_entry(DUMP_PROC_ENTRY_NAME, 0400, NULL);

	if(!dump_proc_entry)
	{
		return 0;
	}

	dump_proc_entry->proc_fops = &task_dump_file_ops;

	return 1;
}

void destroy_dump_seq_file(void)
{
	if(dump_proc_entry)
	{
		remove_proc_entry(DUMP_PROC_ENTRY_NAME, NULL);
		dump_proc_entry = NULL;
	}
}


