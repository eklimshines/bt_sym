#include "main.h"

int aes_encrypt(unsigned char* in, unsigned char* out, int size, unsigned char* key, int KEY_BIT){
	AES_KEY aes_key;
	unsigned char iv_aes[AES_BLOCK_SIZE];

	bzero(iv_aes,sizeof(iv_aes));
	AES_set_encrypt_key(key, KEY_BIT,&aes_key);
	AES_cbc_encrypt(in,out,size, &aes_key, iv_aes, AES_ENCRYPT);

	return 0;
}

int aes_decrypt(unsigned char* in, unsigned char* out, int size, unsigned char* key, int KEY_BIT){
	AES_KEY aes_key;
	unsigned char iv_aes[AES_BLOCK_SIZE];

        bzero(iv_aes,sizeof(iv_aes));
	AES_set_decrypt_key(key, KEY_BIT,&aes_key);
        AES_cbc_encrypt(in,out,size, &aes_key, iv_aes, AES_DECRYPT);
}

unsigned char* HMAC_SHA512(unsigned char *key, int keyLen,  unsigned char *data, unsigned int *len2){
        unsigned char* result;
        unsigned int len = 64; //sha1=20 sha256=32
        HMAC_CTX *ctx = HMAC_CTX_new();

	printf("HMAC_SHA512\n");

	//ctx = EVP_CIPHER_CTX_new();
	//if(ctx == NULL){EVP_MD_CTX_free(ctx);}

        *len2 = len;
        //HMAC_CTX_init(ctx);

        result = (unsigned char*)malloc(sizeof(char) * len);
        // Using sha1 hash engine here.
        // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
        HMAC_Init_ex(ctx, key, keyLen, EVP_sha512(), NULL);
        HMAC_Update(ctx, (unsigned char*)&data, strlen(data));
        HMAC_Final(ctx, result, &len);
        //HMAC_CTX_cleanup(ctx);
	HMAC_CTX_free(ctx);

        return result;
}


unsigned char* HMAC_SHA1(unsigned char *key, unsigned char *data, unsigned int *len2){

	unsigned char* digest;
	char mdString[20];
	int i = 0;

	digest = HMAC(EVP_sha1(), key, strlen(key), (unsigned char*)data, strlen(data), NULL, NULL);

    	for(i = 0; i < 20; i++)
         	sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    	printf("HMAC digest: %s\n", mdString);

	return digest;
}
