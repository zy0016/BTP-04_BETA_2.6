#define MAXPOINT 6192

#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char

#define ALC_NUMERIC				0x0001	/* 0..9								*/
#define ALC_LCALPHA				0x0002	/* a..z								*/
#define ALC_UCALPHA				0x0004	/* A..Z								*/
#define	ALC_PUNCTUATION			0x0008	/* !",:;?¡¢¡£'()¡­¡¶¡·				*/
#define	ALC_SYMBOLS				0x0010	/* #$%&*+-./<=>@£¤					*/
#define ALC_CHINESE_COMMON		0x0020	/* Commonly used Chinese characters	*/
#define ALC_CHINESE_RARE		0x0040	/* Rarely used Chinese characters	*/
#define ALC_CHINESE_VARIANT		0x0080	/* Variant Chinese characters		*/

int _stdcall HWRecognize( BYTE* pTrace, int nLength, char* pResult, int nCand, WORD wRange );
void HWSetDictionary( DWORD dwAddress );
