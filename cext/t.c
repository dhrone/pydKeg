#include <stdio.h>
#include <math.h>

void printframe(int width, int height, int *frame);

int main() {
	int x = 1;
	int y = ceil(x/8.0);

	printf("\nValue is %d\n", y);

	int frame[2][17] = { { 255, 254, 252, 248, 240, 224, 192, 128,   0, 128, 192, 224, 240, 248, 252, 254, 255 },
										{   0, 128, 192, 224, 240, 248, 252, 254, 255, 254, 252, 248, 240, 224, 192, 128,   0} };

	printframe(17, 16, (int *)frame);
}

void printframe(int width, int height, int *frame) {
	int rows = ceil(height/8.0);

	printf("   |");
	int i = 0;
	for (int j=0; j<width; j++){
		if (j%5 == 0) printf("%d",j/5); else printf(" ");
		i++;
		if (i==10) i = 0;
	}
	printf("|\n");
	printf("   ");
	for (int j = 0; j < width+2; j++) {
		printf("-");
	}
	printf("\n");
	for (int j = 0; j < rows; j++) {
		int mask = 1;
		for (int b = 0; b < 8; b++) {
			printf("%03d|",j*8+b);
			for (int i = 0; i < width; i++) {
				if (*((frame+j*width)+i) & mask) printf("*"); else printf("_");
			}
			printf("|\n");
			mask <<= 1;
		}
	}
	printf("   ");
	for (int j = 0; j < width+2; j++) {
		printf("-");
	}
	printf("\n");
}
