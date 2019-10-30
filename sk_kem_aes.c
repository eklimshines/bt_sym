#include "main.h"

unsigned char* aes_hmac_decryption(pairing_t pairing, unsigned char *ID, unsigned char *cipher, element_t Da, element_t Ppub){
	unsigned char *_dec = NULL;
	unsigned char *_U, *_V, *_W, *_H2;

	int uLen, vLen, wLen, dataLen, H2Len;
	int i = 0;

	element_t U, V, W, gp, sigmaP, m, H2;

	mpz_t mpz_sigma, mpz_H2_gp, mpz_V, mpz_W, mpz_M;

	element_init_G1(U, pairing);

	element_init_GT(gp, pairing);

	element_init_Zr(V, pairing);
	element_init_Zr(W, pairing);
	element_init_Zr(sigmaP, pairing);
	element_init_Zr(m, pairing);
	element_init_Zr(H2, pairing);

	mpz_init(mpz_sigma);
	mpz_init(mpz_H2_gp);
	mpz_init(mpz_V);
	mpz_init(mpz_W);
	mpz_init(mpz_M);

	//parsing the CipherData
	if(cipher[i++] == 0x00 && cipher[i++] == 0x02){
		dataLen = cipher[i++] << 8;
		dataLen |= cipher[i++] <<0;

		//printf_byte(cipher, dataLen);

		uLen = cipher[i++] << 8;
		uLen |= cipher[i++] << 0;
		_U = (unsigned char*)malloc(uLen);
		memset(_U, 0, uLen);
		memcpy(_U, &cipher[i], uLen);
		element_from_bytes(U,_U);
		i += uLen;

		//printf("uLen: ");
		//printf_byte(_U, uLen);
		//printf("strlen(_U): ");
		//printf_byte(_U, strlen(_U));

		vLen = cipher[i++] << 8;
		vLen |= cipher[i++] << 0;
		_V = (unsigned char*)malloc(vLen);
		memcpy(_V, &cipher[i], vLen);
		element_from_bytes(V,_V);
		i += vLen;

		//printf("vLen: ");
		//printf_byte(_V, vLen);
		//printf("strlen(_V): ");
		//printf_byte(_V, strlen(_V));
		//
		wLen = cipher[i++] << 8;
		wLen |= cipher[i++] << 0;
		_W = (unsigned char*)malloc(wLen);
		memcpy(_W, &cipher[i], wLen);
		element_from_bytes(W,_W);
		i += wLen;

		//printf("wLen: ");
		//printf_byte(_W, wLen);
		//printf("strlen(_W) : ");
		//printf_byte(_W, strlen(_W));


		//printf("d[%d], u[%d], v[%d], w[%d], cipher[%d]\n",dataLen, uLen, vLen, wLen, strlen(cipher));

		//printf("d[%d], u[%d], v[%d], w[%d]\n",sizeof(cipher), sizeof(_U), sizeof(_V), sizeof(_W));
		if(cipher[i++] == 0x00 && cipher[i++] == 0x03)
			printf("STX Good\n");
	}else {
		printf("Data Parsing Error\n");
		return NULL;
	}

	//element_printf("Dec.Da =%B \n",Da);
	//element_printf("Dec.U =%B \n",U);
	//element_printf("Dec.V =%B \n",V);
	//element_printf("Dec.W =%B \n",W);


	element_to_mpz(mpz_V, V);
	element_to_mpz(mpz_W, W);

	//gmp_printf("mpz_V =%Zd\n",mpz_V);
	//Decryption
	printf("Decrypt. \n");
	element_pairing(gp,U,Da);
	element_printf("g' =%B\n",gp);

	H2Len = element_length_in_bytes(gp);
	_H2 = pbc_malloc(H2Len);
	element_to_bytes(_H2,gp);
	element_from_bytes(H2, _H2);

	element_to_mpz(mpz_H2_gp, H2);
	mpz_xor(mpz_sigma, mpz_V, mpz_H2_gp);
	//gmp_printf("mpz_sigma =%Zd\n",mpz_sigma);
	//gmp_printf("mpz_H2_gp =%Zd\n",mpz_H2_gp);
	element_set_mpz(sigmaP, mpz_sigma);
	element_printf("sigma' =%B\n",sigmaP);

	mpz_xor(mpz_M, mpz_W, mpz_sigma);
	element_set_mpz(m, mpz_M);
	element_printf("m' =%B\n",m);

	element_clear(U);
	element_clear(V);
	element_clear(W);
	element_clear(gp);
	element_clear(sigmaP);
	element_clear(m);
	element_clear(H2);

	free(_H2);

	return _dec;
}

unsigned char* aes_hmac_encryption(pairing_t pairing, unsigned char *ID, element_t P1, element_t P2, element_t Ppub, element_t g){
	unsigned char *_enc;
	unsigned char *_sigma,*_m, *_H3, *_H2;

	int sigmaLen, H3Len, mLen, H2Len;

	element_t sigma, m, IDa, Qa, r, gr, H2, U, V, W;
	element_t temp3;

	mpz_t mpz_sigma,mpz_H2_gr,mpz_V,mpz_W,mpz_m;

	mpz_init(mpz_sigma);
	mpz_init(mpz_H2_gr);
	mpz_init(mpz_V);
	mpz_init(mpz_W);
	mpz_init(mpz_m);

	element_init_G1(Qa, pairing);
	element_init_G1(temp3, pairing);
	element_init_G1(U, pairing);

	element_init_GT(gr, pairing);

	element_init_Zr(sigma, pairing);
	element_init_Zr(m, pairing);
	element_init_Zr(IDa, pairing);
	element_init_Zr(r, pairing);
	element_init_Zr(H2, pairing);
	element_init_Zr(V, pairing);
	element_init_Zr(W, pairing);

	printf("Encrypt. \n");
	element_random(sigma);
	element_printf("Sigma =%B \n",sigma);
	element_random(m);
	element_printf("Message =%B \n",m);

	element_from_hash(IDa, ID, strlen(ID));
	element_mul_zn(temp3, P1, IDa);
	element_add(Qa, temp3, Ppub);
	element_printf("Qa =%B \n",Qa);

	sigmaLen = element_length_in_bytes(sigma);
	mLen = element_length_in_bytes(m);
	H3Len = sigmaLen + mLen;

	_sigma = pbc_malloc(sigmaLen);
	_m = pbc_malloc(mLen);
	_H3 = pbc_malloc(H3Len);

	element_to_bytes(_sigma, sigma);
	element_to_bytes(_m, m);
	concate_byte(_H3, _sigma, sigmaLen, _m, mLen);

	element_from_hash(r, _H3, H3Len);
	element_printf("r =%B \n",r);

	element_pow_zn(gr,g,r);
	element_printf("gr =%B \n",gr);

	H2Len = element_length_in_bytes(gr);
	_H2 = pbc_malloc(H2Len);
	element_to_bytes(_H2,gr);
	//printf("gr_byte[%d]: \n",H2Len);

	element_from_bytes(H2, _H2);

	element_mul_zn(U, Qa, r);
	element_printf("U =%B\n",U); //U

	_enc = (unsigned char*)malloc(sigmaLen+mLen);
	//aes_encrypt(_m, _enc, mLen, _H3,sigmaLen);
	//printf("aes enc[%d]: ",strlen(_enc));
	//printf_byte(_enc, strlen(_enc));

	element_to_mpz(mpz_sigma, sigma);
	element_to_mpz(mpz_H2_gr,H2);
	element_to_mpz(mpz_m,m);

	//gmp_printf("mpz_sigma =%Zd\n",mpz_sigma);
	//gmp_printf("mpz_H2_gr =%Zd\n",mpz_H2_gr);

	mpz_xor(mpz_V,mpz_sigma,mpz_H2_gr);
	element_set_mpz(V,mpz_V); //V
	element_printf("V =%B\n",V);
	//gmp_printf("mpz_V =%Zd\n",mpz_V);

	mpz_xor(mpz_W,mpz_sigma,mpz_m);
	element_set_mpz(W,mpz_W); //W

	element_printf("W =%B\n",W);

	_enc = encData(U, V, W);

	element_clear(sigma);
	element_clear(m);
	element_clear(Qa);
	element_clear(IDa);
	element_clear(r);
	element_clear(gr);
	element_clear(H2);
	element_clear(U);
	element_clear(V);
	element_clear(W);

	element_clear(temp3);

	free(_sigma);
	free(_m);
	free(_H3);
	free(_H2);

	return _enc;
}
