#include "main.h"

static int client_sock = -1;
static int client_conn = -1;
static CLTSESSION_t clientsession = 0;

//
CLTSESSION_t client_setsession(CLTSESSION_t session);
CLTSESSION_t client_getsession(void);
SKMIB_t skmib;
CONFIG_t cfg;

void client_setconn(int conn);
int client_getconn(void);

void client_setsock(int sock);
int client_getsock(void);
int client_disconnect(void);
int client_connect(void);
int client_send(uint8_t *data, int length);

void* client_thread(void *arg);

//
CLTSESSION_t client_setsession(CLTSESSION_t session){
    clientsession = session;
    return clientsession;
}

CLTSESSION_t client_getsession(void){
    return clientsession;
}

void client_setconn(int conn){
	client_conn = conn;
}

int client_getconn(void){
	return client_conn;
}

void client_setsock(int sock){
    client_sock = sock;
}

int client_getsock(void){
    return client_sock;
}

int client_disconnect(void){	
	int sock = -1;
	sock = client_getsock();
	if(sock>=0) close(sock);
	client_setsock(-1);
	client_setconn(-1);
	return sock;
}

int client_connect(void){
	int sock = 0;
	int conn = 0;
	int port = 30005;
	int size = MAXNETBUFSIZE;
	struct sockaddr_in sock_addr;
	struct timeval tv;
	//struct ifreq ifr;
	int result = 0;
	//char *addr = "192.168.93.33";
	char *addr = "223.195.37.174";
	char *data;
	int len = -1;

	if(skmib._neighbor == 1){
		addr = cfg.userip;
		port = cfg.userport;
	}else if(skmib._userAuth == 1 || skmib._userAuthReq == 1){
		addr = cfg.maindeviceip;
		port = cfg.maindeviceport;
	}

	printf("Client Aceess Fun \n");
	//clientlte_disconnect();
	sock = socket(PF_INET,SOCK_STREAM,0);

	/* Bind to eth1 interface only - this is a private VLAN */
	/*
    	memset(&ifr, 0, sizeof(ifr));
    	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "eth1");
    	if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0){
        	perror("Server-setsockopt() error for SO_BINDTODEVICE");
        	close(sock);
        	exit(-1);
    	}
	*/
	if(sock>=0){
		setsockopt(sock,SOL_SOCKET,SO_SNDBUF,&size,sizeof(int));
		setsockopt(sock,SOL_SOCKET,SO_RCVBUF,&size,sizeof(int));
		memset(&tv,0,sizeof(tv));
		tv.tv_sec = 0;
		tv.tv_usec = CLIENTRECVTIMEOUT;
		setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
		tv.tv_usec = CLIENTSENDTIMEOUT;
		setsockopt(sock,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv,sizeof(tv));

		memset(&sock_addr,0,sizeof(sock_addr));
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_addr.s_addr = inet_addr(addr);
		sock_addr.sin_port = htons(port);

		conn = connect(sock,(struct sockaddr*)&sock_addr,sizeof(sock_addr));
		printf("Client sock[%d] conn[%d] ip[%s:%d]\n",sock,conn,inet_ntoa(sock_addr.sin_addr),port);
		client_setconn(conn);
	}
	client_setsock(sock);

	sleep(2);

	if(skmib._neighbor == 1){
		sendnRegPacket();
		//data = enc_setup_neighbor(&len);
	}else if(skmib._userAuth == 1){
		//data = enc_auth_to_main(int authNeighborLen, int* neighborNum);
		//data = select_neighborList(&len);
	}else if(skmib._userAuthReq == 1){
		//data = enc_auth_to_main(&len);
		printf("Auth Req :%lf\n",clock());
	}
	//if(len > 0) client_send(data, len);
	//else printf("Client No Data!!!!!!!!!\n");
	return sock;
}

void client_rxthread(void *arg){
	int sock = 0;
	int result = 0; 
	char buf[MAXNETBUFSIZE] = {0};
	int len = 0;
	int i = 0;
	uint32_t timeout = 0;
	int plen, ptr =0;
	printf("Create Client Rx Thread\n");

	while(1){
		sock = client_getsock();
		if(sock>=0){
			memset(buf, 0, sizeof(buf));
			len = recv(sock,buf,sizeof(buf),MSG_NOSIGNAL); //MSG_DONTWAIT MSG_NOSIGNAL
			if(len>0){
				//client_setsession(SESSION_KEEPCFM);
				
				printf_byte(buf, len);
				parse_role(buf);
			} else {
				//client_setsession(SESSION_CLOSE);
				usleep(1000); //1ms
			}
		} else {
			//client_setsession(SESSION_CLOSE);
		}
	}
	printf("Exit Client Rx Thread\n");
}

void* client_thread(void *arg){
	int sock = 0;
	int result = 0;
	char buf[MAXNETBUFSIZE] = {0};
	CLTSESSION_t session;

	printf("Create Client Thread \n");

	while(1){
		session = client_getsession();
		switch(session){
			case SESSION_CLOSE:
				result = client_connect();
				if(result > 0){
					client_setsession(SESSION_CONNECTED);
				}
				break;
			case SESSION_CONNECTED:
				break;
		}
	}
}

int client_send(uint8_t *data, int length)
{
	int sock = 0;
	int conn = 0;
	int result = 0;

	sock = client_getsock();
	conn = client_getconn();

	if((sock>=0) && (conn>=0)){
		result = send(sock,data,length,MSG_NOSIGNAL);
		if(result<0){
			client_disconnect();
		}else printf("[%d]client send success!!\n",length);
	}
	return result;
}

void* tcp_client_thread(void *arg){

	pthread_t clientid;
	pthread_t client_rxid;
	int ret = 0;
	int err = 0;

	err = pthread_create(&clientid,NULL,client_thread,NULL);
	if(err!=0) printf("Create Client Thread Error[%d]\n",err);

	err = pthread_create(&client_rxid,NULL,client_rxthread,NULL);
	if(err!=0) printf("Create Client Thread Error[%d]\n",err);

	err = pthread_join(clientid,(void**)&ret);
	if(err!=0) printf("Client Thread Return[%d][%d]\n",err,ret);

	err = pthread_join(client_rxid,(void**)&ret);
	if(err!=0) printf("Client Thread Return[%d][%d]\n",err,ret);
}
