#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAGIC_NUMBER	234
#define WRITE_TEXT		_IOWR(MAGIC_NUMBER,0,char *)
#define SELECT_SORT	 	_IOWR(MAGIC_NUMBER,1,char *)
#define INSERT_SORT		_IOWR(MAGIC_NUMBER,2,char *)
#define SHOW_TIME		_IOWR(MAGIC_NUMBER,3,char *)

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
	printf("Nhap xau : ");
	char data[1024];
	__fpurge(stdin);
	scanf("%[^\n]%*c",data);
	int ret = ioctl(fd,WRITE_TEXT,data);
	if(ret < 0){
		printf("Ghi du lieu khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi du lieu thanh cong !!!\n");
	}
}


void selectSort(int fd){
	char *ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,SELECT_SORT,ret);
	if(f<0){
		printf("Loi !!!!\n");
	}else{
		printf("Sap xep chon : ");
		printf("%s\n",ret);
	}
	free(ret);
}

void insertSort(int fd){
	char *ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,INSERT_SORT,ret);
		if(f<0){
		printf("Loi !!!!\n");
	}else{
		printf("Sap xep chen : ");
		printf("%s\n",ret);
	}
	free(ret);
}

void showTime(int fd){
	char *ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,SHOW_TIME,ret);
	if(f<0){
		printf("Loi !!!!\n");
	}else{
		printf("So sanh thoi gian :\n");
		printf("%s\n",ret);
	}
	free(ret);
}

void closeDeviceFile(int fd){
	close(fd);
}

void menu(int fd){
	int sl;
	while(1){
		printf("************************\n");
		printf("Vui long chon chuc nang:\n");
		printf("1. Nhap mot xau\n");
		printf("2. Sap xep xau theo thuat toan chon\n");
		printf("3. Sap xep xau theo thuat toan chen\n");
		printf("4. So sanh thoi gian thuc thi\n");
		printf("5. Ket thuc\n");
		scanf("%d",&sl);
		switch(sl){
			case 1:
				writeDeviceFile(fd);
				break;
			case 2:
				selectSort(fd);
				break;
			case 3:
				insertSort(fd);
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
