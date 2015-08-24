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

#include "proc-entry.h"
#include <linux/kernel.h>
#include <linux/proc_fs.h>

static char *proc_file_name, *data_to_expose;
static struct proc_dir_entry *proc_entry;

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	if(offset > 0)
	{
		/* Nothing else to do */

		return 0;
	}
	else
	{
		return sprintf(buffer, "%s", data_to_expose);
	}
}

int init_proc_entry(char *name, char *data)
{
	if(proc_entry != NULL)
	{
		return 0;
	}
	
	proc_file_name = kmalloc(strlen(name) * sizeof(char) + 1, GFP_ATOMIC);
	
	if(!proc_file_name)
	{
		return 0;
	}

	strcpy(proc_file_name, name);

	data_to_expose = kmalloc(strlen(data) * sizeof(char) + 1, GFP_ATOMIC);

	if(!data_to_expose)
	{
		return 0;
	}

	strcpy(data_to_expose, data);

	proc_entry = create_proc_entry(proc_file_name, 0400, NULL);

	if(proc_entry == NULL)
	{
		return 0;
	}

	proc_entry->read_proc = procfile_read;
	proc_entry->mode = S_IFREG | S_IRUGO;
	proc_entry->uid = 0;
	proc_entry->gid = 0;
	proc_entry->size = strlen(data_to_expose);

	return 1;
}

void destroy_proc_entry(void)
{
	if(proc_entry)
	{
		remove_proc_entry(proc_file_name, NULL);
	}
	
	if(proc_file_name)
	{
		kfree(proc_file_name);
	}
	
	if(data_to_expose)
	{
		kfree(data_to_expose);
	}
}

