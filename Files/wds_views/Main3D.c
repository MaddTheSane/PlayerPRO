#include "MAD.h"
#include "VA.h"
#include "Shuddup.h"
#include "RDriver.h"
#include "3DUtils.h"
#include "math.h"
#include "fade.h"
#include "Palettes.h"

static	long			Longueur;
static	Boolean			WaitTicks, doubleView, SmallCube, TextDefile, vectorBall;
static	long			*CopyStart,*theSAUVE;
static	Rect			MyBounds;
static	long			T3D[100][3];
static	short			T2D[ 30];
static	short			*Xp1, *Yp1;		/* 4 */
static	short			myColor, curforme, SIter;
static	short			imTotal;
static	Point			PP, De;

extern	short 	theDepth, RowBytes, NoEtoiles, EtoileColor;
extern	long	DeviationV, Mz, CY, CX, ds;

void PaintICL8();
void ScrollTextInit( Boolean, Boolean);
void ScrollTextClose( Boolean);
void DoEffaceCODE( Handle	thePtr, long TT);
Ptr	Transformation8( Ptr thePtr);
void InitBounceBall( short xx, short yy, short TypeB);

#define KeyLast 0x184


static	Ptr		sourcePtr;
static	short	HPos, VPos, OscilloL;
static	Ptr		*SaveVal, *StartVal;

extern	Ptr		AudioDSPPtr;

Ptr	TransformationDSPMain3D( Ptr thePtr)
{
	register Ptr		tempPtr;
	register short		i = MADDriver->ASCBUFFER, x = MADDriver->ASCBUFFER + MADDriver->ASCBUFFER;
	register Byte		bb = 0x80;
	
	tempPtr = thePtr;
	do
	{
		AudioDSPPtr[ --i] = *tempPtr - bb;
		tempPtr += 2L;
		AudioDSPPtr[ --x] = *tempPtr - bb;
		tempPtr += 2L;
	}
	while( i != 0);
	
	return( AudioDSPPtr);
}

Ptr	TransformationMonoMain3D( Ptr thePtr)
{
	register Ptr		tempPtr;
	register short		i = MADDriver->ASCBUFFER;
	register Byte		bb = 0x80;
	
	tempPtr = thePtr;
	do
	{
		AudioDSPPtr[ --i] = *tempPtr - bb;
		tempPtr += 2L;
	}
	while( i != 0);
	
	return( AudioDSPPtr);
}

Ptr	Transformation8Main3D( Ptr thePtr)
{
	register Ptr		tempPtr;
	register short		i = MADDriver->ASCBUFFER, x = MADDriver->ASCBUFFER + MADDriver->ASCBUFFER;
	
	tempPtr = thePtr;
	do
	{
		AudioDSPPtr[ --i] = *tempPtr;
		tempPtr ++;
		AudioDSPPtr[ --x] = *tempPtr;
		tempPtr ++;
	}
	while( i != 0);
	
	return( AudioDSPPtr);
}

void SpecialOsci2( short col)
{
short		i, offsetV = VPos, offsetH = HPos - OscilloL, lastv, aY;
Byte		*tempPtr = ( Byte*) sourcePtr;
long		tempL;
Ptr			*tempPtr2 =  (Ptr*) SaveVal;

VA[ 0].color = col;
i = OscilloL;

while( i > 0)
{
	*tempPtr2[ i] = 0xFF;
	
	tempPtr2[ i] = VA[ 0].quickrow[ offsetV + *tempPtr++] + i + offsetH;
	*tempPtr2[ i] = col;
	i--;
}

}

void Set( long	*intPtr, short i, short x, short y, short z)
{
	intPtr[ 0 + i * 3] = x;
	intPtr[ 1 + i * 3] = y;
	intPtr[ 2 + i * 3] = z;
}

void Analyse3D( short forme)
{
short			i, z, x, j, inc, start;
double			longitude, latitude;
TDList			ObjectList;

i = 0;
#define	Aa T3D[0][i++]
#define RADIUS -80.0

curforme = forme;

switch( forme)
{
case 4:
	
	
	i = 0;
break;

case 1:
	Set( T3D[0], i++, -80, -80, -80);			Set( T3D[0],i++, -80, -80, 80);
	Set( T3D[0], i++, -80, 80, 80);				Set( T3D[0],i++, -80, 80, -80);
	Set( T3D[0], i++, -80, -80, -80);			Set( T3D[0],i++, 80, -80, -80);
	Set( T3D[0], i++, 80, -80, 80);				Set( T3D[0],i++, 80, 80, 80);
	Set( T3D[0], i++, 80, 80, -80);				Set( T3D[0],i++, 80, -80, -80);
	Set( T3D[0], i++, -80, -80, 80);			Set( T3D[0],i++, -80, 80, 80);
	Set( T3D[0], i++, 80, 80, 80);				Set( T3D[0],i++, 80, -80, 80);
	Set( T3D[0], i++, -80, -80, 80);			Set( T3D[0],i++, -80, -80, -80);
	Set( T3D[0], i++, -80, 80, -80);			Set( T3D[0],i++, 80, 80, -80);
	Set( T3D[0], i++, 80, -80, -80);			Set( T3D[0],i++, -80, -80, -80);

	Set( T3D[0], i++, -60, -60, -60);			Set( T3D[0],i++, -60, -60, 60);
	Set( T3D[0], i++, -60, 60, 60);				Set( T3D[0],i++, -60, 60, -60);
	Set( T3D[0], i++, -60, -60, -60);			Set( T3D[0],i++, 60, -60, -60);
	Set( T3D[0], i++, 60, -60, 60);				Set( T3D[0],i++, 60, 60, 60);
	Set( T3D[0], i++, 60, 60, -60);				Set( T3D[0],i++, 60, -60, -60);
	Set( T3D[0], i++, -60, -60, 60);			Set( T3D[0],i++, -60, 60, 60);
	Set( T3D[0], i++, 60, 60, 60);				Set( T3D[0],i++, 60, -60, 60);
	Set( T3D[0], i++, -60, -60, 60);			Set( T3D[0],i++, -60, -60, -60);
	Set( T3D[0], i++, -60, 60, -60);			Set( T3D[0],i++, 60, 60, -60);
	Set( T3D[0], i++, 60, -60, -60);			Set( T3D[0],i++, -60, -60, -60);


	T2D[0] = T2D[1] = T2D[2] = T2D[3] = 5;
	T2D[4] = T2D[5] = T2D[6] = T2D[7] = 5;

	T2D[ 8] = -999;
break;

case 2:
	Aa = 160; 	Aa = -80; 	Aa = 100;			Aa = 80; 	Aa =  -80; 	Aa = 100;
	Aa = 80; 	Aa = 80; 	Aa = 100;			Aa = 160; 	Aa = 80; 	Aa = 100;
	Aa = 160; 	Aa = -80; 	Aa = 100;

	Aa = 80; 	Aa = -80; 	Aa = 100;			Aa = 0; 	Aa =  -80; 	Aa = 100;
	Aa = 0; 	Aa = 80; 	Aa = 100;			Aa = 80; 	Aa = 80; 	Aa = 100;
	Aa = 80; 	Aa = -80; 	Aa = 100;

	Aa = 0; 	Aa = -80; 	Aa = 100;			Aa = -80; 	Aa =  -80; 	Aa = 100;
	Aa = -80; 	Aa = 80; 	Aa = 100;			Aa = 0; 	Aa = 80; 	Aa = 100;
	Aa = 0; 	Aa = -80; 	Aa = 100;
	
	Aa = -80; 	Aa = -80; 	Aa = 100;			Aa = -160; 	Aa =  -80; 	Aa = 100;
	Aa = -160; 	Aa = 80; 	Aa = 100;			Aa = -80; 	Aa = 80; 	Aa = 100;
	Aa = -80; 	Aa = -80; 	Aa = 100;

	Aa = 160; 	Aa = -80; 	Aa = 100;			Aa = -160; 	Aa =  -80; 	Aa = 100;
	Aa = -160; 	Aa = 80; 	Aa = 100;			Aa = 160; 	Aa = 80; 	Aa = 100;
	Aa = 160; 	Aa = -80; 	Aa = 100;


	T2D[0] = T2D[1] = T2D[2] = T2D[3] = T2D[4] = 5;
	T2D[5] = -999;
break;

case 3:
	Aa = 240; 	Aa = 80; 	Aa = 0;			Aa = 80; 	Aa = 80; 	Aa = 0;
	Aa = -80; 	Aa = 80; 	Aa = 0;			Aa = -240; 	Aa = 80; 	Aa = 0;
	Aa = -240; 	Aa = -80; 	Aa = 0;			Aa = -80; 	Aa = -80; 	Aa = 0;
	Aa = 80; 	Aa = -80; 	Aa = 0;			Aa = 240; 	Aa = -80; 	Aa = 0;
	Aa = 240; 	Aa = 80; 	Aa = 0;			
	
	Aa = 240; 	Aa = 80; 	Aa = 0;			Aa = 80; 	Aa = 80; 	Aa = 0;
	Aa = -80; 	Aa = 80; 	Aa = 0;			Aa = -240; 	Aa = 80; 	Aa = 0;
	Aa = -240; 	Aa = -80; 	Aa = 0;			Aa = -80; 	Aa = -80; 	Aa = 0;
	Aa = 80; 	Aa = -80; 	Aa = 0;			Aa = 240; 	Aa = -80; 	Aa = 0;
	Aa = 240; 	Aa = 80; 	Aa = 0;


	T2D[0] = 18;
	T2D[1] = -999;
break;
}
imTotal = i + 1;
}

void ScreenSaver(void)
{
register	short			i, add, savedDepth;
			long			Start, End;					/* 4 */

	savedDepth = theDepth;
	if( theDepth != 8)
	{
		if( HasDepth( LMGetTheGDevice(), 8, 0, 0))
		{
			savedDepth = theDepth;
			if( SetDepth( LMGetTheGDevice(), 8, 0, 0) != noErr)
			{
				Erreur( 23, -6);
				return;
			}
			
			theDepth = (*(*LMGetTheGDevice())->gdPMap)->pixelSize;
			RowBytes = (*(*LMGetTheGDevice())->gdPMap)->rowBytes + 0x8000;
			ReComputeVA();
		}
		else
		{
			Erreur( 23, -6);
			return;
		}
	}
	
	if( qd.screenBits.bounds.right < 640 || qd.screenBits.bounds.bottom < 480)
	{
		Erreur( 23, -6);
		return;
	}
	
	if(		MADDriver->DriverSettings.outPutMode == MonoOutPut) doubleView = false;
	else doubleView = true;
	
	MyBounds = qd.screenBits.bounds;
	SmallCube = false;
	vectorBall = false;
	
	Mymain();
	
	if( savedDepth != 8)
	{
		SetDepth( LMGetTheGDevice(), savedDepth, 0, 0);
		
		theDepth = (*(*LMGetTheGDevice())->gdPMap)->pixelSize;
		RowBytes = (*(*LMGetTheGDevice())->gdPMap)->rowBytes + 0x8000;
		ReComputeVA();
	}
}

void DoDrawGQD(short Start, short Size)
{
short x;

	for( x = Start; x < Start + Size - 1 ; x ++)
	{
		MoveTo(		Xp1[ x],
					Yp1[ x]);
		LineTo(
					Xp1[ x+1],
					Yp1[ x+1]);
	}
}

void DoDrawG( register short Start, register short Size)
{
register short 	x;
	
	for( x = Start; x < Start + Size - 1 ; x ++)
	{
	
	VAStaticLine(	Xp1[x],
					Yp1[x],
					Xp1[x+1],
					Yp1[x+1],
					&theSAUVE);
	}
}


void DoEffaceCODE( Handle	thePtr, long TT)
{
register 	Handle	tempPtr;
register 	long	i;
register 	char	Col = 0xFF;

tempPtr = thePtr - TT;
while( TT > 0)
{
	**tempPtr++ = Col;
	TT--;
}
}

void WORK(void)
{
short	y, val;

	val = 0;
	y=0;
	VA[ 0].color = 186;
	do
	{
		DoDrawG(val, T2D[y]);
	//	else DoDrawGQD(val, T2D[y]);
		
		val += T2D[y];
		y++;
	}while( T2D[y] != -999);

switch( curforme)
{
case 1:
	if( !SmallCube) break;
	VAStaticLine(	Xp1[0],
					Yp1[0],
					Xp1[15],
					Yp1[15],
					&theSAUVE);
	VAStaticLine(	Xp1[1],
					Yp1[1],
					Xp1[10],
					Yp1[10],
					&theSAUVE);
	VAStaticLine(	Xp1[2],
					Yp1[2],
					Xp1[11],
					Yp1[11],
					&theSAUVE);
	VAStaticLine(	Xp1[3],
					Yp1[3],
					Xp1[16],
					Yp1[16],
					&theSAUVE);

	VAStaticLine(	Xp1[5],
					Yp1[5],
					Xp1[18],
					Yp1[18],
					&theSAUVE);
	VAStaticLine(	Xp1[6],
					Yp1[6],
					Xp1[13],
					Yp1[13],
					&theSAUVE);
	VAStaticLine(	Xp1[7],
					Yp1[7],
					Xp1[12],
					Yp1[12],
					&theSAUVE);
	VAStaticLine(	Xp1[8],
					Yp1[8],
					Xp1[17],
					Yp1[17],
					&theSAUVE);
	break;
	
	case 3:
	VAStaticLine(	Xp1[3],
					Yp1[3],
					Xp1[12],
					Yp1[12],
					&theSAUVE);
	VAStaticLine(	Xp1[4],
					Yp1[4],
					Xp1[13],
					Yp1[13],
					&theSAUVE);
	VAStaticLine(	Xp1[7],
					Yp1[7],
					Xp1[16],
					Yp1[16],
					&theSAUVE);
	break;
}
}

void TubeMOD( short forme)
{
	#define RR	4
	#define TUBENULL	5

	switch( forme)
	{
	case 1:
		if( MADDriver->Tube[0] > TUBENULL)
		{
			T3D[0][0] = T3D[1][0] = T3D[2][0] = T3D[3][0] = T3D[4][0] = -200;	MADDriver->Tube[0] = TUBENULL;
		}
		if( MADDriver->Tube[1] > TUBENULL)
		{
			T3D[5][0] = T3D[6][0] = T3D[7][0] = T3D[8][0] = T3D[9][0] = 200;	MADDriver->Tube[1] = TUBENULL;
		}
		if( MADDriver->Tube[2] > TUBENULL)
		{
			T3D[10][2] = T3D[11][2] = T3D[12][2] = T3D[13][2] = T3D[14][2] = 200;	MADDriver->Tube[2] = TUBENULL;
		}
		if( MADDriver->Tube[3] > TUBENULL)
		{
			T3D[15][2] = T3D[16][2] = T3D[17][2] = T3D[18][2] = T3D[19][2] = -200;	MADDriver->Tube[3] = TUBENULL;
		}
	
		if( T3D[0][0] < -80)
		{
			T3D[0][0]+=RR;	T3D[1][0]+=RR;	T3D[2][0]+=RR;	T3D[3][0]+=RR;	T3D[4][0]+=RR;
		}
		if( T3D[5][0] > 80)
		{
			T3D[5][0]-=RR;	T3D[6][0]-=RR;	T3D[7][0]-=RR;	T3D[8][0]-=RR;	T3D[9][0]-=RR;
		}
		if( T3D[10][2] > 80)
		{
			T3D[10][2]-=RR;	T3D[11][2]-=RR;	T3D[12][2]-=RR;	T3D[13][2]-=RR;	T3D[14][2]-=RR;
		}
		if( T3D[15][2] < -80)
		{
			T3D[15][2]+=RR;	T3D[16][2]+=RR;	T3D[17][2]+=RR;	T3D[18][2]+=RR;	T3D[19][2]+=RR;
		}
	break;
	
	case 2:
		if( MADDriver->Tube[0] > TUBENULL)
		{
			T3D[0][2] = T3D[1][2] = T3D[2][2] = T3D[3][2] = T3D[4][2] = -100;	MADDriver->Tube[0] = TUBENULL;
		}
		if( MADDriver->Tube[1] > TUBENULL)
		{
			T3D[5][2] = T3D[6][2] = T3D[7][2] = T3D[8][2] = T3D[9][2] = -100;	MADDriver->Tube[1] = TUBENULL;
		}
		if( MADDriver->Tube[2] > TUBENULL)
		{
			T3D[10][2] = T3D[11][2] = T3D[12][2] = T3D[13][2] = T3D[14][2] = -100;	MADDriver->Tube[2] = TUBENULL;
		}
		if( MADDriver->Tube[3] > TUBENULL)
		{
			T3D[15][2] = T3D[16][2] = T3D[17][2] = T3D[18][2] = T3D[19][2] = -100;	MADDriver->Tube[3] = TUBENULL;
		}
	
		if( T3D[0][2] < 100)
		{
			T3D[0][2]+=4;	T3D[1][2]+=4;	T3D[2][2]+=4;	T3D[3][2]+=4;	T3D[4][2]+=4;
		}
		if( T3D[5][2] < 100)
		{
			T3D[5][2]+=4;	T3D[6][2]+=4;	T3D[7][2]+=4;	T3D[8][2]+=4;	T3D[9][2]+=4;
		}
		if( T3D[10][2] < 100)
		{
			T3D[10][2]+=4;	T3D[11][2]+=4;	T3D[12][2]+=4;	T3D[13][2]+=4;	T3D[14][2]+=4;
		}
		if( T3D[15][2] < 100)
		{
			T3D[15][2]+=4;	T3D[16][2]+=4;	T3D[17][2]+=4;	T3D[18][2]+=4;	T3D[19][2]+=4;
		}
	break;
	
	case 3:
		if( MADDriver->Tube[0] > TUBENULL)
		{
			T3D[0][2] = T3D[8][2] = 100;
			T3D[9][2] = T3D[17][2] = -100;
			
			T3D[7][2] = 100;	T3D[16][2] = -100;
			MADDriver->Tube[0] = TUBENULL;
		}
		if( MADDriver->Tube[1] > TUBENULL)
		{
			T3D[1][2] = 100;	T3D[10][2] = -100;	MADDriver->Tube[1] = TUBENULL;
			T3D[6][2] = 100;	T3D[15][2] = -100;
		}
		if( MADDriver->Tube[2] > TUBENULL)
		{
			T3D[2][2] = 100;	T3D[11][2] = -100;	MADDriver->Tube[2] = TUBENULL;
			T3D[5][2] = 100;	T3D[14][2] = -100;
		}
		if( MADDriver->Tube[3] > TUBENULL)
		{
			T3D[3][2] = 100;	T3D[12][2] = -100;	MADDriver->Tube[3] = TUBENULL;
			T3D[4][2] = 100;	T3D[13][2] = -100;
		}
	
		if( T3D[0][2] > 0)
		{
			T3D[0][2] -=RR; T3D[8][2] -=RR;
			T3D[9][2] +=RR; T3D[17][2] +=RR;
			
			T3D[7][2] -=RR;	T3D[16][2] +=RR;
		}
		if( T3D[1][2] > 0)
		{
			T3D[1][2] -=RR;	T3D[10][2] +=RR;
			T3D[6][2] -=RR;	T3D[15][2] +=RR;
		}
		if( T3D[2][2] > 0)
		{
			T3D[2][2] -=RR;	T3D[11][2] +=RR;
			T3D[5][2] -=RR;	T3D[14][2] +=RR;
		}
		if( T3D[3][2] > 0)
		{
			T3D[3][2] -=RR;	T3D[12][2] +=RR;
			T3D[4][2] -=RR;	T3D[13][2] +=RR;
		}
	break;
	}
}

static Boolean		DemoOK;

void DrawS( short	theNo)
{
	Str255	theString;
	
	GetIndString( theString, 129, theNo);
	
	if( theString[ 0] == 0) DemoOK = false;
	else
	{
		ForeColor( redColor);
		MoveTo( (qd.screenBits.bounds.right - StringWidth( theString))/2, qd.screenBits.bounds.bottom/2);
		DrawString( theString);
	}
}

extern	long	*FCosD, *FSinD;

static	short		OndeSize;
static	Byte*	*OndePtr;
static	short		*OndePos, *OndeSpeed;
static	Byte	*OndeCol;
static	Boolean	Action;

void ResetOnde(void)
{
long	i;

	Action = true;

	for( i = 0; i < OndeSize; i++)
	{
		OndePos[ i] = 0;
		OndeSpeed[ i] = RangedRdm( 1, 4);
		
		switch( OndeSpeed[ i])
		{
			case 1:	OndeCol[ i] = 250;	break;
			case 2:	OndeCol[ i] = 248;	break;
			case 3:	OndeCol[ i] = 245;	break;
		}
		
		OndePtr[ i] = (Byte*) VA[ 0].quickrow[ 0] + i;
	}
}

void InitOnde(void)
{
long	i;

OndeSize = qd.screenBits.bounds.right - 1;

OndePtr = (Byte**) MyNewPtr( 100L + OndeSize * 4);
OndePos = ( short*) MyNewPtr( 100L + OndeSize * 2);
OndeSpeed = ( short*) MyNewPtr( 100L + OndeSize * 2);
OndeCol  =	(Byte*) MyNewPtr( 100L + OndeSize);

ResetOnde();
}

void DisposOnde(void)
{
	MyDisposPtr( (Ptr*) &OndePtr);	OndePtr = 0L;
	MyDisposPtr( (Ptr*) &OndePos);	OndePos = 0L;
	MyDisposPtr( (Ptr*) &OndeSpeed);	OndeSpeed = 0L;
	MyDisposPtr( (Ptr*) &OndeCol);	OndeCol = 0L;
}

void Onde(void)
{
	long	oldTicks = LMGetTicks();
	short		i, x, trop;
	Byte	*oldPos, *tempPtr, alpha;
	
	
	for( i = 0; i < OndeSize ; i++)
	{
		if( OndePos[ i] != 0)
		{
			oldPos = OndePtr[ i];
			if( OndePos[ i] < ScreenBounds.bottom - 2) 
			{
				
				OndePos[ i] += OndeSpeed[ i];
				trop = i + -2L +  RangedRdm( 1, 4);
								
				OndePtr[ i] = (Byte*) VA[ 0].quickrow[ OndePos[ i]] + trop;
				
				if( *OndePtr[ i] == 0xFF ||
					*OndePtr[ i] == 245 ||
					*OndePtr[ i] == 248 ||
					*OndePtr[ i] == 250)
				{
					*OndePtr[ i] = OndeCol[ i];
					
					if( *oldPos == 245 ||
						*oldPos == 248 ||
						*oldPos == 250) *oldPos = 0xFF;
				}
				else
				{
					tempPtr = OndePtr[ i];
					while( *tempPtr != 0xFF && tempPtr >= (Byte*) VA[ 0].quickrow[ 0])
					{
						tempPtr -= VA[ 0].row;
					}
					alpha = *oldPos;
					*oldPos = 0xFF;
					*tempPtr = alpha;
					
					OndePos[ i] = 1;
					OndePtr[ i] = (Byte*) VA[ 0].quickrow[ 0] + i;
				}
			}
			else
			{
				tempPtr = OndePtr[ i];
				while( *tempPtr != 0xFF && tempPtr >= (Byte*) VA[ 0].quickrow[ 0])
				{
					tempPtr -= VA[ 0].row;
				}
				alpha = *oldPos;
				*oldPos = 0xFF;
				*tempPtr = alpha;
					
				OndePos[ i] = 1;
				OndePtr[ i] = (Byte*) VA[ 0].quickrow[ 0] + i;
			}
		}
	}
	
	if( Action)
	{
		x = RangedRdm( 0, OndeSize-1);
	
		if( OndePos[ x] == 0) OndePos[ x] = 1;
		else
		{
			for( i = x; i < OndeSize; i++)
			{
				if( OndePos[ i] == 0)
				{
					OndePos[ i] = 1;
					break;
				}
			}
			if( i == OndeSize) Action = false;
		}
	}
}

void DoIntroDEMO(void)
{
	short			X1, X2, compteur = 0, i;
	Point		De1, De2;
	long		oldTicks = LMGetTicks(), tempL;
	Str255		pStr;
	Byte		tempB;
	Rect		tempRect;
	PicHandle	thePict;
	
	TextSize( 18);
	TextFont( kFontIDHelvetica);
	TextFace( bold);
	ForeColor( redColor);
	
	thePict = GetPicture( 133);
	
	tempRect.left = (qd.screenBits.bounds.right - (*thePict)->picFrame.right)/2;
	tempRect.right = tempRect.left + (*thePict)->picFrame.right;
	
	tempRect.bottom = qd.screenBits.bounds.bottom/2 + (*thePict)->picFrame.bottom + 20;
	tempRect.top = qd.screenBits.bounds.bottom/2 + 20;
	
	DrawPicture( thePict, &tempRect);
	
	ReleaseResource( (Handle) thePict);
	
	pStrcpy( pStr, "\pROSSET Software presents...");
	MoveTo( (qd.screenBits.bounds.right - StringWidth( pStr))/2, qd.screenBits.bounds.bottom/2);
	DrawString( pStr);
	
	DemoOK = true;
	
	TextSize( 48);
	ForeColor( blackColor);
	DrawString( "\pAbcdef");
	ForeColor( redColor);
	
	InitOnde();
	
	tempL = LMGetTicks() + 420L;
	
	tempRect.left = qd.screenBits.bounds.left + 10;
	tempRect.right = qd.screenBits.bounds.right - 10;
	tempRect.top = qd.screenBits.bounds.bottom/2 - 50;
	tempRect.bottom = qd.screenBits.bounds.bottom/2 + 20;
	
	*(short*) KeyLast = 0;
	while( DemoOK)
	{	
		Onde();
		
		while( LMGetTicks() == oldTicks);
		oldTicks = LMGetTicks();
		
		if( tempL <= LMGetTicks())
		{
			ForeColor( blackColor); PaintRect( &tempRect);
			ForeColor( redColor);
			
			Onde();
			
			tempL = LMGetTicks() + 420L;
			compteur++;
			DrawS( compteur);
		}

		if( Button()) DemoOK = false;
		
		tempB = (Byte) (*(short*) KeyLast);
		if( tempB == ' ') DemoOK = false;
	}
	
	while( Button());
	
	DisposOnde();
	
	FlushEvents( everyEvent, 0L);
	ForeColor( blackColor);
	PaintRect( &ScreenBounds);
}

void Mymain(void)
{
long			oldTicks, temp, formeTicks, maxS = 0;	
Str255			theName;
short			i,x,Xx1,Xx2, whichForme, SIterCopy;
CGrafPort		myCPort;
Byte			tempB;
Point			Osci1, Osci2, De1, De2;
Rect			ttRect;
Boolean			KOKO = true;
GrafPtr			savePort;
DialogPtr		aWind;

LMSetGhostWindow( 0L);

SIterCopy = SIter;

	GetPort( &savePort);

	OpenCPort( &myCPort);
	
	SetPort( (GrafPtr) &myCPort);
	HideCursor();

Xp1 = (short*) MyNewPtr( 300 * sizeof( short));
Yp1 = (short*) MyNewPtr( 300 * sizeof( short));

ScreenBounds.left = MyBounds.left + 1;
ScreenBounds.right = MyBounds.right - 1;
ScreenBounds.top = MyBounds.top + 1;
ScreenBounds.bottom = MyBounds.bottom - 33;

InitStar();
NoEtoiles = 50;
DeviationV =  -RowBytes * 2;

EffaceEcran2();

ttRect = MyBounds;
ttRect.bottom = ScreenBounds.bottom + 1;

aWind = GetNewDialog( 138, 0L, (WindowPtr) -1L);

PaintRect( &ttRect);

CopyStart = (long*) MyNewPtr( 12000 * sizeof( long));
if( CopyStart == 0L) MyDebugStr("\pMEMORY Please....!! ");
theSAUVE = CopyStart;

Longueur = 0;
ds = thePrefs.Mz;
Init3D();

CX = ScreenBounds.right/2;
CY = ScreenBounds.bottom/2;

oldTicks = LMGetTicks();
Xx1 = 1;

/**********************/
/*********************/

DoIntroDEMO();

PaintICL8();
while( Button());

if( thePrefs.SSStars) ActiveScrollEtoile();

theSAUVE = CopyStart;

ForeColor( blackColor);

WaitTicks 	= true;
temp 		= 0;

SIter = MADDriver->ASCBUFFER;
if( SIter > qd.screenBits.bounds.right) SIter = qd.screenBits.bounds.right;

De2.v 		= qd.screenBits.bounds.top +2;
De1.v 		= De.v 				= qd.screenBits.bounds.bottom - 300;
De1.h 		= De2.h = De.h 		= qd.screenBits.bounds.right/2 - SIter/2;
PP.v 		= De.v;
PP.h		= De.h;
PP.h 		+= SIter;

HPos 		= De1.h + SIter;
VPos 		= De1.v;
OscilloL 	= SIter;
sourcePtr 	= MADDriver->OscilloWavePtr;
StartVal = SaveVal = (Ptr*) MyNewPtr( 5000L*4L);
for( i=0; i< 5000L; i++) SaveVal[i] = VA[ 0].quickrow[ De1.v] + De1.h;

whichForme = 1;
Analyse3D( whichForme);
whichForme++;

InitPlotting( 5, 5);
for( x = 0; x < imTotal; x++) MapCoor( T3D[x][0], T3D[x][1], T3D[x][2], &Xp1[x], &Yp1[x]);

Osci1.v = RangedRdm( 0, 3) - 1;
Osci1.h = RangedRdm( 0, 3) - 1;

Xx1 = 0;
Xx2 = 0;

formeTicks = LMGetTicks() + 60L*60L*3L;
theSAUVE = CopyStart;

if( thePrefs.SSJumping) InitBounceBall(  10,  50,  1);

for(i=0; i < 360; i++)
{	
	WORK();
	
	switch( MADDriver->DriverSettings.outPutBits)
	{
		case 8:
			switch( MADDriver->DriverSettings.outPutMode)
			{
				case MonoOutPut:
					sourcePtr = MADDriver->OscilloWavePtr;
					doubleView = false;
				break;
				
				case StereoOutPut:
				case DeluxeStereoOutPut:
					doubleView = true;
					sourcePtr = Transformation8Main3D( MADDriver->OscilloWavePtr);
				break;
			}
		break;
		
		case 16:
			switch( MADDriver->DriverSettings.outPutMode)
			{
				case MonoOutPut:
					doubleView = false;
					sourcePtr = TransformationMonoMain3D( MADDriver->OscilloWavePtr);
				break;
				
				case StereoOutPut:
				case DeluxeStereoOutPut:
					doubleView = true;
					sourcePtr = TransformationDSPMain3D( MADDriver->OscilloWavePtr);
				break;
			}
		break;
	}
	
	sourcePtr = GetAudioChannel( true, MADDriver->ASCBUFFER);
	
	SpecialOsci2( 185);
	
	if( MADDriver->musicEnd == true)
	{
		DoLoadOtherMusic( false);
		PaintRect( &ScreenBounds);
	}
		
	if( doubleView)
	{
		VPos	= De2.v;
		sourcePtr += MADDriver->ASCBUFFER;		SaveVal 	+= 2000;
		
		SpecialOsci2( 215);

		sourcePtr -= MADDriver->ASCBUFFER;		SaveVal		-= 2000;
		VPos	= De1.v;
	}
	
	tempB = (Byte) (*(short*) KeyLast);
	switch( tempB)
	{
		case 'S':
		case 's':
			WaitTicks = !WaitTicks;
			*(short*) KeyLast = 0;
		break;
	
		case 'T':
		case 't':
			TextDefile = !TextDefile;
		
			if( TextDefile) ActiveScrollText();
			else DesactiveScrollText();
			
			PaintRect( &qd.screenBits.bounds);
			
			*(short*) KeyLast = 0;
		break;
	
		case 'F':
		case 'f':
			SmallCube = !SmallCube;
			*(short*) KeyLast = 0;
		break;
		
		case 'V':
		case 'v':
			vectorBall = !vectorBall;
			*(short*) KeyLast = 0;
		break;
		
		case ' ':
			formeTicks = LMGetTicks();
			*(short*) KeyLast = 0;
		break;
		
		case '+':
			DoSearchUp();
			*(short*) KeyLast = 0;
		break;
		
		case '-':
			DoSearchDown();
			*(short*) KeyLast = 0;
		break;
		
		case '=':
			if( ds < 600) ds +=10;
		break;
		
		case '*':
			if( ds > 11) ds -=10;
		break;
	}
	
	if( tempB >= '0' && tempB <= '9')
	{
		SKVolume( (short) (tempB - 48) * 8);
		*(short*) KeyLast = 0;
	}
	
	if( formeTicks < LMGetTicks())
	{
		Analyse3D( whichForme);
		whichForme++;	if( whichForme > 3) whichForme = 1;
		formeTicks = LMGetTicks() + 60L*60L*3L;
	}
	
	InitPlotting( i, i);
	for( x = 0; x < imTotal; x++) MapCoor( T3D[x][0], T3D[x][1], T3D[x][2], &Xp1[x], &Yp1[x]);
	TubeMOD( curforme);

	Longueur = (theSAUVE - CopyStart);
	
	if( WaitTicks)
	{
		while( LMGetTicks() == oldTicks);
		oldTicks = LMGetTicks();
	}
	
	DoEffaceCODE( (Handle) theSAUVE, Longueur);
	theSAUVE = CopyStart;
	
	if( i >= 359) i=0;
	else if( Button()) break;
	
}

/**********************/
/*********************/

if( thePrefs.SSJumping) CloseBounceBall();

fade_screen( 60, true);

ScrollTextClose( true);

DisposStar();

DisposeDialog( aWind);

CloseCPort( &myCPort);
SetPort( savePort);

MyDisposPtr( (Ptr*) &Xp1);			Xp1 = 0L;
MyDisposPtr( (Ptr*) &Yp1);			Yp1 = 0L;
MyDisposPtr( (Ptr*) &CopyStart);	CopyStart = 0L;

Close3D();

thePrefs.Mz = ds;

//TextFont( 0);		TextSize( 0);
FlushEvents( everyEvent, 0L);
SetPort( savePort);

PaintBehind( FrontWindow(), LMGetGrayRgn());

DrawMenuBar();
ShowCursor();
SetPort( savePort);

SIter = SIterCopy;

UpdateALLWindow();

fade_screen( 60, false);
}

void EffaceEcran2(void)
{
	short		i,x, *Des;
	short		zaza = 0;
	long	oldT = LMGetTicks();
	Rect	ttRe;
	Boolean	EndFade = false;

	Des = (short*) MyNewPtr( MyBounds.right * sizeof(short));
	
	for(i=0; i<MyBounds.right; i++) Des[i] = - RangedRdm( 0, 80);
	Des[0] = 1;
	
	TextDefile = true;
	ScrollTextInit( true, false);
	
	do
	{
		x = 0;
		while( oldT == LMGetTicks());
		oldT = LMGetTicks();
		
		for(i=0; i<MyBounds.right; i++)
		{
			if( Des[i] >= MyBounds.top && Des[i] < (MyBounds.bottom >> 1))
			{
				*(VA[ 0].quickrow[ Des[i]] + i) = 0xFF;
				*(VA[ 0].quickrow[ MyBounds.bottom - Des[i] - 1] + i) = 0xFF;
				x++;
			}
			Des[i] ++;		// = RangedRdm( 0, 2)
		}
		zaza++;
		
		if( x == 0) EndFade = true;
		else if( zaza == 115) if( thePrefs.SSText) ActiveScrollText();
		
	}while( EndFade == false && !Button());
	
	MyDisposPtr( (Ptr*) &Des);	Des = 0L;
	
	if( zaza < 115)
	{
		ttRe = MyBounds;
		ttRe.top = MyBounds.bottom - 32;
		PaintRect( &ttRe);
	}
	
	if( thePrefs.SSText) ActiveScrollText();
}

void EffaceEcran(void)
{
register	short		i,x,tt,ii, HH;
			long		zz, oldT = LMGetTicks();
			Ptr			ScreenPtr, CopyPtr;
			Boolean		EndFade = false;
	
	
	HH = MyBounds.right / 20;
	zz = (long) RowBytes * (long) MyBounds.bottom;
	
		ii = 250;
		while( ii-- != 0 && !Button())
		{
			while( LMGetTicks() == oldT);
			oldT = LMGetTicks();
			
			for(i = 0; i < HH; i++)
			{
				tt = RangedRdm( 0, HH);
				ScreenPtr = (Ptr) ( (long)	VA[ 0].quickrow[ tt] + i);
				
				for( tt = 0; tt < MyBounds.bottom; tt += HH)
				{
					CopyPtr = ScreenPtr;
					for( x = 0; x < MyBounds.right; x += HH)
					{
						*ScreenPtr = 0xFF;
						ScreenPtr += HH;
					}
					ScreenPtr = CopyPtr;
					ScreenPtr += (long) HH* (long) RowBytes;
				}
			}
		}
	while( Button());
}

void SousCopyBits32x32( long*	Source, long* Position)
{
	long			i, x, deltaRow = RowBytes/4;
	long			*SourceD = (long*) Source, *PositionD = (long*) Position;


	for( i = 0; i < 32; i++)
	{
		for( x = 0; x < 8; x++) PositionD[ x] = *(SourceD++);
		PositionD += deltaRow;
	}
}

void PaintICL8(void)
{
	#define ICL8TOT	2

		Str255		theString;
		short		iclType, i, xVal, yVal;
		Handle		theIcons;
		Rect		Destination, tempRect;
static	Ptr			Position;
		Boolean		UpAndDown;
		long		StartTicks = 0L, xxx;

	theIcons = GetResource( 'icl8', 128);
	
	DetachResource( theIcons);
	HLock( theIcons);
	
	ForeColor( blackColor);
	PaintRect( &ScreenBounds);
	
	ForeColor( redColor);
	
	pStrcpy( theString, "\pPlayer PRO");
	Destination.left = (qd.screenBits.bounds.right - StringWidth( theString))/2;
	Destination.bottom = qd.screenBits.bounds.bottom/2;
	MoveTo( Destination.left, Destination.bottom);
	DrawString( theString);
	
	Destination.left -= 35;
	Destination.right = Destination.left + 300;
	Destination.bottom += 20;
	Destination.top = Destination.bottom - 100;
	
	tempRect.left = Destination.right - 35;
	tempRect.right = Destination.right;
	tempRect.bottom = Destination.bottom;
	tempRect.top = Destination.bottom - 10;
	
	TextFont( 4);
	TextSize( 9);
	BackColor( blackColor);
	xxx = 0;
	while( !Button())
	{
		xVal = RangedRdm( 1, qd.screenBits.bounds.right - 33);
		yVal = RangedRdm( 1, ScreenBounds.bottom - 33);
		
		xVal /= 4;
		xVal *= 4;
		
		if( (xVal < Destination.left || xVal > Destination.right) ||
			(yVal < Destination.top || yVal > Destination.bottom))
		{
			SousCopyBits32x32( (long*) *theIcons, (long*) (VA[ 0].quickrow[ yVal] + xVal));
			xxx++;
			
		/*	if( Ticks - StartTicks > 120L)
			{
				NumToString( xxx, theString);
				TextBox( theString + 1, theString[ 0], &tempRect, teCenter);
				
				xxx = 0;
				StartTicks = LMGetTicks();
			}	*/
		}
	}
	BackColor( whiteColor);
	HUnlock( theIcons);
	MyDisposHandle( & theIcons);
	
	ForeColor( blackColor);
	ScreenBounds.left--;
	PaintRect( &ScreenBounds);
	ScreenBounds.left++;
}
