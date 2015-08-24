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

#ifndef _TASK_DUMP_LIST_
#define _TASK_DUMP_LIST_

#include <linux/sched.h>

/* Export structure to the client, in order to avoid mutators and accessors */

#define MAX_STATE_LEN 2

struct task_dump {
	pid_t pid;
	uid_t uid;
	gid_t gid;
	char *aruments;
	char *enviroment;
	char state[MAX_STATE_LEN];
	char comm[TASK_COMM_LEN];
	signed long long start_time;
};

struct task_dump_list;

struct task_dump *init_task_dump_from_task(struct task_struct *task);

struct task_dump_list *init_task_dump_list(void);

int add_task_dump_to_list(struct task_dump_list *list, struct task_dump *dump);

void destroy_task_dump_list(struct task_dump_list *list);

struct task_dump *get_task_dump(struct task_dump_list *list, long n);

long get_size(struct task_dump_list *list);

#endif
