#include "main.h"

pairing_t pairing;
SKMIB_t skmib;
CONFIG_t cfg;

void arg_parser(int argc, char **argv){
	int i = 0;

	memset(&skmib,0,sizeof(SKMIB_t));

	for(i=0;i<argc;i++){
		if(strcmp(argv[i], "-main") == 0){
			printf("main setting\n");
			skmib._main = 1;
		}else if(strcmp(argv[i], "-user") == 0){
			printf("user Setup setting\n");
			skmib._userSetup = 1;
		}else if(strcmp(argv[i], "-neighbor") == 0){
			printf("neighbor setting\n");
			skmib._neighbor = 1;
		}else if(strcmp(argv[i], "-auth") == 0){
			printf("Auth setting\n");
			skmib._userAuth = 1;
		}else if(strcmp(argv[i], "-neighborauth") == 0){
			printf("Neighbor Auth setting\n");
			skmib._neighborAuth = 1;
		}else if(strcmp(argv[i], "-auth2") == 0){
                        printf("Auth request setting\n");
                        skmib._userAuthReq = 1;
                }
	}

}

int main(int argc, char **argv)
{
    pthread_t bluetoothid;
    pthread_t networkid;
    int ret = 0;
    int err = 0;
    int len = 0;
    //insert_neighbor(6, get_bt_addr(), get_bt_uuid(), get_bt_name());

    //select_neighborList();

    //printf("%s , %s, %s\n",get_bt_addr(), get_bt_name(), get_bt_uuid());

    //printf("%d \n",argc);

    arg_parser(argc, argv);

    config_init(&cfg);

    //Set the paring parameters
    pbc_demo_pairing_init(pairing, argc, argv);
 
    //readParamNeighbor();

    init_sqlite3();
    //init_sqlite3_param();

    if(skmib._userSetup == 1) _sk_setup();
    if(skmib._userAuth == 1) readParamCon(); //select_param(); //_sk_setup_load();
    if(skmib._neighborAuth == 1) readParamNeighbor();

    //_sk_setup_key(_sk_gen_key(cfg.maindevicemac));

    //_sk_kem_dec(_sk_kem_enc(cfg.maindevicemac, &len));


    //printf("After param len:%d\n",len);

    //select_neighborList();
    //sdp_search();

    err = pthread_create(&networkid, NULL, network_thread, NULL);
    if(err!=0) printf("Create Network Thread Error[%d]\n",err);
    
    err = pthread_create(&bluetoothid, NULL, bluetooth_thread, NULL);
    if(err!=0) printf("Create Bluetooth Thread Error[%d]\n",err);

    err = pthread_join(networkid,(void**)&ret);
    if(err!=0) printf("Network Thread Return[%d][%d]\n",err,ret);
    
    err = pthread_join(bluetoothid,(void**)&ret);
    if(err!=0) printf("Bluetooth Thread Return[%d][%d]\n",err,ret);

    _sqlite3_close();

}
