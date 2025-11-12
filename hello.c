
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/slab.h> // for kmalloc/kfree
#include <linux/ktime.h> // for ktime_t

MODULE_AUTHOR("Diana Menchynska <menchynska.diana@lll.kpi.ua>");
MODULE_DESCRIPTION("Lab4. Building a custom kernel module training");
MODULE_LICENSE("Dual BSD/GPL");

// Parameter declaration
static uint howmany = 1;
module_param(howmany, uint, 0444);
MODULE_PARM_DESC(howmany,
	"Determines the number of times the string 'Hello, world!' is printed.\n"
	"Valid values range is [1:10], recommended range [1:4], default is 1");

// Linked list declaration
struct time_data {
	ktime_t ktime;
	struct list_head tlist;
};

static LIST_HEAD(time_data_head);

static int __init hello_init(void)
{
	struct time_data *ptr;
	uint n;

	// Check the parameter value
	if (howmany > 10) {
		printk(KERN_ERR "hello: Parameter value greater than 10!\n");
		return -EINVAL;
	}

	if (howmany == 0 || howmany >= 5)
		printk(KERN_WARNING "hello: Not recommended parameter value: %d\n",
			howmany);

	// Print the message the required number of times
	for (n = 1; n <= howmany; n++) {
		ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);
		ptr->ktime = ktime_get();

		if (ptr == NULL) {
			printk(KERN_ERR "hello: Memory allocation error\n");
			return -ENOMEM;
		}

		list_add_tail(&ptr->tlist, &time_data_head);

		printk(KERN_INFO "hello: #%d Hello, world!\n", n);
	}

	printk(KERN_INFO "hello: Module loaded.\n");

	return 0;
}

static void __exit hello_exit(void)
{
	struct time_data *entry, *tmp;
	int count = 0;

	// print all elements in list
	list_for_each_entry_safe(entry, tmp, &time_data_head, tlist) {
		count++;

		printk(KERN_INFO "hello: Event #%d, kernel time = %lld ns.\n",
			count, ktime_to_ns(entry->ktime));

		// Remove the element from the list
		list_del(&entry->tlist);

		// Free the allocated memory
		kfree(entry);
	}

	printk(KERN_INFO "hello: %d elements freed, module unloaded.\n", count);
}

module_init(hello_init);
module_exit(hello_exit);
