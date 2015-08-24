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
#include <linux/ctype.h>
#include <asm/pgtable.h>
#include "vm-utils.h"
#include "my-kallsyms.h"

/* Memory Map of the kernel is not exported anymore, 
 * so find its address and cast it into a mm_struct
 */

static struct mm_struct *shedder_init_mm;

int get_init_mm(void)
{
	unsigned long init_mm_address;
	init_mm_address = my_kallsyms_lookup_name("init_mm");

	if(init_mm_address == 0)
	{
		return 0;
	}

	shedder_init_mm = (struct mm_struct *) init_mm_address;

	return 1;
}

int init_vm_utils(void)
{
	return (get_init_mm());
}

int mapped_address(unsigned long address)
{
	pgd_t *pgd;
	pmd_t *pmd;
	pte_t *ptep;

	if(shedder_init_mm == NULL)
	{
		return 0;
	}

	pgd = pgd_offset(shedder_init_mm, address);
	
	if(pgd_none(*pgd) || pgd_bad(*pgd))
	{
		return 0;
	}

	pmd = pmd_offset(pud_offset(pgd, address), address);
	
	if(pmd_none(*pmd) || pmd_bad(*pmd))
	{
		return 0;
	}

	ptep = pte_offset_kernel(pmd, address);

	if(!ptep || pte_none(*ptep))
	{
		return 0;
	}

	return 1;
}

/* Given a string and a limit, this function checks if the candidate string is
 * mapped in the virtual memory. If the limit is reached without encountering
 * a termination character, the given candidate is considered not a string.
 */

int is_mapped_string(char *string, int limit)
{
	int i = 0;

	while(limit--)
	{
		if(mapped_address( * (unsigned long *) (string + i) == 0))
		{
			/* Not a mapped character */

			return 0;
		}
		else if(string[i] == '\0' && i > 0)
		{
			/* All characters up to here are mapped and 
			 * termination character were found.
			 */

			return 1;
		}
		else if(isalnum(string[i]) == 0)
		{
			/* Not an alphanumeric */

			//TODO other valid characters?

			return 0;
		}

		i++;
	}

	/* limit reached, it's not a string */

	return 0;
}

