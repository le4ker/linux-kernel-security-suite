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

#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/utsname.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/workqueue.h>
#include "carbonite.h"

#define UDP_LOG_PORT 514
#define BUFFER_SIZE 1920

static int log_remote;

static struct socket *sock;
static struct sockaddr_in log_addr;

/* We cannot send messages when in atomic, so we create work 
 * queues and add works that they will send the messages in process context
 */

static struct workqueue_struct *remote_log_work_queue;

struct remote_log_work
{
	struct work_struct my_work;
	char *buffer;
};

void ksocket_send(struct work_struct *work)
{
	int length;
	char *buffer;
	struct iovec iov;
	struct msghdr msg;
	mm_segment_t oldfs;
	struct remote_log_work *remote_log_work;

	if(!sock || !sock->sk || !work)
	{
		return;
	}

	remote_log_work = (struct remote_log_work *) work;
	buffer = remote_log_work->buffer;
	length = strnlen(buffer, BUFFER_SIZE);

	memset(&msg, 0, sizeof(struct msghdr));
	msg.msg_flags = 0;
	msg.msg_name = (struct sockaddr *) &log_addr;
	msg.msg_namelen = sizeof(struct sockaddr_in);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	iov.iov_base = (char *) buffer;
	iov.iov_len = length;

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	/* If it fails to send the message it will not log any alert.
	 * There is nothing we can do more. Persistent sending may cause problems.
	 */

	sock_sendmsg(sock, &msg, (size_t) length);
	set_fs(oldfs);

	kfree(remote_log_work->buffer);
	kfree(work);
}

int remote_udp_log(const char *format, va_list args) 
{
	char buffer[BUFFER_SIZE + 1];
	int length, remaining, bytes_written;

	length = 0;
	remaining = BUFFER_SIZE;

	if(!sock)
	{
		return -1;
	}

	{
	/* Get time */
	
	struct rtc_time tm;
	struct timespec curtime = CURRENT_TIME;

	rtc_time_to_tm(curtime.tv_sec, &tm);
	bytes_written = snprintf(buffer, remaining, "%d-%02d-%dT%d:%d:%d.%d+00:00 ",
			      1900 + tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour,
			      tm.tm_min, tm.tm_sec, (unsigned int) curtime.tv_nsec / 1000);
	}

	if(bytes_written >= remaining)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to log message (too long message)\n");

		return 0;		
	}

	length += bytes_written;
	remaining -= length;
	
	{
	/* Get uts name */
		
	struct new_utsname *uts_name;

	uts_name = utsname();
	
	bytes_written = snprintf(buffer + length, __NEW_UTS_LEN, "%s", uts_name->nodename);

	if(bytes_written >= __NEW_UTS_LEN)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to log message (too long message)\n");
		
		return 0;		
	}

	length += bytes_written;	
	remaining -= length;
	}

	/* Add module's name and the log message */
	
	bytes_written = snprintf(buffer + length, remaining, " kernel: " MODULE_NAME ": ");

	if(bytes_written >= remaining)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to log message (too long message)\n");

		return 0;		
	}

	length += bytes_written;
	remaining -= length;

	bytes_written = vsnprintf(buffer + length, remaining, format, args);

	if(bytes_written >= remaining)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to log message (too long message)\n");

		return 0;		
	}

	length += bytes_written;
	remaining -= length;

	buffer[length++] = '\0';

	{
	struct remote_log_work *remote_log_work;

	remote_log_work = kmalloc(sizeof(struct remote_log_work), GFP_ATOMIC);

	if(remote_log_work == NULL)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to log message (work allocation failed)\n");
		
		return 0;
	}

	remote_log_work->buffer = kmalloc(sizeof(char) * length, GFP_ATOMIC);

	if(remote_log_work->buffer == NULL)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Failed to log message (work buffer allocation failed)\n");
		kfree(remote_log_work);
				
		return 0;		
	}

	strncpy(remote_log_work->buffer, buffer, length);
	
	INIT_WORK((struct work_struct *) remote_log_work, ksocket_send);
	queue_work(remote_log_work_queue, (struct work_struct *) remote_log_work);
	}
	
	return length;
}

int print_log(const char *format, ...) 
{
	int bytes, i;
	va_list args;
	char log_level[4] = { '\0' };

	if(!format)
	{
		return -1;
	}

	/* Get first 3 characters of the format, it's the log level */

	i = 0;
	while(i < 3 && format[i] != '\0')
	{
		log_level[i] = format[i];
		i++;
	}

	va_start(args, format);
	bytes = vprintk(format, args);

	if(log_remote) 
	{
		remote_udp_log(format, args);
	}

	va_end(args);	

	return bytes;
}

int init_logger(char *ip)
{
	if(sock)
	{
		printk(KERN_ERR MODULE_NAME ":\t[-] Logger was already initialized\n");

		return 0;
	}

	if(ip)
	{
		printk(KERN_INFO MODULE_NAME ":\t[+] Remote logging is enabled\n");

		log_remote = 1;

		log_addr.sin_family = AF_INET;
		log_addr.sin_port = htons(UDP_LOG_PORT);
		log_addr.sin_addr.s_addr = in_aton(ip);


		if(sock_create_kern(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock) < 0)
		{
			sock = NULL;
			printk(KERN_ERR MODULE_NAME ":\t[-] Failed to create the socket\n");
		
			return 0;
		}

		remote_log_work_queue = create_workqueue("remote_log_work_queue");

		if(!remote_log_work_queue)
		{
			printk(KERN_ERR MODULE_NAME ":\t[-] Failed to create the work queue\n");
		
			return 0;
		}
	}
	else
	{
		printk(KERN_INFO MODULE_NAME ":\t[-] Remote logging is not enabled\n");	
	}

	return 1;
}

void destroy_logger(void)
{
	if(sock)
	{
		sock_release(sock);
		sock = NULL;
	}

	if(remote_log_work_queue)
	{
		destroy_workqueue(remote_log_work_queue);
	}
}



