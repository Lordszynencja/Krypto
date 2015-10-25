#include <stdio.h>

int main() {
	char c;
	scanf("%c",&c);
	int i=0,lines=0,character=0;
	while (c!=':') {
		if (c=='0') {
			character=character<<1;
			i++;
			if (i>7) {
				i=0;
				lines++;
				int c1=character/16;
				int c2=character%16;
				printf("\\x%c%c",(c1>9?c1+55:c1+48),(c2>9?c2+55:c2+48));
				character=0;
			}
		} else if (c=='1') {
			character=(character<<1)+1;
			i++;
			if (i>7) {
				i=0;
				lines++;
				int c1=character/16;
				int c2=character%16;
				printf("\\x%c%c",(c1>9?c1+55:c1+48),(c2>9?c2+55:c2+48));
				character=0;
			}
		}
		scanf("%c",&c);
	}
	printf("\n%d\n",lines);
}