#include <stdio.h>
#define dl_krypt 39

int isinrng(char c) {
	if (c>=32 && c<=127) {
		if (c>='A' && c<='Z') return 1;
		if (c>='a' && c<='z') return 2;
		if (c==' ') return 3;
		if (c>='0' && c<='9') return 4;
		return 5;
	}
	return 0;
}

int main(int argc,char** argv) {
	char sk[dl_krypt];
	char krypt[21][dl_krypt];
	int keys[128][2];
	int h,i,j,l,k=0,rzad=0;
	char c,c1,c2;
	FILE* file=fopen("a.txt","r");
	for (j=0;j<21;j++) {//loading
		c=fgetc(file);
		l=0;
		i=0;
		while (c!=':') {
			if (l<dl_krypt) {
				if (c=='1') {
					krypt[j][l]=(krypt[j][l]<<1)+1;
					i++;
					if (i%8==0) l++;
				} else if (c=='0') {
					krypt[j][l]=krypt[j][l]<<1;
					i++;
					if (i%8==0) l++;
				}
			}
			c=fgetc(file);
		}
	}
	fclose(file);
	int odpowiedz=0;
	
	for (rzad=0;rzad<dl_krypt;rzad++) {
		int minliter=21;
		do {
			if (odpowiedz==256) {
				minliter--;
			} else if (odpowiedz<0) {
				rzad=rzad+odpowiedz;
				if (rzad<0) rzad=0;
				minliter=21;
				odpowiedz=258;
			} else if (odpowiedz==257) {
				minliter++;
			}
			int minkey,maxkey;
			if (krypt[0][rzad]<0) {
				minkey=128;
				maxkey=256;
			} else {
				minkey=0;
				maxkey=128;
			}
			k=0;
			while (k==0) {
				for (i=minkey;i<maxkey;i++) {//searching possible keys
					int litery=0;
					int porazki=0;
					for (j=0;j<21;j++) {
						c1=krypt[j][rzad];
						c2=i;
						c1=c1^c2;
						int r=isinrng(c1);
						if (r==1 || r==2 || r==3) {
							litery++;
						}
						if (r==0) {
							porazki++;
						} else if (j==20 && porazki<=0 && litery>=minliter) {
							keys[k][0]=i;
							keys[k][1]=litery;
							k++;
						}
					}
				}
				if (k==0) {
					minliter--;
				}
			}
			
			if (k>1 || odpowiedz<0 || odpowiedz>255) {
				for (j=0;j<rzad;j++) {//printing known keys
					for (i=0;i<21;i++) {
						c1=krypt[i][j];
						c2=sk[j];
						printf("%c ",c1^c2);
					}
					printf("-line %d\n",j);
				}
				for (l=21;l>=0;l--) {//printing possible keys
					for (i=0;i<k;i++) {
						if (keys[i][1]==l) {
							for (j=0;j<21;j++) {
								c1=krypt[j][rzad];
								c2=keys[i][0];
								printf("%c ",c1^c2);
							}
							printf("-key %d-%d letters\n",keys[i][0],keys[i][1]);
						}
					}
				}
				scanf("%d",&odpowiedz);
			} else if (odpowiedz>=0 && odpowiedz<=255){
				odpowiedz=keys[0][0];
				printf("rzad %d policzony automatycznie (jeden klucz z maksymalna iloscia liter)\n",rzad);
			}
		} while (odpowiedz<0 || odpowiedz>255);
		sk[rzad]=odpowiedz;
		if (rzad==dl_krypt-1) {
			for (j=0;j<21;j++) {
				for (i=0;i<dl_krypt;i++) {
					c1=sk[i];
					c2=krypt[j][i];
					printf("%c",c1^c2);
				}
				printf("\n");
			}
			scanf("%d",&odpowiedz);
			if (odpowiedz<0) {
				rzad=rzad+odpowiedz;
				odpowiedz=258;
				if (rzad<0) rzad=0;
			}
		}
	}
	return 0;
}