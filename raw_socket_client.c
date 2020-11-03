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

#define ETHER_TYPE 0x8898
typedef unsigned char  BYTE;    /* 8-bit   */

int create_socket(char *device){
	int sock_fd;
	struct ifreq ifr;
	struct sockaddr_ll sll;

	memset(&ifr, 0, sizeof(ifr));
	memset(&sll, 0, sizeof(sll));

	sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	if (sock_fd == 0){
		printf("ERROR CREATING SOCKET...");
		return 0;
	}
	strncpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1){
		printf("FAILED TO LOCATE INTERFACE: %s\n", device);
		return 0;
	}
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);

	if (bind(sock_fd, (struct sockaddr *) &sll, sizeof(sll)) == -1){
		printf("ERROR BINDING\n");
	}
	return sock_fd;
}

void addByte(BYTE dest[], BYTE src[], int i, int s){
	int ctr = 0;
	while(ctr!=s)
		dest[i++] = src[ctr++];
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
	BYTE dst_mac[6] = {0x08, 0x00, 0x27, 0xc5, 0x0d, 0x1c};
	BYTE src_mac[6] = {0x08, 0x00, 0x27, 0x5c, 0x65, 0x26};
	BYTE type[2] = {0x88, 0x98};
	BYTE data[150];
	BYTE fileData[130];
	readFile("test.txt", fileData);
	addByte(data, dst_mac, 0, 6);
	addByte(data, src_mac, 6, 6);
	addByte(data, type, 12, 2);
	addByte(data, fileData,14, 130);
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
