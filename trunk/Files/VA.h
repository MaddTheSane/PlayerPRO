#ifndef __VAH__
#define __VAH__

struct VAGlobalsRecord
{
	Ptr				base;
	Ptr				*quickrow;
	short			row;
	Byte			color;

	Rect			frame;
};
typedef struct VAGlobalsRecord VAGlobalsRecord;


void VABresenhamS( short x1, short y1, short x2, short y2, long **theGAM);
void VABresenham(short x1, short y1, short x2, short y2);
void VALine(short x1, short y1, short x2, short y2);
void VALineTo(short x, short y);
void VASafeLineTo(short x, short y);
void VAPixel(short,short);
void VASafePixel(short x, short y);
void VASpot(short,short);
void VASafeSpot(short,short);
void VADrawNumber(long num, short x, short y);
void VADrawPadNumber(long num, short x, short y, short width);
void VAExplosion(short x, short y, short size, short color);
void VAStaticLine(short,short,short,short, long**);
void VAStaticLineTo(short,short);
void VAStep(void);
void VAClose(void);
Handle	ScreenSelect();
short	 VAClip(Rect *ptrec);
void VADrawText(char *,long,long);
void *VAColorToQD(short);

#endif

/*#ifdef VECTOR_ANIMATION_MAIN
		VAGlobalsRecord		VA[ 10];
#else
extern	VAGlobalsRecord		VA[ 10];
#endif*/
