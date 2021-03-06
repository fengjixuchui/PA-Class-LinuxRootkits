/*
 * WARNING:
 *
 * This LKM has no exit routine! As a result, it cannot be removed in the standard way (rmmmod)
 * 
 */


#define pr_fmt(fmt)	KBUILD_MODNAME " -> %s:%d -> " fmt,__func__,__LINE__
#include <linux/module.h>
#include <linux/init.h>
#include "processlibrary.h"

MODULE_AUTHOR("Cooper Van Kampen");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("(WARNING: CANNOT BE EXITED) Get details about given task by its userspace PID");

static int process_id;
module_param(process_id, int, S_IWUSR | S_IRUGO);
MODULE_PARM_DESC(process_id, "Target userspace PID");

static int __init lkm_init(void) {
	struct task_struct *target_task;
	struct task_struct *process_thread;

	target_task = get_task_struct_by_pid(process_id);
	if(!IS_ERR_OR_NULL(target_task)) {
		// PID details
		pr_info("Received uspace PID: %d\n", process_id);
		print_task_pid_details(target_task);
		print_task_parent_pid_details(target_task);

		// Thread details for PID
		pr_info("Number of threads: %d\n", get_task_thread_count(target_task));
		pr_info("Threads details\n");
		for_each_thread(target_task, process_thread) // Uses RCU doubly linked list macro in signal.h
			print_task_pid_details(process_thread);

		// Path details
		pr_info("Task's root and working directory paths\n");
		print_task_binary_name(target_task);
		print_task_root_path_pwd(target_task);

		// File descriptor details
		pr_info("Task's open file list");
		print_list_of_open_files(target_task);

		// UID & EUID
		pr_info("Task's UID & EUID (owner) information\n");
		print_task_owner_info(target_task);
	} else {
		pr_info("Received invalid uspace PID: %d\n", process_id);
		return -EINVAL;
	}

	return 0;
}

// Define init and exit functions
module_init(lkm_init);
