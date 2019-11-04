#include "main.h"

sqlite3 *conn;
sqlite3_stmt *statement;

SKMIB_t skmib;

void init_sqlite3_param(){
	int ret = 0;
	char *err_msg = 0;
	char *sql = NULL;
	ret = sqlite3_open("sk.sqlite3", &conn);

	printf("init SQLite3 \n");

	if(ret != SQLITE_OK){
		printf("Can not open Database \n");
		return ;
	}

	printf("skmib %d, %d \n", skmib._neighbor, skmib._neighborAuth);

	if(skmib._neighbor == 1 || skmib._neighborAuth == 1){
		sql = "CREATE TABLE Param(dev_id INTEGER PRIMARY KEY, P1 TEXT, P2 TEXT, PUB TEXT, Da TEXT);" ;
	}else sql = "CREATE TABLE Param(dev_id INTEGER PRIMARY KEY, P1 TEXT, P2 TEXT, s TEXT);" ;
	
	printf("sql: %s \n",sql);

	 ret = sqlite3_exec(conn, sql, 0, 0, &err_msg);
	 
	 if (ret != SQLITE_OK ) {
		 fprintf(stderr, "SQL error: %s\n", err_msg);
	 }

	 return ;
    
}

void init_sqlite3(){
	int ret = 0;
	char *err_msg = 0;

	ret = sqlite3_open("sk.sqlite3", &conn);

	printf("init SQLite3 \n");

	if(ret != SQLITE_OK){
		printf("Can not open Database \n");
		return ;
	}

	char *sql = "CREATE TABLE Neighbors(dev_id INTEGER PRIMARY KEY, mac TEXT, uuid TEXT, name TEXT, Da TEXT);" ;
	
	 ret = sqlite3_exec(conn, sql, 0, 0, &err_msg);
	 
	 if (ret != SQLITE_OK ) {
		 fprintf(stderr, "SQL error: %s\n", err_msg);
	 }

	 return ;
    
}

void insert_param_neighbor(int dev_num, unsigned char* P1, unsigned char* P2, unsigned char* PUB, unsigned char* Da){

	int ret = 0;
	char *err_msg = 0;

	char* sql = sqlite3_mprintf("INSERT OR REPLACE INTO param(dev_id, P1, P2, PUB, Da) VALUES ('%d', '%q', '%q', '%q', '%q');", dev_num, P1, P2, PUB, Da);

	//printf("%s %s %s param \n", P1, P2, s);

	ret = sqlite3_exec(conn, sql, 0, 0, &err_msg);

	if(ret != SQLITE_OK ) {
		fprintf(stderr, "Failed to create table\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
	} else {
		fprintf(stdout, "Insert param successfully\n");
	}
}
void insert_param(int dev_num, unsigned char* P1, unsigned char* P2, unsigned char* s){

	int ret = 0;
	char *err_msg = 0;

	char* sql = sqlite3_mprintf("INSERT OR REPLACE INTO param(dev_id, P1, P2, s) VALUES ('%d', '%q', '%q', '%q');", dev_num, P1, P2, s);

	//printf("%s %s %s param \n", P1, P2, s);

	ret = sqlite3_exec(conn, sql, 0, 0, &err_msg);

	if(ret != SQLITE_OK ) {
		fprintf(stderr, "Failed to create table\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
	} else {
		fprintf(stdout, "Insert param successfully\n");
	}
}

void insert_Da(int dev_num, char* Da){
	int ret = 0;
	char* err_msg = 0;

	char* sql = sqlite3_mprintf("UPDATE Neighbors set Da ='%q' where dev_id = '%d';", Da, dev_num);;

	ret = sqlite3_exec(conn, sql, 0, 0, &err_msg);

	if(ret != SQLITE_OK ) {
		fprintf(stderr, "Failed to create table\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
	} else {
		fprintf(stdout, "Insert Neighbor successfully\n");
	}
}

void insert_neighbor(int dev_num, char* mac, char* uuid, char* name){

	int ret = 0;
	char *err_msg = 0;
	
	/*
	char *sql = "INSERT INTO Neighbors VALUES ('Tom');";
	
	char *sql = "INSERT INTO Neighbors (Id, mac, uuid, name) VALUES ('tez', 'tez@tez.kr') ON DUPLICATE KEY UPDATE mac='tez', uuid='tezpark@tez.kr', name=''";
	*/
	//char* sql = sqlite3_mprintf("INSERT INTO Neighbors (dev_id, mac, uuid, name, Da) VALUES ('%d', '%q', '%q', '%q', '%q');",dev_num, mac, uuid, name, mac);

	//char* sql = sqlite3_mprintf("", s);
	
	char* sql = sqlite3_mprintf("INSERT OR REPLACE INTO Neighbors(dev_id, mac, uuid, name) VALUES ('%d', '%q', '%q', '%q');", dev_num, mac, uuid, name);

	ret = sqlite3_exec(conn, sql, 0, 0, &err_msg);
	
	if(ret != SQLITE_OK ) {
		fprintf(stderr, "Failed to create table\n");
		fprintf(stderr, "SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
	} else {
		fprintf(stdout, "Insert Neighbor successfully\n");
	}
}

int callback(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    for (int i = 0; i < argc; i++) {

        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    
    printf("%d\n",argc);
    
    return 0;
}

int select_neighborByMAC(char* MAC){
	char* sql = sqlite3_mprintf("select * from Neighbors where mac = '%q';", MAC);

	char **result;
	char *err_msg;
	int row = 0;
	int col = 0;
	int ret = 0;
	int i = 0;
	int j = 0;

	ret = sqlite3_get_table(conn, sql, &result, &row, &col, &err_msg);

	if(ret != SQLITE_OK) printf("Fail Get Table Info!! \r\n");

	printf("Sqlite3 Select Result Row:%d, Col:%d\n",row,col);
/*
	if(row >0){
		for(i = 0; i <= row; i++){
			printf("%s, %s, %s, %s, %s \n",
				result[i*col],
				result[i*col + 1],
				result[i*col + 2],
				result[i*col + 3],
				result[i*col + 4]);
//		printf("%d, %d, %d\n", strlen(result[i*col]), strlen(result[i*col+1]), strlen(result[i*col+2]));
		}
	}
	*/
	return row;

}

unsigned char* select_neighborList(int* len){
	char *sql = "SELECT * FROM Neighbors";

	char **result;
	char *err_msg;
	int row = 0;
	int col = 0;
	int ret = 0;
	int i = 0;
	int j = 0;

	unsigned char* data = NULL;
	int dataLen = 0;

	/*
	ret = sqlite3_exec(conn, sql, callback, 0, &err_msg);

    	if (ret != SQLITE_OK ) {

	        fprintf(stderr, "Failed to select data\n");
        	fprintf(stderr, "SQL error: %s\n", err_msg);

	        sqlite3_free(err_msg);

	        return ;
    	}
	*/

	ret = sqlite3_get_table(conn, sql, &result, &row, &col, &err_msg);

	if(ret != SQLITE_OK) printf("Fail Get Table Info!! \r\n");

	printf("Sqlite3 Select Result Row:%d, Col:%d\n",row,col);
/*
	for(i = 0; i <= row; i++){
		printf("%s, %s, %s, %s, %s \n",
				result[i*col],
				result[i*col + 1],
				result[i*col + 2],
				result[i*col + 3],
				result[i*col + 4]);
//		printf("%d, %d, %d\n", strlen(result[i*col]), strlen(result[i*col+1]), strlen(result[i*col+2]));
	}
*/
        data = enc_setup_neighborList(row, col, result, &dataLen);

	*len = dataLen;
	//client_send(data, dataLen);

	sqlite3_free_table(result);

	return data;

}

void select_param(){
	char *sql = "SELECT * FROM param";
	
	char **result;
	char *err_msg;
	int row = 0;
	int col = 0;
	int ret = 0;
	int i = 0;
	int j = 0;

	ret = sqlite3_get_table(conn, sql, &result, &row, &col, &err_msg);

	if(ret != SQLITE_OK) printf("Fail Get Table Info!! \r\n");

	printf("Sqlite3 Select Result Row:%d, Col:%d\n",row,col);
/*
	for(i = 0; i <= row; i++){
		printf("%d, %s, %s, %s\n",
				result[i*col],
				result[i*col + 1],
				result[i*col + 2],
				result[i*col + 3]);
	}
*/
	if(skmib._neighborAuth == 1) _sk_setup_load_neighbor(row, col, result);
	if(skmib._userAuth == 1) _sk_setup_load(row, col, result);
	sqlite3_free_table(result);

}

void select_neighbor(){

}

void _sqlite3_close(){

	sqlite3_close(conn);
}
