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

#include "customProtocol.h"

void dump(BYTE *data, int len, char fileName[]){
	struct fileProtocol fp;
	recvProtocol(data, &fp);
	FILE *file;
	file = fopen(fileName, "wb+");
	fwrite(&fp.data, sizeof(fp.data), 1, file);
	fclose(file);
	printf("wrote file\n");
}

int main(int argc, char ** argv){
        int sock_fd =0;
        BYTE data[150];
        sock_fd = create_socket(argv[1]);
        if( !(sock_fd)){
                printf("No SOCK FOUND\n");
                return 0;
        }
	while(1){
		int data_len = recv(sock_fd, data, 150, 0);
		if (data_len>0){
			dump(data, data_len, "output.txt");
		}
		else
			printf("ERROR: Not found, %d", data_len);
	}
        //int data_len =  recv(sock_fd, data, 1500, 0);
	//if (data_len>0)
	//	dump(data, data_len);
}
