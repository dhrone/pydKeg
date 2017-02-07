#include <stdio.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <bcm2835.h>
#include <unistd.h>
#include "winstar_weg.h"

void updateframe(int rs, int e, int d4, int d5, int d6, int d7, int width, int height, int *frame) {

	int val;
	int j,i;

	for (j=0; j<height; j++) {
		setrow(rs, e, d4, d5, d6, d7, j);
		for (i=0; i<width; i++) {
			val = (unsigned char)*((frame+j*width)+i);
			send_byte(rs, e, d4, d5, d6, d7, val, 1);
		}
	}
}

void printframe(int width, int height, int *frame) {
	printf("\n   |");
	int i = 0, j, b;
	for (j=0; j<width; j++){
		if (j%5 == 0) printf("%d",j/5); else printf(" ");
		i++;
		if (i==10) i = 0;
	}
	printf("|\n");
	printf("   ");
	for (j = 0; j < width+2; j++) {
		printf("-");
	}
	printf("\n");
	for (j = 0; j < height; j++) {
		int mask = 1;
		for (b = 0; b < 8; b++) {
			printf("%03d|",j*8+b);
			for (i = 0; i < width; i++) {
				if (*((frame+j*width)+i) & mask) printf("*"); else printf(" ");
			}
			printf("|\n");
			mask <<= 1;
		}
	}
	printf("   ");
	for (j = 0; j < width+2; j++) {
		printf("-");
	}
	printf("\n");
}

void send_byte( int rs, int e, int d4, int d5, int d6, int d7, const unsigned char u8_byte, int mode ) {

	bcm2835_gpio_write(rs, mode);
	bcm2835_gpio_write(d4, (u8_byte & 0x10));
	bcm2835_gpio_write(d5, (u8_byte & 0x20));
	bcm2835_gpio_write(d6, (u8_byte & 0x40));
	bcm2835_gpio_write(d7, (u8_byte & 0x80));
	pulse(e, mode);

	bcm2835_gpio_write(rs, mode);
	bcm2835_gpio_write(d4, (u8_byte & 0x01));
	bcm2835_gpio_write(d5, (u8_byte & 0x02));
	bcm2835_gpio_write(d6, (u8_byte & 0x04));
	bcm2835_gpio_write(d7, (u8_byte & 0x08));
	pulse(e, mode);
}

void send_nibble( int rs, int e, int d4, int d5, int d6, int d7, const unsigned char u8_byte, int mode ) {

	bcm2835_gpio_write(rs, mode);
	bcm2835_gpio_write(d4, (u8_byte & 0x01));
	bcm2835_gpio_write(d5, (u8_byte & 0x02));
	bcm2835_gpio_write(d6, (u8_byte & 0x04));
	bcm2835_gpio_write(d7, (u8_byte & 0x08));
	pulse(e, mode);
}

int nsleep(unsigned int nsec) {
	struct timespec timeout0;
	struct timespec timeout1;
	struct timespec* tmp;
	struct timespec* t0 = &timeout0;
	struct timespec* t1 = &timeout1;

	t0->tv_sec = nsec / 1000000000;
	t0->tv_nsec = nsec % 1000000000;
	printf ("Sleeping %ld seconds and %ld nanoseconds\n",t0->tv_sec, t0->tv_nsec);

	while(nanosleep(t0, t1) == -1) {
		if(errno == EINTR) {
			tmp = t0;
			t0 = t1;
			t1 = tmp;
		}
		else return -1;
	}
	return 0;
}

void pulse( int e, int narrow ) {
  bcm2835_gpio_write(e, 1);
  nsleep(150);
  bcm2835_gpio_write(e, 0);
  //(narrow)? usleep(P_NARROW) : usleep(P_WIDE);
}

int init_gpio( int rs, int e, int d4, int d5, int d6, int d7 ) {
	// bcm could not initalize return status
	if ( ! bcm2835_init() ) return 1;

	// Set relevant pins to OUTPUT
	bcm2835_gpio_fsel(rs, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(e, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(d4, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(d5, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(d6, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(d7, BCM2835_GPIO_FSEL_OUTP);
	usleep(1);

	// Initialize pins to LOW
	bcm2835_gpio_write(rs, 0);
	bcm2835_gpio_write(e, 0);
	bcm2835_gpio_write(d4, 0);
	bcm2835_gpio_write(d5, 0);
	bcm2835_gpio_write(d6, 0);
	bcm2835_gpio_write(d7, 0);
	usleep(100);

  return 0;
}

int init( int rs, int e, int d4, int d5, int d6, int d7 ) {

  // Set all pins low;
  if ( ! init_gpio(rs, e, d4, d5, d6, d7) ) return 1;

	// Allow display to stabilize
  usleep(16000);

	// Assuming that the display may already be in 4 bit mode
	// send four 0000 instructions to resync the display
	send_nibble(rs, e, d4, d5, d6, d7, 0x00, 0);
	send_nibble(rs, e, d4, d5, d6, d7, 0x00, 0);
	send_nibble(rs, e, d4, d5, d6, d7, 0x00, 0);
	send_nibble(rs, e, d4, d5, d6, d7, 0x00, 0);
	usleep(1000);

	// Set to 8 bit mode
	send_nibble(rs, e, d4, d5, d6, d7, 0x03, 0);
	send_nibble(rs, e, d4, d5, d6, d7, 0x03, 0);
	usleep(1000);

	// Set to 4-bit mode
	send_nibble(rs, e, d4, d5, d6, d7, 0x02, 0);
	usleep(1000);

	send_byte(rs, e, d4, d5, d6, d7, 0x08, 0);
	send_byte(rs, e, d4, d5, d6, d7, 0x29, 0);
	send_byte(rs, e, d4, d5, d6, d7, 0x06, 0);
	send_byte(rs, e, d4, d5, d6, d7, 0x1F, 0);
	send_byte(rs, e, d4, d5, d6, d7, 0x01, 0);
	send_byte(rs, e, d4, d5, d6, d7, 0x0c, 0);

	usleep(1000);
	return 0;
}

void clear( int rs, int e, int d4, int d5, int d6, int d7 ) {
  send_byte(rs, e, d4, d5, d6, d7, 0x01, 0);
}

void setrow( int rs, int e, int d4, int d5, int d6, int d7, unsigned char row )
{
  send_byte(rs, e, d4, d5, d6, d7, LCD_SETDDRAMADDR, 0);
  send_byte(rs, e, d4, d5, d6, d7, LCD_SETCGRAMADDR|row, 0);
}

int main() {
	int x = 1;
	int y = ceil(x/8.0);

	printf("\nValue is %d\n", y);

	// int frame[2][17] = { { 255, 254, 252, 248, 240, 224, 192, 128,   0, 128, 192, 224, 240, 248, 252, 254, 255 },
	// 									{   0, 128, 192, 224, 240, 248, 252, 254, 255, 254, 252, 248, 240, 224, 192, 128,   0} };
	int frame[2][35] = { { 2, 2, 254, 2, 2, 112, 168, 168, 168, 48, 144, 168, 168, 168, 64, 8, 126, 136, 128, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
										{   254, 4, 8, 4, 254, 112, 168, 168, 168, 48, 144, 168, 168, 168, 64, 144, 168, 168, 168, 64, 64, 168, 168, 168, 240, 16, 40, 168, 168, 120, 112, 168, 168, 168, 48} };

	if ( ! init(8,7,12,16,20,21) ) { printf("Error initializing display.  Exiting...\n"); return 1; }
	printframe(35, 2, (int *)frame);
	updateframe(8,7,12,16,20,21,35,2, (int *)frame);

	bcm2835_close();
	return 0;
}
