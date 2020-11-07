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
	addByte(data, (BYTE*) &wrapper->fileData->isEnd, 18, 1);
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
	while((ch = fgetc(fp)) != EOF)
		data[i++] = (BYTE)ch;
	data[i] = 0x00;
	fclose(fp);
	return;
}


int main(int argc, char ** argv){
	int sock_fd =0;
	struct fileProtocol fp;

	fp.seq = 10000000;
	fp.isEnd = 1;
	readFile("test.txt", fp.data);	
	
	struct fileProtocolWrapper fpw ={ {0x08, 0x00, 0x27, 0xc5, 0x0d, 0x1c}, 
					{0x08, 0x00, 0x27, 0x5c, 0x65, 0x26}, 
					{0x88, 0x98}, 
					&fp};
	
	BYTE data[150];
	parseProtocol(&fpw, data);

	//addByte(data, dst_mac, 0, 6);
	//addByte(data, src_mac, 6, 6);
	//addByte(data, type, 12, 2);
	//addByte(data, fileData,14, 130);
	//strncat(data, dst_mac, 6);
	//strncat(data, src_mac, 6);
	//strncat(data, dst_mac, 2);
	//BYTE data[150] = {0x08, 0x00, 0x27, 0x5c, 0x65, 0x26, 0x08, 0x00, 0x27, 0xc5, 0x0d, 0x1c, 0x88, 0x98};
	sock_fd = create_socket(argv[1]);
	if( !(sock_fd)){
		printf("No SOCK FOUND\n");
		return 0;
	}
	write(sock_fd, data, 150);
}
