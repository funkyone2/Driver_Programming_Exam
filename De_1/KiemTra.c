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


#define DRIVER_AUTHOR "Anh Quan"
#define DRIVER_DESC	"Kiem Tra"
#define MEM_SIZE 1024
#define MAGIC_NUMBER	234
#define WRITE_TEXT_1		_IOWR(MAGIC_NUMBER,0,char *)
#define WRITE_TEXT_2	 	_IOWR(MAGIC_NUMBER,1,char *)
#define CHUYEN_TEXT_THUONG	_IOWR(MAGIC_NUMBER,2,char *)
#define CHUYEN_TEXT_HOA		_IOWR(MAGIC_NUMBER,3,char *)
#define CHUAN_HOA_TEXT_1	_IOWR(MAGIC_NUMBER,4,char *)
#define NOI_2_TEXT		_IOWR(MAGIC_NUMBER,5,char *)


static dev_t dev_num;
static struct class *dev_class;
static struct cdev *dev_cdev;
static int flag;
static char *kernel_buf;

static char text_1[1024];
static char text_2[1024];

static char * chuanHoaXau(char *str){
	int i;
	char *val;
	int count = strlen(str);
	val = (char *)kmalloc((count+1)*sizeof(char),GFP_KERNEL);
	for(i=0;i<count;i++){
		if(*(str+i)==' '){
			*(val+i)=*(str+i);
			continue;		
		}
		if(*(str+i)>=97){
			*(val+i)=*(str+i)-97+65;
		}else{
			*(val+i)=*(str+i);
		}
	}
	*(val+count) = '\0';
	return val;
}

static char * chuyenChuHoa(char *str){
	int len = strlen(str);
	int i;
	char *val = kmalloc((len+1)*sizeof(char),GFP_KERNEL);
	for(i=0;i<len;i++){
		if(*(str+i)==' '){
			*(val+i)=*(str+i);
			continue;		
		}
		if(*(str+i)>=97){
			*(val+i)=*(str+i)-97+65;
		}else{
			*(val+i)=*(str+i);
		}
	}
	*(val+len+1) = '\0';
	return val;
}

static char * chuyenChuThuong(char *str){
	int len = strlen(str);
	int i;
	char *val = kmalloc((len+1)*sizeof(char),GFP_KERNEL);
	for(i=0;i<len;i++){
		if(*(str+i)==' '){
			*(val+i)=*(str+i);
			continue;		
		}
		if(*(str+i)<=90){
			*(val+i)=*(str+i)-65+97;
		}else{
			*(val+i)=*(str+i);
		}
	}
	*(val+len+1) = '\0';
	return val;
}

static char * appendString(char *str1, char *str2){
	int len_1 = strlen(str1);
	int len_2 = strlen(str2);
	int len_val = len_1 + len_2;
	char *val = (char *)kmalloc(len_val*sizeof(char),GFP_KERNEL);
	sprintf(val,"%s %s",text_1,text_2);
	return val;
}

static long device_ioctl(struct file *filp,unsigned int cmd,unsigned long arg){
	char *ret = (char *)kmalloc(MEM_SIZE*sizeof(char),GFP_KERNEL);
	int retrn = 0;
	switch(cmd){
	
		case WRITE_TEXT_1:
			copy_from_user(text_1,(char *)arg,MEM_SIZE);
			printk("text_1 : %s",text_1);
			printk("");
			break;
		case WRITE_TEXT_2:
			copy_from_user(text_2,(char *)arg,MEM_SIZE);
			printk("text_2 : %s",text_2);
			printk("");
			break;
		case CHUYEN_TEXT_THUONG:
			
			ret = chuyenChuThuong(text_1);
			printk("Chu thuong text_1 : %s",ret);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			break;
		case CHUYEN_TEXT_HOA:
			ret = chuyenChuHoa(text_1);
			printk("Chu hoa text_1 : %s",ret);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			break;
		case CHUAN_HOA_TEXT_1:
			ret = chuanHoaXau(text_1);
			printk("chuan hoa xau : %s",ret);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			break;
		case NOI_2_TEXT:
			ret = appendString(text_1,text_2);
			printk("Noi hai xau : %s",ret);
			copy_to_user((char *)arg,ret,MEM_SIZE);
			printk("");
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
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
