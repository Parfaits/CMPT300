#include <linux/kernel.h>
#include <limits.h>
#include "array_stats.h"

asmlinkage long sys_array_stats(
                struct array_stats *stats,
                long data[],
                long size);

asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size){
	long sum = 0;
	long max = LONG_MIN;
	long min = LONG_MAX;
	long buf = 0;
	struct array_stats* stuff;
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
	for (long i = 0; i < size; ++i)
	{
		if (copy_from_user(&buf, &data[i], sizeof(long)) != 0)
		{
			printk("Failed to read data from user\n");
			return -EFAULT;
		}
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
	stuff->sum = sum;
	stuff->max = max;
	stuff->min = min;
	if (copy_to_user(stats, stuff, sizeof(struct array_stats*)) != 0)
	{
		printk("Failed to write stats to user\n");
		return -EFAULT;
	}
	return 0;
}