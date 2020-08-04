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

#define DRIVER_AUTHOR	"Pham Hung"
#define DRIVER_DESC	"Kiem Tra"
#define MEM_SIZE	1024
#define MAGIC_NUMBER	234
#define WRITE_KEY 		_IOWR(MAGIC_NUMBER,0,char *)
#define WRITE_PLAIN_TEXT 	_IOWR(MAGIC_NUMBER,1,char *)
#define ENCRYPT_PLAIN_TEXT	_IOWR(MAGIC_NUMBER,2,char *)
#define SHOW_TIME		_IOWR(MAGIC_NUMBER,3,char *)
#define WRITE_LEN_KEY		_IOWR(MAGIC_NUMBER,4,int *)
#define DECRYPT_PLAIN_TEXT	_IOWR(MAGIC_NUMBER,5,char *)

static dev_t dev_num;
static struct class *dev_class;
static struct cdev *dev_cdev;
static int flag;
static char *kernel_buf;

static char encrypt_text[1024];
static char key[1024];
static int lenKey;
static int flag_encrypt = 0,flag_buf=0,flagKey=0;
static char *ret;
static int dev_open(struct inode *,struct file *);
static int dev_release(struct inode *,struct file *);
static unsigned long startTimeEncrypt;
static unsigned long stopTimeEncrypt;
static unsigned long startTimeDecrypt;
static unsigned long stopTimeDecrypt;

static int encrypt(char *ret){
	if(flag_buf == 0){
		char temp[50] = "Khong co du lieu ma hoa !!!";
		sprintf(ret,"%s",temp);
		printk("Khong co du lieu ma hoa !!!");
		return -1;
	}else{
		if(flagKey==0) return 2;
		int len = strlen(kernel_buf);
		int i;
		startTimeEncrypt = jiffies;
		for(i=0;i<len;i++){
			encrypt_text[i] = kernel_buf[i]^key[i%lenKey];
		}
		stopTimeEncrypt  =jiffies;
		flag_encrypt = 1;
		char temp[50] = "Ma hoa thanh cong !!!";
		sprintf(ret,"%s",temp);
		return 0;
	}
}

static int decrypt(char *ret){
	if(flag_buf == 0){
		char str[50] = "Khong co du lieu ma hoa !!!!";
		sprintf(ret,"%s",str);
		printk("Khong co du lieu ma hoa !!!");
		return -1;
	}else{
		if(flag_encrypt==0){
			char str[50] = "Chua ma hoa";
			sprintf(ret,"%s",str);
			printk("Chua ma hoa !!!");
			return -1;
		}
		else{
			
			int len = strlen(kernel_buf);
			int i;
			startTimeDecrypt = jiffies;
			for(i=0;i<len;i++){
				*(ret+i) = encrypt_text[i]^key[i%lenKey];

			}
			stopTimeDecrypt = jiffies;
		}
		
		return 0;
	}
}

static void showTime(char *ret){
	unsigned long timeEncrypt = stopTimeEncrypt - startTimeEncrypt;
	unsigned long timeDecrypt = stopTimeDecrypt - startTimeDecrypt;
	int uSecEncrypt = jiffies_to_usecs(timeEncrypt);
	int uSecDecrypt = jiffies_to_usecs(timeDecrypt);
	sprintf(ret,"Time Encrypt : %ld (microseconds) \n Time Decrypt : %ld (microseconds) \n",timeEncrypt,timeDecrypt);
	
}

static long device_ioctl(struct file *filp,unsigned int cmd,unsigned long arg){
	ret = (char *)kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
	int retrn = 0;
	switch(cmd){
	
		case WRITE_KEY:
			copy_from_user(key,(char *)arg,MEM_SIZE);
			printk("Key : %s",key);
			flagKey = 1;
			printk("");
			break;
		case WRITE_LEN_KEY:
			copy_from_user(&lenKey,(int *)arg,sizeof(int));
			break;
		case WRITE_PLAIN_TEXT:
			printk("Write data to device file !!!");
			kernel_buf = (char *)kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
			copy_from_user(kernel_buf,(char *)arg,MEM_SIZE);
			printk("data input : %s",kernel_buf);
			printk("");
			flag_buf = 1;			
			break;
		case ENCRYPT_PLAIN_TEXT:
			retrn = encrypt(ret);
			printk("Encrypt : %s",encrypt_text);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			printk("");
			break;
		case DECRYPT_PLAIN_TEXT:
			retrn = decrypt(ret);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			printk("decrypt : %s",ret);
			printk("");
			break;
		case SHOW_TIME:
			showTime(ret);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			printk("%s",ret);
			printk("");
			break;
		
	}
	kfree(ret);
	return retrn;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.unlocked_ioctl = device_ioctl
};

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
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

