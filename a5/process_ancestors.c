#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include "process_ancestors.h"

asmlinkage long sys_process_ancestors(
                struct process_info info_array[],
                long size,
                long *num_filled);

asmlinkage long sys_process_ancestors(struct process_info info_array[], long size, long *num_filled){
	long check_num_filled = 0;
	long filled = 0;
	long i = 0;
	long j = 0;
	long count_children = 0;
	long count_sibling = -1;
	struct process_info info;
	struct task_struct *task = current;
	struct list_head *head_children;
	struct list_head *head_sibling;
	printk("size is %ld\n", size);
	if (size <= 0)
	{
		printk("size was 0\n");
		return -EINVAL;
	}
	if (copy_from_user(&check_num_filled, num_filled, sizeof(long)) != 0)
	{
		printk("Error copying num_filled\n");
		return -EFAULT;
	}
	if (info_array == NULL || num_filled == NULL)
	{
		printk("info_array or num_filled was NULL\n");
		return -EFAULT;
	}
	printk("num_filled is %ld\n", check_num_filled);
	for (i = 0; i < size; ++i)
	{
		if (copy_from_user(&info, &info_array[i], sizeof(struct process_info)) != 0)
		{
			printk("Error copying info_array[%ld]\n", i);
			return -EFAULT;
		}
		info.pid = task->pid;
		for (j = 0; j < ANCESTOR_NAME_LEN; ++j)
		{
			info.name[j] = task->comm[j];
		}
		j = 0;
		info.state = task->state;
		info.uid =  task->cred->uid.val;
		info.nvcsw = task->nvcsw;
		info.nivcsw = task->nivcsw;
		list_for_each(head_children, &task->children){
			count_children++;
		}
		list_for_each(head_sibling, &task->sibling){
			count_sibling++;
		}
		if(count_sibling<0){
			count_sibling=0;
		}
		info.num_children = count_children;
		info.num_siblings = count_sibling;
		// printk("info.pid is %ld", info.pid);
		// printk("info.state is %ld", info.state);
		// printk("info.uid is %ld", info.uid);
		// printk("info.nvcsw is %ld", info.nvcsw);
		// printk("info.nivcsw is %ld", info.nivcsw);
		// printk("info.num_children is %ld", info.num_children);
		// printk("info.num_siblings is %ld", info.num_siblings);
		// printk("================================================================");
		filled++;
		if (copy_to_user(&info_array[i], &info, sizeof(struct process_info)) != 0)
		{
			printk("Error copying info to user\n");
			return -EFAULT;
		}
		count_children = 0;
		count_sibling = -1;
		if (task->parent->pid == task->pid)
		{
			break;
		}
		task = task->parent;
	}
	printk("filled is %ld\n", filled);
	if (copy_to_user(num_filled, &filled, sizeof(long)) != 0)
	{
		printk("Error copying filled to user\n");
		return -EFAULT;
	}
	return 0;
}
