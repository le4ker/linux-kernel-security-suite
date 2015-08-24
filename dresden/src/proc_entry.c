#include "proc_entry.h"
#include <linux/proc_fs.h>

struct proc_dir_entry *dresden_proc_entry;

int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	if(offset > 0)
	{
		/* Nothing else to do */
	
		return 0;
	}
	else
	{
		/* Just return '1' to say that dresden is deployed */
	
		return sprintf(buffer, "1\n");
	}
}


int init_proc_entry(void)
{
	if(dresden_proc_entry != NULL)
	{
		return PROC_ENTRY_EXISTS;
	}
	
	dresden_proc_entry = create_proc_entry(PROC_FS_NAME, 0444, NULL);

	if(dresden_proc_entry == NULL)
	{
		return -ECREATE_PROC_ENTRY;
	}

	dresden_proc_entry->read_proc = procfile_read;
	dresden_proc_entry->mode = S_IFREG | S_IRUGO;
	dresden_proc_entry->uid = 0;
	dresden_proc_entry->gid = 0;
	dresden_proc_entry->size = 1;

	return PROC_CREATED;
}

