#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/timekeeping.h>
#include <linux/time.h>




#define MEM_SIZE 1024
#define MAGIC_NUMBER	234
#define WRITE_TEXT		_IOWR(MAGIC_NUMBER,0,char *)
#define SELECT_SORT	 	_IOWR(MAGIC_NUMBER,1,char *)
#define INSERT_SORT		_IOWR(MAGIC_NUMBER,2,char *)
#define SHOW_TIME		_IOWR(MAGIC_NUMBER,3,char *)



static dev_t dev_num;
static struct class *dev_class;
static struct cdev *dev_cdev;
static int flag;
static int flagWriteText = 0;
static int flagBubbleSort = 0,flagInsertSort = 0;
static char *kernel_buf;
static char text_bubble[1024],text_insert[1024];
static struct timespec64 wt_start_bubble;
static struct timespec64 wt_end_bubble;
static struct timespec64 wt_start_insert;
static struct timespec64 wt_end_insert;


static void select_sort(char *ret){
	if(flagWriteText == 0){
		ret = "Chua nhap du lieu !!!!!\n";
	}
	else{
		flagBubbleSort = 1;
		int len = strlen(text_bubble);
		int len_i = len-1;
		int i,j;
		int tmp=0;
		int min = 0;
		int index=0;
		for(i=0;i<len_i;i++){
			min = text_bubble[i];
			for(j=i+1;j<len;j++){
				if(min>text_bubble[j]){
					min = text_bubble[j];
					index = j;
				}
			}
			if(min<text_bubble[i]){
				tmp = text_bubble[i];
				text_bubble[i] = text_bubble[index];
				text_bubble[index] = tmp;
			}
		}
		text_bubble[len] = '\0';
		sprintf(ret,"%s",text_bubble);
		unsigned long later = jiffies + 20;
		while(jiffies <= later);
		//printk("In Bubble Sort : %s",ret);
	}
}

static void insert_sort(char *ret){
	if(flagWriteText == 0){
		ret = "Chua nhap du lieu !!!!\n";
	}
	else{
		flagInsertSort = 1;
		int len = strlen(text_insert);
		int i,j;
		int tmp;
		for(i=1;i<len;i++){
			if(text_insert[i]<text_insert[i-1]){
				j=i-1;
				tmp = text_insert[i];
				while(j>=0 && text_insert[j]>tmp){
					text_insert[j+1]=text_insert[j];
					j-=1;
				}
				text_insert[j+1] = tmp;
			}
		}
		text_insert[len] = '\0';
		sprintf(ret,"%s",text_insert);
		unsigned long later = jiffies + 15;
		while(jiffies <= later);
		//printk("In Insert Sort : %s",ret);
	}
	
}

static void showTime(char *ret){
	if(flagBubbleSort == 0){
		ret = "Chua thuc hien select sort !!!!\n";
	}
	else if(flagInsertSort == 0){
		ret = "Chua thuc hien insert sort !!!!\n";
	}
	else{
		long wt_bubble = wt_end_bubble.tv_nsec - wt_start_bubble.tv_nsec;
		long wt_insert = wt_end_insert.tv_nsec - wt_start_insert.tv_nsec;
		if(wt_bubble < 0) wt_bubble*=-1;
		if(wt_insert < 0) wt_insert*=-1;
		sprintf(ret,"Time select sort : %ld (nanoseconds)\nTime insert sort : %ld (nanoseconds)",wt_bubble,wt_insert);
	}
}

static long device_ioctl(struct file *filp,unsigned int cmd,unsigned long arg){
	char *ret = (char *)kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
	int retrn = 0;
	switch(cmd){
	
		case WRITE_TEXT:
			copy_from_user(text_bubble,(char *)arg,MEM_SIZE);
			copy_from_user(text_insert,(char *)arg,MEM_SIZE);
			flagWriteText = 1;
			printk("text : %s",text_bubble);
			printk("");
			break;
		case SELECT_SORT:
			ktime_get_coarse_real_ts64(&wt_start_bubble);
			select_sort(kernel_buf);
			ktime_get_coarse_real_ts64(&wt_end_bubble);
			printk("SELECT_SORT : %s",kernel_buf);
			printk("");
			copy_to_user((char *)arg,kernel_buf,MEM_SIZE);
			break;
		case INSERT_SORT:
			ktime_get_coarse_real_ts64(&wt_start_insert);
			insert_sort(kernel_buf);
			ktime_get_coarse_real_ts64(&wt_end_insert);
			printk("INSERT_SORT : %s",kernel_buf);
			printk("");
			copy_to_user((char *)arg,kernel_buf,MEM_SIZE);
			break;
		case SHOW_TIME:
			showTime(kernel_buf);
			printk(kernel_buf);
			copy_to_user((char *)arg,kernel_buf,MEM_SIZE);
			break;
		
	}
	kfree(ret);
	return retrn;
}

static int dev_open(struct inode *node,struct file *filp){
	printk("Open device file !!!");
	printk("");
	return 0;
}

static int dev_release(struct inode *node,struct file *filp){
	printk("Release device file !!!!");
	printk("");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.unlocked_ioctl = device_ioctl
};

static int __init init_lab(void){
	if(alloc_chrdev_region(&dev_num,0,1,"device_number_Kiem_Tra")<0){
		printk("Cap phat device number khong thanh cong !!!");
		return -1;
	}
	printk("Cap phat device number thanh cong !!!!");
	printk("Major Number : %d",MAJOR(dev_num));
	printk("Minor Number : %d",MINOR(dev_num));
	
	if((dev_class=class_create(THIS_MODULE,"device_class_Kiem_Tra"))==NULL){
		printk("Tao device class khong thanh cong !!!!");
		goto failed_create_class;
	}
	
	if(device_create(dev_class,NULL,dev_num,NULL,"KiemTra")== NULL){
		printk("Tao device file khong thanh cong !!!");
		goto failed_create_device;
	}

	if((kernel_buf = kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL)) == 0){
		printk("Tao kernel buffer khong thanh cong !!!");
		goto failed_create_kernel_buf;
	}

	if((dev_cdev = cdev_alloc()) == NULL){
		printk("Tao cdev khong thanh cong !!!");
		goto failed_create_cdev;
	}

	cdev_init(dev_cdev,&fops);

	if(cdev_add(dev_cdev,dev_num,1) < 0){
		printk("Dang ky cdev khong thanh cong !!!");
		goto failed_add_cdev;
	}
	printk("Init module !!!!");
	printk("");
	return 0;
	
	failed_add_cdev:
		cdev_del(dev_cdev);
	failed_create_cdev:
		kfree(kernel_buf);
	failed_create_kernel_buf:
		device_destroy(dev_class,dev_num);
	failed_create_device:
		class_destroy(dev_class);
	failed_create_class:
		unregister_chrdev_region(dev_num,1);
	flag = 1;
	return 0;
}

static void __exit exit_lab(void){
	if(flag == 0){
		cdev_del(dev_cdev);
		kfree(kernel_buf);
		device_destroy(dev_class,dev_num);
		class_destroy(dev_class);
		unregister_chrdev_region(dev_num,1);
	}
	printk("Exit module !!!");
	printk("");	
}

module_init(init_lab);
module_exit(exit_lab);

MODULE_LICENSE("GPL");
