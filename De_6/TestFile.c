#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define MAGIC_NUMBER	234
#define CONVERT_OCT_TO_DEC 	_IOR(MAGIC_NUMBER,0,char *)
#define CONVERT_OCT_TO_BIN 	_IOR(MAGIC_NUMBER,1,char *)
#define SHOW_TIME		_IOR(MAGIC_NUMBER,3,char *)

int openDeviceFile(char *path){
	
	char dev[20] = "/dev/";
	char *ret = strcat(dev,path);
	int fd = open(ret,O_RDWR);
	if(fd<0){
		printf("Khong the mo device file!!!!!\n");
		return fd;
	}
	else{
		printf("Mo device file thanh cong !!!!!\n");
		return fd;
	}
}

void writeDeviceFile(int fd){
	printf("Nhap du lieu Oct: ");
	char data[1024];
	__fpurge(stdin);
	scanf("%s",data);
	int ret = write(fd,data,strlen(data));
	if(ret < 0){
		printf("Ghi du lieu khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi du lieu thanh cong !!!\n");
	}
}

void readDataDec(int fd){
	char buf[1024];
	int ret = ioctl(fd,CONVERT_OCT_TO_DEC,buf);
	if(ret < 0){
		printf("Read data error !!!!\n");
	}
	else{
		printf("Du lieu o dang Dec : %s\n",buf);
	}
}

void readDataBin(int fd){
	char buf[1024];
	int ret = ioctl(fd,CONVERT_OCT_TO_BIN,buf);
	if(ret < 0){
		printf("Read data error !!!!\n");
	}
	else{
		printf("Du lieu o dang Bin : %s\n",buf);
	}
}

void showTime(int fd){
	char buf[1024];
	int ret = ioctl(fd,SHOW_TIME,buf);
	if(ret < 0){
		printf("Read data error !!!!\n");
	}
	else{
		printf("%s\n",buf);
	}
}

void closeDeviceFile(int fd){
	close(fd);
}

void menu(int fd){
	int sl;
	while(1){
		printf("************************\n");
		printf("Vui long chon chuc nang:\n");
		printf("1. Nhap vao so he 8\n");
		printf("2. Chuyen so he 8 sang he 10\n");
		printf("3. Chuyen so he 8 sang he 2\n");
		printf("4. Xem thoi gian chuyen doi\n");
		printf("5. Ket thuc\n");
		scanf("%d",&sl);
		switch(sl){
			case 1:
				writeDeviceFile(fd);
				break;
			case 2:
				readDataDec(fd);
				break;
			case 3:
				readDataBin(fd);
				break;
			case 4:
				showTime(fd);
				break;
			case 5:
				closeDeviceFile(fd);
				return;
			default:
				printf("Ky tu khong hop le !!!!!!");
				break;
		}
	}
}
int main(int argc,char **argv){
	if(argc < 3){
		printf("Vui long truyen tham so theo mau : -f 'Ten file'\n");
		return 0;
	}else{
		if(strcmp(argv[1],"-f") == 0){
			int fd = openDeviceFile(argv[2]);
			printf("%s\n",argv[2]);
			if(fd >= 0){
				menu(fd);
			}
		}else{
			printf("Vui long truyen tham so theo mau : -f 'Ten file'\n");
			return 0;
		}	
	}
	return 0;
}
