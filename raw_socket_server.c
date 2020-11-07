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
	BYTE wData[150];
	int i;
	for(i=0; data[i+14]!=0x00; i++){
		printf("%x ", data[i+14]);
		wData[i] = data[i+14];
	}
	printf("\n");
	FILE *fp;
	fp = fopen(fileName, "wb+");
	fwrite(&wData, i, 1, fp);
	fclose(fp);
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
		memset(data, 0x00, sizeof(data));
		int data_len = recv(sock_fd, data, 150, 0);
		if (data_len>0)
			dump(data, data_len, "output.txt");
		else
			printf("ERROR: Not found, %d", data_len);
	}
        //int data_len =  recv(sock_fd, data, 1500, 0);
	//if (data_len>0)
	//	dump(data, data_len);
}
