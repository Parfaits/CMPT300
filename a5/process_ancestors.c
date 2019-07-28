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
	long filled = 0;
	long i = 0;
	long j = 0;
	//long count_children = 0;
	//long count_sibling = 0;
	struct process_info info;
	struct task_struct *task;
	//struct task_struct *child_struct;
	struct list_head *head_children;
	struct list_head *head_sibling;
	printk("size is %ld\n", size);
	if (size <= 0)
	{
		printk("size was 0\n");
		return -EINVAL;
	}


	if (copy_from_user(&filled, num_filled, sizeof(long)) != 0)
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
		printk("Error copying current process\n");
		return -EFAULT;
	}

	task = current;
	filled++;
	printk("current->pid is %d", current->pid);
	for (i = 0; i < size; ++i)
	//while (current->parent != current)
	{
		if (copy_from_user(&info, &info_array[i], sizeof(struct process_info)) != 0)
		{
			printk("Error copying info_array[%ld]\n", i);
			//return -EFAULT;
		}

		info.pid = task->pid;
		for (j = 0; j < ANCESTOR_NAME_LEN; ++j)
		{
			info.name[j] = task->comm[j];
		}
		info.state = task->state;
		info.uid =  task->cred->uid.val;
		info.nvcsw = task->nvcsw;
		info.nivcsw = task->nivcsw;
		printk("info.pid is %ld", info.pid);
		printk("info.name is %s", info.name);
		printk("info.state is %ld", info.state);
		printk("info.nvcsw is %ld", info.nvcsw);
		printk("info.nivcsw is %ld", info.nivcsw);
	
		list_for_each(head_children, &task->children){
			info.num_children++;
			//task = list_entry(head_children, struct task_struct, children);		
		}
		list_for_each(head_sibling, &task->sibling){
			info.num_siblings++;
			//task = list_entry(head_sibling, struct task_struct, sibling);
		}

		if (copy_to_user(&info_array[i], &info, sizeof(struct process_info)) != 0)
		{
			printk("Error copying info to user\n");
			return -EFAULT;
		}

		filled++;

		if (task->parent->pid == info.pid)
		{
			break;
		}
		task = task->parent;

	}
	if (copy_to_user(num_filled, &filled, sizeof(long)) != 0)
	{
		printk("Error copying filled to user\n");
		return -EFAULT;
	}
	
	return 0;
}
