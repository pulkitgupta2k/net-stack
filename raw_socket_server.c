#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>      
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<fcntl.h>

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
	//printf("%d", fp.isEnd);	
	return 1;
}

int main(int argc, char ** argv){
        int sock_fd =0;
        BYTE data[150];
        sock_fd = create_socket(argv[1]);
        if( !(sock_fd)){
                printf("No SOCK FOUND\n");
                return 0;
        }
	int flag = 1;
	while(flag){
		int data_len = recv(sock_fd, data, 150, 0);
		if (data_len>0){
			flag = dump(data, data_len, "output.txt");
		}
		else
			printf("ERROR: Not found, %d", data_len);
	}
}
