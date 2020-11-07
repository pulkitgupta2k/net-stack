#define ETHER_TYPE 0x8898
typedef unsigned char BYTE;

struct fileProtocol{
         unsigned long long int seq;
         BYTE data[130];
         int isEnd;
};
struct fileProtocolWrapper{
        BYTE dstMac[6];
        BYTE srcMac[6];
        BYTE eType[2];
        struct fileProtocol *fileData;
};

int create_socket(char *device){
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
        int ctr = 0;
        while(ctr!=s)
                dest[i++] = src[ctr++];
}


