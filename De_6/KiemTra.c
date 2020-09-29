#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/timekeeping.h>
#include <linux/delay.h>
#include <linux/string.h>

#define MEM_SIZE	1024
#define MAGIC_NUMBER	234
#define CONVERT_OCT_TO_DEC 	_IOR(MAGIC_NUMBER,0,char *)
#define CONVERT_OCT_TO_BIN 	_IOR(MAGIC_NUMBER,1,char *)
#define SHOW_TIME		_IOR(MAGIC_NUMBER,3,char *)


static dev_t dev_num;
static struct class *dev_class;
static struct cdev *dev_cdev;
static int flag;
static int flagWriteOct = 0;
static int flagConvertOctToDec = 0;
static char *kernel_buf;
static int dec;
static char *oct;
int lenOct;
static unsigned long start;
static unsigned long end;
static struct timespec64 wt_start;
static struct timespec64 wt_end;

static int dev_open(struct inode *,struct file *);
static int dev_release(struct inode *,struct file *);
//static ssize_t dev_read(struct file *,char *,size_t,loff_t *);
static ssize_t dev_write(struct file *,const char *,size_t,loff_t *);

static int pow(int a,int b){
	int val=1;
	int i;
	for(i=1;i<=b;i++){
		val*=a;
	}
	return val;
}

static void convertOctToDec(char *ret){
	if(flagWriteOct == 0){
		ret = "Khong co du lieu !!!!!\n";
	}
	else{
		int i;
		int n = lenOct;
		dec = 0;
		for(i=0;i<n;i++)
		{
			dec += (oct[i]-48)*(pow(8,(n-i-1)));
			//printk("%d - %d",i,bin[i]-48);
		}
	
		printk("Dec : %d",dec);
		sprintf(ret,"%d",dec);
		printk("");
		flagConvertOctToDec = 1;
	}

	
}

static void convertOctToBin(char *ret){
	if(flagWriteOct == 0){
		ret = "Chua co du lieu !!!!!\n";
	}else{
		if(flagConvertOctToDec == 0){
			char *temp = kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
			convertOctToDec(temp);	
			printk("Oct to Dec in Bin !!!!");
		}
		char bin_tmp[32];
		char bin[32];
		int flagCheck = 0;
		int i=0,j;
		for(i=31;i>=0;i--){
			bin_tmp[31-i] = ((dec>>i) & 1) +48;
			
			
		}
		for(i=0;i<32;i++){
			if(flagCheck == 0){
				if(bin_tmp[i] == 48){
					continue;
				}else{
					flagCheck = 1;
					j = 0;
				}
			}
			bin[j] = bin_tmp[i];
			j++;
		}
		bin[j] = '\0';
		sprintf(ret,"%s",bin);
		printk("Bin : %s",ret);
	}
	
}

static char * showTime(void){
	char *val = (char *)kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
	//char *val_1 = (char *)kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
	//struct timespec ts_start;
	//struct timespec	ts_end;
	//struct timeval tv_start;
	//struct timeval tv_end;
	//jiffies_to_timespec(start,&ts_start);
	//jiffies_to_timespec(end-start,&ts_end);
	//long time = ts_end.tv_nsec - ts_start.tv_nsec;
	
	//jiffies_to_timespec(start,&ts_start);
	//jiffies_to_timespec(end,&ts_end);
	//jiffies_to_timeval(start,&tv_start);
	//sprintf(val,"Time start : %ld (s) %ld (nanoseconds)",ts_start.tv_sec,ts_start.tv_nsec);
	//sprintf(val_1,"Time start : %ld (s) %ld (nanoseconds)",tv_start.tv_sec,tv_start.tv_usec);
	//printk("%s",val);
	//printk("%s",val_1);
	
	long wt_sec = wt_end.tv_sec - wt_start.tv_sec;
	long wt_nsec = wt_end.tv_nsec - wt_start.tv_nsec;
	if(wt_nsec < 0) wt_nsec*=-1;
	sprintf(val,"Time : %ld (nanoseconds)",wt_nsec);
	//printk("sec : %ld - nanoseconds : %ld",wt_sec,wt_nsec);
	return val;
}

static long device_ioctl(struct file *filp,unsigned int cmd,unsigned long arg){
	int ret = 0;
	switch(cmd){
		case CONVERT_OCT_TO_DEC:
			convertOctToDec(kernel_buf);
			printk("Oct to Dec : %s",kernel_buf);
			printk("");
			copy_to_user((char *)arg,kernel_buf,MEM_SIZE);
			end = jiffies;
			ktime_get_coarse_real_ts64(&wt_end);
			break;
		case CONVERT_OCT_TO_BIN:
			convertOctToBin(kernel_buf);
			printk("Oct to Bin : %s",kernel_buf);
			printk("");
			copy_to_user((char *)arg,kernel_buf,MEM_SIZE);
			break;
		case SHOW_TIME:
			kernel_buf = showTime();
			copy_to_user((char *)arg,kernel_buf,MEM_SIZE);
			printk("Time : %s",kernel_buf);
			printk("");
			break;
	}
	return ret;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.write = dev_write,
	.unlocked_ioctl = device_ioctl
};

static int dev_open(struct inode *node,struct file *filp){
	start=jiffies;
	ktime_get_coarse_real_ts64(&wt_start);
	printk("Open device file !!!");
	return 0;
}

static int dev_release(struct inode *node,struct file *filp){
	printk("Release device file !!!!");
	return 0;
}


static ssize_t dev_write(struct file *filp,const char *user_buf,size_t len,loff_t *off){
	printk("Write data to device file !!!");
	oct = (char *)kmalloc(len*sizeof(char),GFP_KERNEL);
	copy_from_user(oct,(char *)user_buf,len);
	printk("len : %d",len);
	lenOct = len;
	printk("Oct : %s",oct);
	flagWriteOct = 1;
	printk("");
	return len;
}	

static int __init init_lab(void){
	if(alloc_chrdev_region(&dev_num,0,1,"device_number_KiemTra")<0){
		printk("Cap phat device number khong thanh cong !!!");
		return -1;
	}
	printk("Cap phat device number thanh cong !!!!");
	printk("Major Number : %d",MAJOR(dev_num));
	printk("Minor Number : %d",MINOR(dev_num));
	
	if((dev_class=class_create(THIS_MODULE,"device_class_KiemTra"))==NULL){
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

