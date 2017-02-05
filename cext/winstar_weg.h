
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80
//#define P_NARROW 100   // Min delay 120
#define P_NARROW 150   // Min delay 120
#define P_WIDE   1000  // Pulse width
#define P_15MS   15500 // >15ms

void updateframe(int rs, int e, int d4, int d5, int d6, int d7, int width, int height, int *frame);
void printframe(int width, int height, int *frame);
void send_byte( int rs, int e, int d4, int d5, int d6, int d7, const unsigned char u8_byte, int mode );
void send_nibble( int rs, int e, int d4, int d5, int d6, int d7, const unsigned char u8_byte, int mode );
void pulse( int e, int narrow );
int init_gpio( int rs, int e, int d4, int d5, int d6, int d7 );
int init( int rs, int e, int d4, int d5, int d6, int d7 );
void clear( int rs, int e, int d4, int d5, int d6, int d7 );
void setrow( int rs, int e, int d4, int d5, int d6, int d7, unsigned char row );
