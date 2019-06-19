
#define LCD_A0		0x4000
#define LCD_RESET	0x2000
#define LCD_CS		0x1000
#define LCD_NRD		0x800
#define LCD_WR		0x40
#define LCD_DATA_MASK   0x7fff8000
#define LCD_DATA_SHIFT	15

/* LCD COMMAND*/
#define DISON                      0xaf
#define DISOFF                     0xae
#define DISNOR                     0xa6
#define DISINV                     0xa7
#define COMSCN                     0xbb
#define DISCTL                     0xca
#define SLPIN                      0x95
#define SLPOUT                     0x94
#define PASET                      0x75
#define CASET                      0x15
#define DATCTL                     0xbc
#define DATALL                     0xbd
#define DATCAN                     0xbe
#define RGBSET8                    0xce
#define RAMWR                      0x5c
#define RAMRD                      0x5d
#define PTLIN                      0xa8
#define PTLOUT                     0xa9
#define RMWIN                      0xe0
#define RMWOUT                     0xee
#define ASCSET                     0xaa
#define SCSTART                    0xab
#define OSCON                      0xd1
#define OSCOFF                     0xd2
#define PWRCTR                     0x20
#define VOLCTR                     0x81
#define TMPGRD                     0x82
#define VOLUP                      0xd6
#define VOLDOWN                    0xd7
#define EPCTIN                     0xcd
#define EPCOUT                     0xcc
#define EPMWR                      0xfc
#define EPMRD                      0xfd
#define EPSRRD1                    0x7c
#define EPSRRD2                    0x7d
#define NOP                        0x25

static short hash_ta[16] ={0x0,0x8,0x4,0xc,0x2,0xa,0x6,0xe,0x1,0x9,0x5,0xd,0x3,0xb,0x7,0xf};

#define CHANGE_DATA(n) (unsigned long) (hash_ta[ (n) & 0xf]<<12 | hash_ta[( (n) & 0xf0)>>4]<<8 | hash_ta[( (n) & 0xf00)>>8]<<4 | hash_ta[( (n) & 0xf000)>>12]<<0)
           
