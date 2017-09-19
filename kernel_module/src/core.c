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

extern struct miscdevice npheap_dev;

struct node_list {
	struct npheap_cmd cmd;
	//  struct mutex lock;
	long offset;
	//unsigned long km_addr_start;
	unsigned long phys_addr;
	//unsigned long size;
	struct list_head list;
};

struct node_list ndlist;
struct mutex lock;

int npheap_mmap(struct file *filp, struct vm_area_struct *vma)
{
 // struct node_list pos;
  //pos = ndlist;
  int found = 0;
  struct node_list *tmp;
  struct list_head* pos;
  unsigned long phys_addr;
  unsigned long size = vma->vm_end - vma->vm_start;

  int s = list_empty(&(ndlist.list));
  printk(KERN_INFO "is list empty %d",s);

  list_for_each ( pos , & ndlist.list ){
	  tmp= list_entry(pos, struct node_list, list);

	  if (vma->vm_pgoff == tmp->offset){
		  found = 1;
		  //vma->vm_start = tmp->km_addr_start;
		  printk(KERN_INFO "found %zu %zu %x \n",tmp->offset, vma->vm_pgoff);
		  break;
	  }
  }

  if ( found == 0) {
	  void *kmemory = kmalloc(size, GFP_KERNEL);
	  printk(KERN_INFO "I got: %zu bytes of memory\n", ksize(kmemory));
	  printk(KERN_INFO "VMA offset: %zu, Offset: %zu", vma->vm_pgoff, vma->vm_pgoff*PAGE_SHIFT);
	  printk(KERN_INFO "flag is %zu", vma->vm_page_prot);
	  phys_addr = (unsigned long)virt_to_phys((void *)kmemory) >> PAGE_SHIFT;
	  //	        printk(KERN_INFO "VMA offset: %zu", vma->vm_pgoff);

	  int ret;
	  if((ret = remap_pfn_range(vma,
			  vma->vm_start,
			  phys_addr,
			  size,
			  vma->vm_page_prot)) < 0) {
		  printk(KERN_ERR "ret is %d\n", ret);
		  return ret;
	  }
	  printk(KERN_INFO "phys addr is %x %zu %p %p \n", phys_addr, phys_addr, phys_addr, vma->vm_start);

	  tmp = (struct node_list *)kmalloc(sizeof(struct node_list), GFP_KERNEL);
	  tmp->offset = vma->vm_pgoff;
	  //tmp->km_addr_start = vma->vm_start;
	  tmp->phys_addr = phys_addr;
	  tmp->cmd.size = size;
	  list_add(&(tmp->list), &(ndlist.list));

  } else if (found == 1){
	  int ret;
	  printk(KERN_INFO "Actual size vs new size  %zu %zu ", tmp->cmd.size, size);
	  if((ret = remap_pfn_range(vma,
			  vma->vm_start,
			  tmp->phys_addr,
			  tmp->cmd.size,
			  vma->vm_page_prot)) < 0) {
		  printk(KERN_ERR "ret is %d\n", ret);
		  return ret;
	  }

  }

	return 0;
}

int npheap_init(void)
{
	int ret;
	if ((ret = misc_register(&npheap_dev)))
		printk(KERN_ERR "Unable to register \"npheap\" misc device\n");
	else{
		INIT_LIST_HEAD(&ndlist.list);
		mutex_init(&lock);
		printk(KERN_ERR "\"npheap\" misc device installed\n");
	}
	return ret;
}

void npheap_exit(void)
{
    misc_deregister(&npheap_dev);
}
