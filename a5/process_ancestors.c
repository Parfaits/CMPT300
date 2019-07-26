#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include "process_ancestors.h"

asmlinkage long sys_process_ancestors(
                struct process_info info_array[],
                long size,
                long *num_fill);

asmlinkage long sys_process_ancestors(struct process_info info_array[], long size, long *num_filled){
	struct process_info info;
	long fill = 0;
	long i = 0;
	// printk("\n");
	printk("size is %ld\n", size);
	if (size <= 0)
	{
		printk("size was 0\n");
		return -EINVAL;
	}
	if (copy_from_user(&fill, num_filled, sizeof(long)) != 0)
	{
		printk("Error copying num_filled\n");
		return -EFAULT;
	}
	if (info_array == NULL || num_filled == NULL)
	{
		printk("info_array or num_filled was NULL\n");
		return -EFAULT;
	}
	if (copy_from_user(&info, info_array, sizeof(struct process_info)) != 0)
	{
		printk("Error copying current process\n")
		return -EFAULT;
	}
	printk("num_filled is %ld\n", fill);
	for (i = 1; i < fill; ++i)
	{
		if (copy_from_user(&info, &info_array[i], sizeof(struct process_info)) != 0)
		{
			printk("Error copying info_array[%ld]\n", i);
			return -EFAULT;
		}
	}
	return 0;
}