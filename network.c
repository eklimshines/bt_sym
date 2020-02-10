#include "main.h"

uint32_t service_uuid_int[] = { 0, 0, 0, 0xABCD };

SKMIB_t skmib;
CHECK_t auth[8];

void* network_thread(void *arg){
	pthread_t serverid;
	pthread_t clientid;

	int ret = 0;
	int err = 0;

	//parse_role(enc_setup_neighbor());

	if(skmib._main == 1 || skmib._userSetup == 1){
		err = pthread_create(&serverid,NULL,tcp_server_thread,NULL);
		if(err!=0) printf("Create Server Thread Error[%d]\n",err);

		err = pthread_join(serverid,(void**)&ret);
		if(err!=0) printf("Server Return[%d][%d]\n",err,ret);
	}
	if(skmib._neighbor == 1 || skmib._userAuth == 1 || skmib._userAuthReq == 1){
		err = pthread_create(&clientid,NULL,tcp_client_thread,NULL);
		if(err!=0) printf("Create Client Thread Error[%d]\n",err);
		err = pthread_join(clientid,(void**)&ret);
		if(err!=0) printf("Client Return[%d][%d]\n",err,ret);
	}

}
