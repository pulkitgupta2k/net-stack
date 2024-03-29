#include "customProtocol.h"

void senderWrapper(int sock, struct fileProtocolWrapper *fpw, char fileName[]){
	FILE *file = NULL;
	BYTE data[DATA_LEN];
	size_t bytesRead=0;	

	file = fopen(fileName, "rb");

	if (file != NULL){
		memset(fpw->fileData->data, 0, DATA_LEN-20);
		while( (bytesRead = fread(fpw->fileData->data, 1,DATA_LEN-20, file)) >0 ){
			printf("%d ", bytesRead);
			fpw->fileData->seq++;
			parseProtocol(fpw, data);
			write(sock, data, DATA_LEN);
			memset(fpw->fileData->data, 0, DATA_LEN-20);
		}
		fpw->fileData->seq++;
		memset(fpw->fileData->data,0, DATA_LEN-20);
		fpw->fileData->isEnd = 1;
		parseProtocol(fpw, data);
		write(sock, data, DATA_LEN);	
	}
	fclose(file);
	return;
}

int main(int argc, char ** argv){
	
	BYTE srcmac[6];
    BYTE dstmac[6] = {0x08, 0x00, 0x27, 0x25, 0x38, 0x55};
    int mac_resp = getMac(argv[1], srcmac);

	int sock_fd =0;
	struct fileProtocol fp;
	struct fileProtocolWrapper fpw;
    memcpy(fpw.srcMac,srcmac,6);
    memcpy(fpw.dstMac,dstmac, 6);
    memcpy(fpw.eType, (BYTE[])ETHER_TYPE_BYTE, sizeof fpw.eType);
    fpw.fileData = &fp;

	sock_fd = create_socket(argv[1]);

	if( !(sock_fd)){
		printf("No SOCK FOUND\n");
		return 0;
	}
	// send syn signal

	BYTE data[DATA_LEN];
	fp.isEnd = 1;
	fp.seq = 0;
	parseProtocol(&fpw, data);
	write(sock_fd, data, DATA_LEN);	
	// wait for ack signal
	int startSend = 0;
	while(!startSend){
		int data_len = recv(sock_fd, data, DATA_LEN, 0);
		struct fileProtocol fp;
		if (data_len>0){
			recvProtocol(data, &fp);
			if((int)fp.seq==0)
				startSend = 1;
		}
	}
	fp.isEnd = 0;

	senderWrapper(sock_fd, &fpw, "test.txt");
}
