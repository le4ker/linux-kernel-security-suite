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
 
#include <stdio.h>
#include <fcntl.h>
#include "../src/command-chrdev.h"

/* A simple source to trigger the module to execute the carbonite functionality.
 * This utility shoud be used from the system's administrator after a security
 * incident, in order to freeze the system and dump snapshots of the running processes.
 */

int main(void)
{	
	int fd, ret;
	
	fd = open("/dev/command-the-drip-dry-carbonite", O_RDONLY);
	
	if(fd < 0)
	{
		perror("/dev/command-the-drip-dry-carbonite");
		return -1;
	}

	ret = ioctl(fd, IOCTL_CARBONITE_COMMAND);

	if(ret != 0)
	{
		perror("need root");
		return -1;
	}

	close(fd);

	return 0;
}


