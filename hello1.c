#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include "hello.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Hello1 module");
MODULE_AUTHOR("IO-26 Vladyslav Bazyliuk");

static LIST_HEAD(hello_list);
static int kmalloc_fail_counter = 0;

void print_hello(int count) {
    struct hello_data *data;
    int i;

    for (i = 0; i < count; i++) {
        kmalloc_fail_counter++;
        if (kmalloc_fail_counter == 4) {
            pr_err("Simulated kmalloc failure on iteration 4");
            return;
        }

        data = kmalloc(sizeof(*data), GFP_KERNEL);
        if (!data) {
            pr_err("Failed to allocate memory\n");
            return;
        }

        data->start_time = ktime_get();
        pr_info("Hello, world!\n");
        data->end_time = ktime_get();
        list_add_tail(&data->list, &hello_list);
    }
}

EXPORT_SYMBOL(print_hello);

static void __exit hello1_exit(void) {
    struct hello_data *data, *tmp;

    list_for_each_entry_safe(data, tmp, &hello_list, list) {
        pr_info("Print duration: %lld ns\n",
                ktime_to_ns(data->end_time) - ktime_to_ns(data->start_time));
        list_del(&data->list);
        kfree(data);
    }
}

module_exit(hello1_exit);

