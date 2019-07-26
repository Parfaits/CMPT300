#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include "array_stats.h"

asmlinkage long sys_array_stats(
                struct array_stats *stats,
                long data[],
                long size);

asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size){
	long sum = 0;
	long max = 0;
	long min = 0;
	// long buf[size];
	long buf = 0;
	struct array_stats test;
	long i = 0;
	struct array_stats stuff;
	stuff.min = 0;
	stuff.max = 0;
	stuff.sum = 0;
	printk("size is %ld\n", size);
	if (size <= 0)
	{
		printk("size is <= 0\n");
		return -EINVAL;
	}
	if (copy_from_user(&test, stats, sizeof(struct array_stats)))
	{
		printk("Invalid address for stats\n");
		return -EFAULT;
	}
	if (stats == NULL || data == NULL)
	{
		printk("stats or data is NULL\n");
		return -EFAULT;
	}
	printk("BUF TESTING\n")
	if (copy_from_user(&buf, &data[0], sizeof(long)) != 0)
	{
		return -EFAULT;
	}
	max = buf;
	min = buf;
	sum += buf;
	printk("buf=%ld\n", buf);
	for (i = 1; i < size; ++i)
	{
		sum += buf;
		if (buf > max)
		{
			max = buf;
		}
		if (buf < min)
		{
			min = buf;
		}
	}
	stuff.sum = sum;
	stuff.min = min;
	stuff.max = max;
	printk("Stuff.sum is %ld\n", stuff.sum);
	printk("Stuff.max is %ld\n", stuff.max);
	printk("Stuff.min is %ld\n", stuff.min);
	if (copy_to_user(stats, &stuff, sizeof(struct array_stats)) != 0)
	{
		printk("Failed to write stats to user\n");
		return -EFAULT;
	}
	// if (copy_to_user(&stats->, &stuff, sizeof(struct array_stats)) != 0)
	// {
	// 	printk("Failed to write stats to user\n");
	// 	return -EFAULT;
	// }
	// if (copy_to_user(stats, &stuff, sizeof(struct array_stats)) != 0)
	// {
	// 	printk("Failed to write stats to user\n");
	// 	return -EFAULT;
	// }
	// if (copy_to_user(stats, &stuff, sizeof(struct array_stats)) != 0)
	// {
	// 	printk("Failed to write stats to user\n");
	// 	return -EFAULT;
	// }
	return 0;
}
