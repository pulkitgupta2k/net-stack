#include "customProtocol.h"

int dump(BYTE *data, int len, char fileName[]){
	struct fileProtocol fp;
	int fd;
	recvProtocol(data, &fp);
	//FILE *file;
	//file = fopen(fileName, "wb+");
	fd = open(fileName, O_RDWR|O_CREAT, 0777);
	pwrite(fd, &fp.data, sizeof(fp.data), fp.seq*130);
	printf("%d ", fp.seq*130);
	//fclose(file);
	close(fd);
	printf("wrote file\n");
	printf("%d", fp.isEnd);	
	return fp.isEnd;
}

int main(int argc, char ** argv){
	int sock_fd =0;
	BYTE data[150];
	sock_fd = create_socket(argv[1]);
	if( !(sock_fd)){
			printf("No SOCK FOUND\n");
			return 0;
	}

	// wait for start signal
	int startSend = 0;
	while(!startSend){
		int data_len = recv(sock_fd, data, 150, 0);
		struct fileProtocol fp;
		if (data_len>0){
			recvProtocol(data, &fp);
			if((int)fp.seq==0){
				startSend = 1;
			}
		}
	}
	// send ack
	// todo dynamic sender mac
	struct fileProtocol fp;
	fp.isEnd = 1;
	fp.seq = 0;
	struct fileProtocolWrapper fpw ={ {0x08, 0x00, 0x27, 0x5c, 0x65, 0x26}, 
					{0x08, 0x00, 0x27, 0x25, 0x38, 0x55}, 
					{0x88, 0x98}, 
					&fp};
					
	parseProtocol(&fpw, data);
	write(sock_fd, data, 150);	

	// receiving part
	fp.isEnd = 0;
	fp.seq = 0;
	int isEnd = 0;
	while(!isEnd){
		int data_len = recv(sock_fd, data, 150, 0);
		if (data_len>0){
			isEnd = dump(data, data_len, "output.txt");
		}
		else
			printf("ERROR: Not found, %d", data_len);
	}
}
