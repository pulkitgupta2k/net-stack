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
#include <fcntl.h>

#define ETHER_TYPE 0x8898  // Ethernet Type for custom protocol
#define lli unsigned long long int
typedef unsigned char BYTE;


struct fileProtocol{
	// L3 fragment
	lli seq;
	BYTE data[130];
	int isEnd;
};
struct fileProtocolWrapper{
	//L2 fragment
	BYTE dstMac[6];
	BYTE srcMac[6];
	BYTE eType[2];
	struct fileProtocol *fileData;
};

void recvProtocol(BYTE *data, struct fileProtocol *fp){
	// Adds raw data from wire in stucture
	memcpy(&fp->seq, data+14, 5);
	memcpy(&fp->isEnd, data+19, 1);
	memcpy(&fp->data, data+20, sizeof(fp->data));
};

int create_socket(char *device){
	// Creates a network socket
	int sock_fd;
	struct ifreq ifr;
	struct sockaddr_ll sll;
	memset(&ifr, 0, sizeof(ifr));
	memset(&sll, 0, sizeof(sll));

	sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETHER_TYPE));

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
	sll.sll_protocol = htons(ETHER_TYPE);

	if (bind(sock_fd, (struct sockaddr *) &sll, sizeof(sll)) == -1){
		printf("ERROR BINDING\n");
	}
	return sock_fd;
}

void addByte(BYTE dest[], BYTE src[], int i, int s){
	// adds entire src into dest from index i -> s
	int ctr = 0;
	while(ctr!=s)
		dest[i++] = src[ctr++];
}


void parseProtocol(struct fileProtocolWrapper *wrapper, BYTE data[]){
	// changes struct to data
	addByte(data, wrapper->dstMac, 0, 6);
	addByte(data, wrapper->srcMac, 6, 6);
	addByte(data, wrapper->eType, 12, 2);
	addByte(data, (BYTE*) &wrapper->fileData->seq, 14, 5);
	addByte(data, (BYTE*) &wrapper->fileData->isEnd, 19, 1);
	addByte(data, wrapper->fileData->data, 20, 130);
}
