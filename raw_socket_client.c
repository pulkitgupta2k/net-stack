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

void parseProtocol(struct fileProtocolWrapper *wrapper, BYTE data[]){
	addByte(data, wrapper->dstMac, 0, 6);
	addByte(data, wrapper->srcMac, 6, 6);
	addByte(data, wrapper->eType, 12, 2);
	addByte(data, (BYTE*) &wrapper->fileData->seq, 14, 5);
	addByte(data, (BYTE*) &wrapper->fileData->isEnd, 19, 1);
	addByte(data, wrapper->fileData->data, 20, 130);
}

void readFile(char fileName[], BYTE data[]){
	FILE *fp;
	fp = fopen(fileName, "rb");
	char ch;
	if (fp == NULL){
		perror("ERROR while opening the file \n");
		exit(EXIT_FAILURE);
	}
	int i=0;
	memset(data, 0, 150);
	while((ch = fgetc(fp)) != EOF)
		data[i++] = (BYTE)ch;
	data[i] = 0x00;
	fclose(fp);
	return;
}

void senderWrapper(int sock, struct fileProtocolWrapper *fpw, char fileName[]){
	FILE *file = NULL;
	BYTE data[150];
	size_t bytesRead=0;	

	file = fopen(fileName, "rb");

	if (file != NULL){
		while( (bytesRead = fread(fpw->fileData->data, 1,130, file)) >0 ){
			fpw->fileData->seq++;
			parseProtocol(fpw, data);
			write(sock, data, 150);
		}	
	}
	fclose(file);
	return;
}

int main(int argc, char ** argv){
	int sock_fd =0;
	struct fileProtocol fp;

	fp.seq = 0;
	fp.isEnd = 1;
	//readFile("test.txt", fp.data);	
	
	struct fileProtocolWrapper fpw ={ {0x08, 0x00, 0x27, 0x5c, 0x65, 0x26}, 
					{0x08, 0x00, 0x27, 0x5c, 0x65, 0x26}, 
					{0x88, 0x98}, 
					&fp};
	
	//BYTE data[150];
	//parseProtocol(&fpw, data);

	sock_fd = create_socket(argv[1]);
	if( !(sock_fd)){
		printf("No SOCK FOUND\n");
		return 0;
	}
	senderWrapper(sock_fd, &fpw, "test.txt");
	//write(sock_fd, data, 150);
}
