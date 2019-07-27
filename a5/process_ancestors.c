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
	// long filled = 0;
	// long i = 0;
	long j = 0;
	long count_children = 0;
	long count_sibling = 0;
	struct process_info info;
	struct task_struct *task;
	struct list_head *head_children;
	struct list_head *head_sibling;
	// struct list_head cur_children;
	// struct list_head cur_sibling;
	printk("size is %ld\n", size);
	// if (size <= 0)
	// {
	// 	printk("size was 0\n");
	// 	return -EINVAL;
	// }
	// if (copy_from_user(&filled, num_filled, sizeof(long)) != 0)
	// {
	// 	printk("Error copying num_filled\n");
	// 	return -EFAULT;
	// }
	// if (info_array == NULL || num_filled == NULL)
	// {
	// 	printk("info_array or num_filled was NULL\n");
	// 	return -EFAULT;
	// }
	// if (copy_from_user(&info, info_array, sizeof(struct process_info)) != 0)
	// {
	// 	printk("Error copying current process\n");
	// 	return -EFAULT;
	// }
	// info.pid = current->pid;
	// printk("Getting name\n");
	// for (j = 0; j < ANCESTOR_NAME_LEN; ++j)
	// {
	// 	info.name[j] = current->comm[j];
	// 	printk("%c", info.name[j]);
	// }
	printk("\n");
	info.state = current->state;
	info.uid =  current->cred->uid.val;
	info.nvcsw = current->nvcsw;
	info.nivcsw = current->nivcsw;
	list_for_each(head_children, &current->children){
		task = list_entry(head_children, struct task_struct, children);
		printk("swag");
		info.num_children++;
	}
	list_for_each(head_sibling, &current->sibling){
		printk("DAB");
		task = list_entry(head_sibling, struct task_struct, sibling);
		info.num_siblings++;
	}
	info.num_children = count_children;
	info.num_siblings = count_sibling;
	current->parent;
	printk("info.pid is %ld\n", info.pid);
	printk("info.state is %ld\n", info.state);
	printk("info.uid is %ld\n", info.uid);
	printk("info.nvcsw is %ld\n", info.nvcsw);
	printk("info.nivcsw is %ld\n", info.nivcsw);
	printk("info.num_children is %ld\n", info.num_children);
	printk("info.num_siblings is %ld\n", info.num_siblings);
	printk("New pid is %d\n", current->pid);
	// for (i = 0; i < size; ++i)
	// {
	// 	if (copy_from_user(&info, &info_array[i], sizeof(struct process_info)) != 0)
	// 	{
	// 		printk("Error copying info_array[%ld]\n", i);
	// 		return -EFAULT;
	// 	}
	// 	info.pid = current->pid;
	// 	printk("Getting name\n");
	// 	for (j = 0; j < ANCESTOR_NAME_LEN; ++j)
	// 	{
	// 		info.name[j] = current.comm[j];
	// 		printk(info.name[j]);
	// 	}
	// 	printk("\n");
	// 	info.state = current->state;
	// 	info.uid = current->uid;
	// 	info.nvcsw = current->nvcsw;
	// 	info.nivcsw = current->nivcsw;
	// 	info.num_children = current->children;
	// 	info.num_siblings = current->sibling;
	// 	current = current->parent;
	// }
	return 0;
}
