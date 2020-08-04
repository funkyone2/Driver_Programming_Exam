#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAGIC_NUMBER	234
#define WRITE_KEY 		_IOWR(MAGIC_NUMBER,0,char *)
#define WRITE_PLAIN_TEXT 	_IOWR(MAGIC_NUMBER,1,char *)
#define ENCRYPT_PLAIN_TEXT	_IOWR(MAGIC_NUMBER,2,char *)
#define SHOW_TIME		_IOWR(MAGIC_NUMBER,3,char *)
#define WRITE_LEN_KEY		_IOWR(MAGIC_NUMBER,4,int *)
#define DECRYPT_PLAIN_TEXT	_IOWR(MAGIC_NUMBER,5,char *)

char *ret;

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
	printf("Nhap du lieu : ");
	char data[1024];
	__fpurge(stdin);
	scanf("%[^\n]%*c",data);
	int ret = ioctl(fd,WRITE_PLAIN_TEXT,data);
	if(ret < 0){
		printf("Ghi du lieu khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi du lieu thanh cong !!!\n");
	}
	printf("Nhap key : ");
	char key[1024];
	__fpurge(stdin);
	scanf("%[^\n]%*c",key);
	ret = ioctl(fd,WRITE_KEY,key);
	if(ret < 0){
		printf("Ghi key khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi key thanh cong !!!\n");
	}
	int len = strlen(key);
	ret = ioctl(fd,WRITE_LEN_KEY,&len);
	if(ret < 0){
		printf("Ghi len key khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi len key thanh cong !!!\n");
	}
}



void maHoaThayThe(int fd){
	char *temp = (char *)malloc(1024*sizeof(char));
	ioctl(fd,ENCRYPT_PLAIN_TEXT,temp);
	printf("%s\n",temp);
	free(temp);
}




void giaiMaThayThe(int fd){
	ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,DECRYPT_PLAIN_TEXT,ret);
	if(f == 0){
		printf("Plain Text : %s\n",ret);
	}else{
		printf("%s\n",ret);
	}
	free(ret);
}

void showTime(int fd){
	ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,SHOW_TIME,ret);
	if(f == 0){
		printf("Plain Text : %s\n",ret);
	}else{
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
		printf("1. Nhap xau ro va khoa\n");
		printf("2. Ma hoa xau\n");
		printf("3. Giai ma xau\n");
		printf("4. Xem thoi gian ma hoa va giai ma\n");
		printf("5. Ket thuc\n");
		scanf("%d",&sl);
		switch(sl){
			case 1:
				writeDeviceFile(fd);
				break;
			case 2:
				maHoaThayThe(fd);
				break;
			case 3:
				giaiMaThayThe(fd);
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
