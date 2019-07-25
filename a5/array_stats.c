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
	long buf = 0;
	long i = 0;
	struct array_stats stuff;
	stuff.min = 0;
	stuff.max = 0;
	stuff.sum = 0;
	if (size <= 0)
	{
		printk("size is <= 0\n");
		return -EINVAL;
	}
	printk("size is %ld\n", size);
	if (stats == NULL || data == NULL)
	{
		printk("stats or data is NULL\n");
		return -EFAULT;
	}
	if (copy_from_user(&max, &data[0], sizeof(long)) != 0)
	{
		return -EFAULT;
	}
	if (copy_from_user(&min, &data[0], sizeof(long)) != 0)
	{
		return -EFAULT;
	}
	sum += max;
	for (i = 1; i < size; ++i)
	{
		if (copy_from_user(&buf, &data[i], sizeof(long)) != 0)
		{
			printk("Failed to read data from user\n");
			return -EFAULT;
		}
		printk("buf is %ld\n", buf);
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
	printk("Stuff.sum is %ld\n", stuff.sum);
	printk("Stuff.max is %ld\n", stuff.max);
	printk("Stuff.min is %ld\n", stuff.min);
	// if (copy_to_user(&stats->sum, &sum, sizeof(struct array_stats*)) != 0)
	// {
	// 	printk("Failed to write stats to user\n");
	// 	return -EFAULT;
	// }
	// if (copy_to_user(&stats->max, &max, sizeof(struct array_stats*)) != 0)
	// {
	// 	printk("Failed to write stats to user\n");
	// 	return -EFAULT;
	// }
	// if (copy_to_user(&stats->min, &min, sizeof(struct array_stats*)) != 0)
	// {
	// 	printk("Failed to write stats to user\n");
	// 	return -EFAULT;
	// }
	if (copy_to_user(stats, &stuff, sizeof(struct array_stats)) != 0)
	{
		printk("Failed to write stats to user\n");
		return -EFAULT;
	}
	return 0;
}