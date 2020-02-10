#include "main.h"

bdaddr_t bdaddr_any = { 0, 0, 0, 0, 0, 0 };
bdaddr_t bdaddr_local = { 0, 0, 0, 0xff, 0xff, 0xff };

CONFIG_t cfg;

int sdp_search(int* psm){
    uint8_t svc_uuid_int[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0xAB, 0xCD };
    uuid_t svc_uuid;
    int err;
    bdaddr_t target;
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    sdp_session_t *session = 0;

    printf("search SDP protocol\n");

    //str2ba( "B8:27:EB:BB:62:35", &target );
    str2ba( cfg.maindevicemac, &target);

    // connect to the SDP server running on the remote machine
    session = sdp_connect( BDADDR_ANY, &target, SDP_RETRY_IF_BUSY );

    if(session == NULL) {
	    printf("NULL\n");
	    return -1;
    }
    // specify the UUID of the application we're searching for

    //_str2uuid("00001101-0000-1000-8000-00805F9B34FB",&svc_uuid);
    _str2uuid(cfg.maindeviceuuid, &svc_uuid);

    //sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
    search_list = sdp_list_append( NULL, &svc_uuid );

    // specify that we want a list of all the matching applications' attributes
    uint32_t range = 0x0000ffff;
    attrid_list = sdp_list_append( NULL, &range );

    // get a list of service records that have UUID 0xabcd
    err = sdp_service_search_attr_req( session, search_list, \
            SDP_ATTR_REQ_RANGE, attrid_list, &response_list);
    sdp_list_t *r = response_list;

    if(response_list == NULL) {
	    printf("NULL\n");
	    return -1;
    }
    printf("err: %d\n",err);
    // go through each of the service records
    for (; r; r = r->next ) {
        sdp_record_t *rec = (sdp_record_t*) r->data;
        sdp_list_t *proto_list;

	// get a list of the protocol sequences
        if( sdp_get_access_protos( rec, &proto_list ) == 0 ) {
        sdp_list_t *p = proto_list;

        // go through each protocol sequence
        for( ; p ; p = p->next ) {
            sdp_list_t *pds = (sdp_list_t*)p->data;

            // go through each protocol list of the protocol sequence
            for( ; pds ; pds = pds->next ) {

                // check the protocol attributes
                sdp_data_t *d = (sdp_data_t*)pds->data;
                int proto = 0;
                for( ; d; d = d->next ) {
                    switch( d->dtd ) {
                        case SDP_UUID16:
                        case SDP_UUID32:
                        case SDP_UUID128:
                            proto = sdp_uuid_to_proto( &d->val.uuid );

                            break;
			case SDP_UINT16:
                            if( proto == L2CAP_UUID ) {
                                printf("uint16 l2cap port: %ld\n",d->val.uint16);
				//Connect to Main Device
				*psm = d->val.uint16;
                            }
			    break;
                        case SDP_UINT8:
                            if( proto == RFCOMM_UUID ) {
                                printf("rfcomm channel: %d\n",d->val.int8);
                            }

                            if( proto == L2CAP_UUID ) {
                                printf("l2cap port: %d\n",d->val.int8);
                            }
                            break;
                    }
                }
            }
            sdp_list_free( (sdp_list_t*)p->data, 0 );
        }
        sdp_list_free( proto_list, 0 );

        }

        printf("found service record 0x%x\n", rec->handle);

        sdp_record_free( rec );
    }

    printf("finish sdp search \n");
    sdp_close(session);

    return 1;
}


sdp_session_t *register_service(){
    uint32_t service_uuid_int[] = { 0, 0, 0, 0xABCD };
    uint8_t rfcomm_channel = 17;
    const char *service_name = "SymmTest";
    const char *service_dsc = "Anthentication";
    const char *service_prov = "Symmetric";

    int err = 0;
    uint16_t l2cap_channel_num = 0x1001;
    sdp_session_t *session = 0;

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;
    sdp_list_t *l2cap_list = 0,
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0,
               *access_proto_list = 0;
    sdp_data_t *channel = 0, *psm = 0, *l2cap_channel = 0;

    //sdp_record_t *record = sdp_record_alloc();
    sdp_record_t record = { 0 };

    printf("register sdp service\n");

    //_str2uuid("00001101-0000-1000-8000-00805F9B34FB",&svc_uuid);
    //_str2uuid(cfg.maindeviceuuid, &svc_uuid);
    _str2uuid(cfg.myuuid, &svc_uuid);
    sdp_set_service_id(&record, svc_uuid);

    char str[256] = "";
    sdp_uuid2strn(&svc_uuid, str, 256);
    printf("Registering UUID %s\n", str);

    // set the general service ID
    //sdp_uuid128_create( &svc_uuid, &service_uuid_int );
    //sdp_set_service_id( &record, svc_uuid );

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( &record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_channel = sdp_data_alloc(SDP_UINT16, &l2cap_channel_num);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    sdp_list_append( l2cap_list, l2cap_channel );
    proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos(&record, access_proto_list );

    // set the name, provider, and description
    sdp_set_info_attr(&record, service_name, service_prov, service_dsc);

    // connect to the local SDP server, register the service record, and
    // disconnect
    session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );
    //session = sdp_connect( &bdaddr_any, &bdaddr_local, SDP_RETRY_IF_BUSY );

    err = sdp_record_register(session, &record, 0);

    printf("SDP Register result :%d\n",err);

    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );

    printf("Finish SDP Register\n");
    return session;
}
