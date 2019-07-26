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
	long buf[size];
	struct array_stats test;
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
	if (copy_from_user(buf, data, size*sizeof(long)) != 0)
	{
		return -EFAULT;
	}
	printk("buf[0]=%ld\nn", buf[0]);
	for (i = 0; i < size; ++i)
	{
		sum += buf[i];
		if (buf[i] > max)
		{
			max = buf[i];
		}
		if (buf[i] < min)
		{
			min = buf[i];
		}
	}
	printk("Stuff.sum is %ld\n", stuff.sum);
	printk("Stuff.max is %ld\n", stuff.max);
	printk("Stuff.min is %ld\n", stuff.min);
	if (copy_to_user(stats, &stuff, sizeof(struct array_stats)) != 0)
	{
		printk("Failed to write stats to user\n");
		return -EFAULT;
	}
	return 0;
}
