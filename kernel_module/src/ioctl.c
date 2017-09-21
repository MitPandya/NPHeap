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
    //  struct mutex lock;
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
/*    struct node_list *tmp;

    if(list_empty(&(ndlist.list))) {
	printk(KERN_INFO "init list");
        INIT_LIST_HEAD(&ndlist.list);
    }
    INIT_LIST_HEAD(&ndlist.list);

    tmp = (struct node_list *)kmalloc(sizeof(struct node_list), GFP_KERNEL);
    if(copy_from_user(&(tmp->cmd), user_cmd, sizeof(tmp->cmd))) {
        return -1;
    }
    list_add(&(tmp->list), &(ndlist.list));
    
    int s = list_empty(&(ndlist.list));
    printk(KERN_INFO "Is List Empty %d\n", s);*/
    /*struct node_list *tmp;
    struct list_head *pos;
    list_for_each(pos, &ndlist.list) {
        tmp = list_entry(pos, struct node_list, list);
        if(user_cmd->offset == tmp->cmd.offset){
            tmp->cmd.op = 0;
            break;
        }
    }*/
    mutex_lock(&lock);

    return 0;
}     

long npheap_unlock(struct npheap_cmd __user *user_cmd)
{
    /*struct node_list *tmp;
    struct list_head *pos;
    list_for_each(pos, &ndlist.list) {
        tmp = list_entry(pos, struct node_list, list);
        if(user_cmd->offset == tmp->cmd.offset) {
            tmp->cmd.op = 1;
            break;
        }
    }*/
    mutex_unlock(&lock);
    return 0;
}

long npheap_getsize(struct npheap_cmd __user *user_cmd)
{
	struct npheap_cmd *n_cmd;
	n_cmd = (struct node_list *)kmalloc(sizeof(struct node_list), GFP_KERNEL);
	if(copy_from_user(n_cmd, user_cmd, sizeof(struct node_list))) {
		printk(KERN_ERR "error in copy_from_user");
        	return -1;
	}
	n_cmd->offset = n_cmd->offset/PAGE_SIZE;
	
	struct node_list *tmp;
	struct list_head *pos;
	list_for_each(pos, &ndlist.list) {
	  tmp = list_entry(pos, struct node_list, list);
	  if (n_cmd->offset == tmp->cmd.offset){
		  printk(KERN_INFO "found in ioctl %zu %zu, size is %zu \n",tmp->cmd.offset, n_cmd->offset,tmp->cmd.size);
		  return tmp->cmd.size;
	  }
	}
        return 0;
}
long npheap_delete(struct npheap_cmd __user *user_cmd)
{
    struct node_list *tmp;
    struct list_head *pos;
    list_for_each(pos, &ndlist.list) {
      tmp = list_entry(pos, struct node_list, list);

      if (user_cmd->offset == tmp->cmd.offset){
            //Delete Code
            //kfree(tmp->cmd.data);
            break;
      }
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
