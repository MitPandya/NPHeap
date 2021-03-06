//////////////////////////////////////////////////////////////////////
//                             North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng
//
//   Description:
//     Skeleton of NPHeap Pseudo Device
//
////////////////////////////////////////////////////////////////////////

#include "npheap.h"

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/list.h>


struct node_list {
    struct npheap_cmd cmd;
    struct mutex lock;
    //long offset;
    unsigned long km_addr_start;
    unsigned long phys_addr;
	//unsigned long size;
    struct list_head list;
};

extern struct node_list ndlist;
extern struct mutex lock;

// 
long npheap_lock(struct npheap_cmd __user *user_cmd)
{
    /*struct npheap_cmd cmd;
    if(copy_from_user(&cmd, user_cmd, sizeof(*user_cmd))) {
        return -1;
    }
    struct node_list *tmp;
    struct list_head *pos, *q;
    list_for_each_safe(pos, q, &ndlist.list) {
        tmp = list_entry(pos, struct node_list, list);
        if((cmd.offset >> PAGE_SHIFT) == tmp->cmd.offset){
            tmp->cmd.op = 0;
            //mutex_lock(tmp->lock);
            break;
        }
    }*/
    mutex_lock(&lock);

    return 0;
}     

long npheap_unlock(struct npheap_cmd __user *user_cmd)
{
    /*struct npheap_cmd cmd;
    if(copy_from_user(&cmd, user_cmd, sizeof(*user_cmd))) {
        return -1;
    }
    struct node_list *tmp;
    struct list_head *pos, *q;
    list_for_each_safe(pos, q, &ndlist.list) {
        tmp = list_entry(pos, struct node_list, list);
        if((cmd.offset >> PAGE_SHIFT) == tmp->cmd.offset) {
            tmp->cmd.op = 1;
            //mutex_unlock(tmp->lock);
            break;
        }
    }*/
    mutex_unlock(&lock);
    return 0;
}

long npheap_getsize(struct npheap_cmd __user *user_cmd)
{
    struct npheap_cmd cmd;
    if(copy_from_user(&cmd, user_cmd, sizeof(*user_cmd))) {
        return -1;
    }
	struct node_list *tmp;
	struct list_head *pos, *q;
	list_for_each_safe(pos, q, &ndlist.list) {
        tmp = list_entry(pos, struct node_list, list);
        if ((cmd.offset >> PAGE_SHIFT) == tmp->cmd.offset){
            printk(KERN_INFO "found in ioctl %zu %zu\n",tmp->cmd.offset, cmd.offset);
            return tmp->cmd.size;
        }
	}
	return 0;
}

long npheap_delete(struct npheap_cmd __user *user_cmd)
{
    struct npheap_cmd cmd;
    if(copy_from_user(&cmd, user_cmd, sizeof(*user_cmd))) {
        return -1;
    }
    struct node_list *tmp;
    struct list_head *pos, *q;
    list_for_each_safe(pos, q, &ndlist.list) {
        tmp = list_entry(pos, struct node_list, list);
        if ((cmd.offset >> PAGE_SHIFT) == tmp->cmd.offset){
            //Delete Code
	    printk(KERN_INFO "deleting node %zu\n",tmp->cmd.offset);
	    //list_del(pos);
	    tmp->cmd.size = 0;
            kfree(tmp->cmd.data);
            break;
        }
    }
    struct node_list *tmp1;
    struct list_head *pos1, *q1;
    list_for_each_safe(pos1, q1, &ndlist.list) {
        tmp1 = list_entry(pos1, struct node_list, list);
        printk("del offset is %zu \n",tmp1->cmd.offset);
    }

    return 0;
}

long npheap_ioctl(struct file *filp, unsigned int cmd,
                                unsigned long arg)
{
    switch (cmd) {
    case NPHEAP_IOCTL_LOCK:
        return npheap_lock((void __user *) arg);
    case NPHEAP_IOCTL_UNLOCK:
        return npheap_unlock((void __user *) arg);
    case NPHEAP_IOCTL_GETSIZE:
        return npheap_getsize((void __user *) arg);
    case NPHEAP_IOCTL_DELETE:
        return npheap_delete((void __user *) arg);
    default:
        return -ENOTTY;
    }
}
