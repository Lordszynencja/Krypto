#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <unistd.h>

#define keylength 128
#define keysize 16

int readLine(char** line,FILE* f) {
	line[0]=malloc(sizeof(char)*100);
	int line_length=100,i=0;
	fread(line,1,1,f);
	while (line[0][i]!='\n') {
		if (i==line_length) {
			line_length+=100;
			char* newline=malloc(sizeof(char)*line_length);
			if (newline==NULL) return 0;
			strcpy(line[0],newline);
		}
		i++;
		fread(line[0]+i,1,1,f);
	}
	line[0][i]='\0';
	return 1;
}

void writeKey(FILE* f,char* key,char* iv) {
	fwrite(key,1,keysize,f);
	fwrite(iv,1,AES_BLOCK_SIZE,f);
}

void readKey(FILE* f,char** key,char** iv) {
	key[0]=malloc(keysize);
	iv[0]=malloc(AES_BLOCK_SIZE);
	fread(key[0],keysize,1,f);
	fread(iv[0],AES_BLOCK_SIZE,1,f);
}

int readFile(char** c,FILE* f,int isencoded,int* real_l) {
	if (isencoded==1) {
		int i=0;
		char lc;
		int l=0;
		fread(&lc,1,1,f);
		while (lc!='\n') {
			l=l*10+(lc-'0');
			i++;
			fread(&lc,1,1,f);
		}
		real_l[0]=l;
		l=0;
		fread(&lc,1,1,f);
		while (lc!='\n') {
			l=l*10+(lc-'0');
			i++;
			fread(&lc,1,1,f);
		}
		c[0]=malloc(l);
		fread(c[0],1,l,f);
		fclose(f);
		return l;
	}
    int l;
    fseek(f, 0L, SEEK_END);
	l=ftell(f);
    c[0]=malloc(l+1);
    if(c[0]==NULL) return 0;
    rewind(f);
    fread(c[0], 1, l, f);
    fclose(f);
    c[0][l] = '\0';
	return l;
}

int encode(char* plain,char** encoded,char* key,char* iv,int plain_l,int mode) {
	printf("Started encoding\n");
	int enc_l=((plain_l+AES_BLOCK_SIZE-1)/AES_BLOCK_SIZE)*AES_BLOCK_SIZE;
	char* plain2=malloc(enc_l);
	if (plain2==NULL) {
		printf("error\n");
		return 0;
	}
	memset(plain2,0,enc_l);
	int i;
	for (i=0;i<plain_l;i++) plain2[i]=plain[i];
	for (;i<enc_l;i++) plain2[i]=0;
	
	encoded[0]=malloc(sizeof(char)*enc_l);
	if (encoded[0]==NULL) return 0;
	AES_KEY k;
    AES_set_encrypt_key(key, keylength, &k);
	if (mode==1) {
		for (i=0;i<enc_l/16;i++) {
			if (i%256==0) printf ("\r%d/%d",i,enc_l/16);
			AES_cbc_encrypt(plain2+i*16, encoded[0]+i*16, 16, &k, iv, AES_ENCRYPT);
		}
		printf ("\r%d/%d",i,enc_l/16);
	} else if (mode==2) {
		char* buf=malloc(AES_BLOCK_SIZE);
		memset(buf,0,AES_BLOCK_SIZE);
		int num=0;
		for (i=0;i<enc_l/16;i++) {
			if (i%256==0) printf ("\r%d/%d",i,enc_l/16);
			AES_ctr128_encrypt(plain+i*16, encoded[0]+i*16, 16, &k, buf, iv, &num);
		}
		printf ("\r%d/%d",i,enc_l/16);
	}
	return 1;
}

int decode(char* encoded,char** plain,char* key,char* iv,int enc_l,int real_l,int mode) {
	printf("Started decoding\n");
	int i;
	plain[0]=malloc(enc_l);
	if (plain[0]==NULL) return 0;
	AES_KEY k;
	memset(plain[0]+enc_l-16,0,16);
	memset(iv,0,16);
	if (mode==1) {
		AES_set_decrypt_key(key, keylength, &k);
		for (i=0;i<enc_l/16;i++) {
			if (i%256==0) printf ("\r%d/%d",i,enc_l/16);
			AES_cbc_encrypt(encoded+i*16, plain[0]+i*16, 16, &k, iv, AES_DECRYPT);
		}
		printf ("\r%d/%d",i,enc_l/16);
	} else if (mode==2) {
		AES_set_encrypt_key(key, keylength, &k);
		char* buf=malloc(AES_BLOCK_SIZE);
		memset(buf,0,AES_BLOCK_SIZE);
		int num=0;
		for (i=0;i<enc_l/16;i++) {
			if (i%256==0) printf ("\r%d/%d",i,enc_l/16);
			AES_ctr128_encrypt(encoded+i*16, plain[0]+i*16, 16, &k, buf, iv, &num);
		}
		printf ("\r%d/%d",i,enc_l/16);
	}
	return 1;
}

int getKey(FILE* f,int keyID,char* key,char* iv) {
	printf("\nEnter the password: ");
	char* c=malloc(sizeof(char)*16);
	memset(c,0,16);
	int i=0;
	c[i]=getch();
	while (c[i]!=13 && i<16) {
		printf("*");
		i++;
		c[i]=getch();
	}
	printf("\n");
	char* key2=malloc(keysize);
	char* iv2=malloc(AES_BLOCK_SIZE);
	fsetpos(f,(const fpos_t*)(keyID*(AES_BLOCK_SIZE+keysize)));
	fread(key2,1,keysize,f);
	fread(iv2,1,AES_BLOCK_SIZE,f);
	char* iv3=malloc(AES_BLOCK_SIZE);
	char* buf=malloc(16);
	int num=0;
	memset(buf,0,16);
	buf[15]=keyID%256;
	buf[14]=keyID/256;
	AES_KEY k;
	memset(iv3,0,AES_BLOCK_SIZE);
    AES_set_encrypt_key(c, keylength, &k);
	AES_ctr128_encrypt(key2, key, keysize, &k, buf, iv3, &num);
	AES_ctr128_encrypt(iv2, iv, AES_BLOCK_SIZE, &k, buf, iv3, &num);
	printf("Key read\n");
	return 1;
}

int saveKey(FILE* f,int keyID,char* key,char* iv) {
	printf("\nEnter the password: ");
	char* c=malloc(sizeof(char)*17);
	memset(c,0,16);
	int i=0;
	c[i]=getch();
	while (c[i]!=13 && i<16) {
		printf("*");
		i++;
		c[i]=getch();
	}
	printf("\n");
	char* key2=malloc(keysize);
	char* iv2=malloc(AES_BLOCK_SIZE);
	char* iv3=malloc(AES_BLOCK_SIZE);
	char* buf=malloc(16);
	int num=0;
	memset(buf,0,16);
	buf[15]=keyID%256;
	buf[14]=keyID/256;
	AES_KEY k;
	memset(iv3,0,AES_BLOCK_SIZE);
    AES_set_encrypt_key(c, keylength, &k);
	AES_ctr128_encrypt(key, key2, keysize, &k, buf, iv3, &num);
	AES_ctr128_encrypt(iv, iv2, AES_BLOCK_SIZE, &k, buf, iv3, &num);
	fsetpos(f,(const fpos_t*)(keyID*(AES_BLOCK_SIZE+keysize)));
	fwrite(key2,1,keysize,f);
	fwrite(iv2,1,AES_BLOCK_SIZE,f);
	return 1;
}

int main(int argc, char **argv) {
	int mode1=1,mode2=1;
	FILE* keystore;
	FILE* t1;
	FILE* t2;
	int keyID;
	int i,l;
	char* c1;
	char* c2;
	char* key=malloc(keysize);
	char* iv=malloc(AES_BLOCK_SIZE);
	if (argc<4) {
		printf("Nie podano wystarczajaco duzo parametrow,%d\n",argc);
		return 0;
	}
	mode1=argv[1][0]-'0';
	mode2=argv[2][0]-'0';
	keystore=fopen(argv[3],(mode1==3?"wb":"rb"));
	keyID=atoi(argv[4]);
	
	
	if (mode1==1) {
		if (argc<6) {
			printf("Nie podano wystarczajaco duzo parametrow\n");
			return 0;
		}
		t1=fopen(argv[5],"rb");
		printf("mode1=AES ENC\nmode2=%s\nkeystore path=%s\nkeyID=%d\nfile 1=%s\nfile 2=%s\n",(mode2==1?"CBC":(mode2==2?"CTR":"GCM")),argv[3],keyID,argv[5],argv[6]);
		getKey(keystore,keyID,key,iv);
		l=readFile(&c1,t1,0,0);
		encode(c1,&c2,key,iv,l,mode2);
		t2=fopen(argv[6],"wb");
		fprintf(t2,"%d\n%d\n",l,((l+AES_BLOCK_SIZE-1)/AES_BLOCK_SIZE)*AES_BLOCK_SIZE);
		fwrite(c2,1,((l+AES_BLOCK_SIZE-1)/AES_BLOCK_SIZE)*AES_BLOCK_SIZE,t2);
	} else if (mode1==2) {
		if (argc<6) {
			printf("Nie podano wystarczajaco duzo parametrow\n");
			return 0;
		}
		t1=fopen(argv[5],"rb");
		printf("mode1=AES DEC\nmode2=%s\nkeystore path=%s\nkeyID=%d\nfile 1=%s\nfile 2=%s\n",(mode2==1?"CBC":"CTR"),argv[3],keyID,argv[5],argv[6]);
		int rl;
		getKey(keystore,keyID,key,iv);
		l=readFile(&c1,t1,1,&rl);
		decode(c1,&c2,key,iv,l,rl,mode2);
		t2=fopen(argv[6],"wb");
		fwrite(c2,1,rl,t2);
	} else if (mode1==3) {
		printf("mode1=CREATE KEYSTORE\nkeystore path=%s\n",argv[3]);
		memset(key,0,keysize);
		memset(iv,0,AES_BLOCK_SIZE);
		RAND_bytes(key, keysize);
		RAND_bytes(iv, AES_BLOCK_SIZE);
		saveKey(keystore, keyID, key, iv);
	}
    return 0;
}