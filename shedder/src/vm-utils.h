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

#ifndef _VM_UTILS
#define _VM_UTILS

int init_vm_utils(void);

/* Given a linear address, it returns if it's mapped into the Virtual Memory */

int mapped_address(unsigned long address);

/* Given a string and a limit, this function checks if the candidate string is
 * mapped in the virtual memory. If the limit is reached without encountering
 * a termination character, the given candidate is considered not a string.
 */

int is_mapped_string(char *string, int limit);


#endif
