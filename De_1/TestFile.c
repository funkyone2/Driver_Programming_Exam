#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAGIC_NUMBER	234
#define WRITE_TEXT_1		_IOWR(MAGIC_NUMBER,0,char *)
#define WRITE_TEXT_2	 	_IOWR(MAGIC_NUMBER,1,char *)
#define CHUYEN_TEXT_THUONG	_IOWR(MAGIC_NUMBER,2,char *)
#define CHUYEN_TEXT_HOA		_IOWR(MAGIC_NUMBER,3,char *)
#define CHUAN_HOA_TEXT_1	_IOWR(MAGIC_NUMBER,4,char *)
#define NOI_2_TEXT		_IOWR(MAGIC_NUMBER,5,char *)


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
	printf("Nhap xau 1: ");
	char data[1024];
	__fpurge(stdin);
	scanf("%[^\n]%*c",data);
	int ret = ioctl(fd,WRITE_TEXT_1,data);
	if(ret < 0){
		printf("Ghi du lieu khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi du lieu thanh cong !!!\n");
	}
	printf("Nhap xau 2: ");
	char data_2[1024];
	__fpurge(stdin);
	scanf("%[^\n]%*c",data_2);
	ret = ioctl(fd,WRITE_TEXT_2,data_2);
	if(ret < 0){
		printf("Ghi du lieu khong thanh cong !!!!\n");
	}
	else{
		printf("Ghi du lieu thanh cong !!!\n");
	}
}



void chuyenTextThuong(int fd){
	char *temp = (char *)malloc(1024*sizeof(char));
	ioctl(fd,CHUYEN_TEXT_THUONG,temp);
	printf("%s\n",temp);
	free(temp);
}



void chuyenTextHoa(int fd){
	char *ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,CHUYEN_TEXT_HOA,ret);
	printf("%s\n",ret);
	free(ret);
}

void chuanHoa(int fd){
	char *ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,CHUAN_HOA_TEXT_1,ret);
	printf("%s\n",ret);
	free(ret);
}

void noiHaiChuoi(int fd){
	char *ret = (char *)malloc(1024*sizeof(char));
	int f = ioctl(fd,NOI_2_TEXT,ret);
	printf("%s\n",ret);
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
		printf("1. Nhap hai xau\n");
		printf("2. Chuyen xau 1 sang chu thuong\n");
		printf("3. Chuyen xau 2 sang chu hoa\n");
		printf("4. Chuan hoa xau 1\n");
		printf("5. Noi 2 xau\n");
		printf("6. Ket thuc\n");
		scanf("%d",&sl);
		switch(sl){
			case 1:
				writeDeviceFile(fd);
				break;
			case 2:
				chuyenTextThuong(fd);
				break;
			case 3:
				chuyenTextHoa(fd);
				break;
			case 4:
				chuanHoa(fd);
				break;
			case 5:
				noiHaiChuoi(fd);
				break;
			case 6:
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
