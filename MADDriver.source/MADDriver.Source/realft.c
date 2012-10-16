/********************						***********************/
//
//	Player PRO 5.9 - DRIVER SOURCE CODE -
//
//	Library Version 5.9
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	20 Micheli-Du-Crest
//	1205 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998, 1999, 2000, 2001, 2002
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		RossetAntoine@bluewin.ch
//
/********************						***********************/

#include <math.h>
#include "RDriver.h"
#include "RDriverInt.h"

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

long Interpolate(SInt32 p, SInt32 p1, SInt32 p2, SInt32 v1, SInt32 v2);
double EQInterpolate(double p,double p1,double p2,double v1,double v2);

double EQInterpolate(double p,double p1,double p2,double v1,double v2)
{
	double dp,dv,di;

	if( p1 == p2) return v1;

	dv=v2-v1;
	dp=p2-p1;
	di=p-p1;

	return v1 + ((di*dv) / dp);
}

static short *SDataInter;

OSErr MADInitEqualizer( MADDriverRec *intDriver)
{
	long i;
	
	intDriver->Filter	= (double*) calloc( sizeof( double) * ((EQPACKET*2)+2), 1);
	intDriver->fData	= (double*) calloc( sizeof( double) * ((EQPACKET*2)+2), 1);
	

	
	if( intDriver->Filter == NULL) return -1;
	
	for( i = 0; i <= EQPACKET*2; i++)
	{
		intDriver->Filter[ i] = 1.0;
	}
	
	return noErr;
}

void MADCloseEqualizer( MADDriverRec *intDriver)
{
	free( (Ptr) intDriver->Filter);
	free( (Ptr) intDriver->fData);
}

void MADfour1( double *data,int nn,int isign)
{
	int 		n,mmax,m,j,istep,i;
	double 		wtemp,wr,wpr,wpi,wi,theta;
	double 		tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=2*mmax;
		
		theta=M_PI/(isign*mmax);
		wtemp=sin(theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(2.0*theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

#undef SWAP


void MADrealft(double *data,int n,int isign)
{
	int 		i,i1,i2,i3,i4,n2p3;
	double 		c1=0.5,c2,h1r,h1i,h2r,h2i;
	double 		wr,wi,wpr,wpi,wtemp,theta;

	theta=M_PI/(double) n;
	if (isign == 1) {
		c2 = -0.5;
		MADfour1(data,n,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	n2p3=2*n+3;
	for (i=2;i<=n/2;i++) {
		i4=1+(i3=n2p3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i;
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r-data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		MADfour1(data,n,-1);
	}
}

/*void twofft(double *data1,double *data2,double *fft1,double *fft2, int n)
{
	int nn3,nn2,jj,j;
	double rep,rem,aip,aim;

	nn3=1+(nn2=2+n+n);
	for (j=1,jj=2;j<=n;j++,jj+=2) {
		fft1[jj-1]=data1[j];
		fft1[jj]=data2[j];
	}
	MADfour1(fft1,n,1);
	fft2[1]=fft1[2];
	fft1[2]=fft2[2]=0.0;
	for (j=3;j<=n+1;j+=2) {
		rep=0.5*(fft1[j]+fft1[nn2-j]);
		rem=0.5*(fft1[j]-fft1[nn2-j]);
		aip=0.5*(fft1[j+1]+fft1[nn3-j]);
		aim=0.5*(fft1[j+1]-fft1[nn3-j]);
		fft1[j]=rep;
		fft1[j+1]=aim;
		fft1[nn2-j]=rep;
		fft1[nn3-j] = -aim;
		fft2[j]=aip;
		fft2[j+1] = -rem;
		fft2[nn2-j]=aip;
		fft2[nn3-j]=rem;
	}
}*/

void MADCallFFT( sData *SData, double *filter, MADDriverRec *intDriver, Boolean shift)
{
	if( filter == 0) filter = intDriver->Filter;

	switch( SData->amp)
	{
		case 8:
			if( SData->stereo) FFT8S( SData->data, SData->size, filter, intDriver, 2, shift);
			else FFT8S( SData->data, SData->size, filter, intDriver, 1, shift);
		break;
		
		case 16:
			if( SData->stereo) FFT16S( (short*) SData->data, SData->size, filter, intDriver, 2, shift);
			else FFT16S( (short*) SData->data, SData->size, filter, intDriver, 1, shift);
		break;
	}
}

/*double MADEQInterpolate(double p,double p1,double p2,double v1,double v2)
{
	double dp,dv,di;

	if( p1 == p2) return v1;

	dv=v2-v1;
	dp=p2-p1;
	di=p-p1;

	return v1 + ((di*dv) / dp);
}*/

void FFT8S( char* SData, size_t size, double *filter, MADDriverRec *intDriver, short nochan, Boolean shift)
{
	long	i, y, powersize;
	long	*shiftAr;
	double	pente, axe, *fDataCopy2, *fDataCopy = intDriver->fData;
	
	if( nochan == 2)	// STEREO
	{
		if( size != EQPACKET*2*2)
		{
			powersize = 1;
			do
			{
				powersize *= 2;
			}while( powersize < size/2);
			
			fDataCopy = (double*) malloc( sizeof( double) * (powersize+2));
		}
		else powersize = EQPACKET*2;
	}
	else
	{
		if( size != EQPACKET*2)
		{
			powersize = 1;
			do
			{
				powersize *= 2;
			}while( powersize < size);
			
			fDataCopy = (double*) malloc( sizeof( double) * (powersize+2));
		}
		else powersize = EQPACKET*2;
	}
	
	if( shift)
	{
		fDataCopy2 = (double*) malloc( sizeof( double) * (powersize+2));
		if( fDataCopy2 == NULL) return;
		
		shiftAr = (long*) calloc( sizeof( long) * (powersize+2), 1);
		if( shiftAr == NULL) return;
	}
	
	if( fDataCopy == NULL) return;
	
	for( y = 0; y < nochan; y++)
	{
		// Copy data
		
		if( nochan == 2)	// STEREO
		{
			if( powersize*2 > size)
			{
				for( i = 0 ; i < size/2; i++) fDataCopy[ i+1] = SData[ 2*i];
				for( i = size/2 ; i < powersize; i++) fDataCopy[ i+1] = 0;
			}
			else
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i+1] = SData[ 2*i];
			}
		}
		else				// MONO
		{
			if( powersize > size)
			{
				for( i = 0 ; i < size; i++) fDataCopy[ i+1] = SData[ i];
				for( i = size ; i < powersize; i++) fDataCopy[ i+1] = 0;
			}
			else
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i+1] = SData[ i];
			}
		}
		
		// First and last values MUST be zero! // ** //  // ** //  // ** //
		pente = (fDataCopy[ powersize] - fDataCopy[ 1]) / (double) (powersize -1);
		axe = fDataCopy[ 1];
		
		for( i = 1 ; i <= powersize; i++)
		{
			fDataCopy[ i] -= (axe + (double)(i-1) * pente);
		}
		// ** //  // ** //  // ** //  // ** //  // ** //  // ** //  // ** //
		
		MADrealft( fDataCopy, powersize/2, true);
		
		if( shift)
		{
			long shiftval, a;//, b;
			
			for( i = 0 ; i < powersize; i++)
			{
				a = (i * EQPACKET*2) / powersize;
			//	b = a+1;
				
				if( a+1 < powersize)
				{
					shiftAr[ i] = (EQInterpolate( (double) (i * EQPACKET*2) / (double) powersize, a, a+1, filter[ a], filter[ a+1]) * powersize) / (EQPACKET*2);
				}
				else
				{
					shiftAr[ i] = (filter[ a] * powersize) / (EQPACKET*2);
				}
				fDataCopy2[ i] = 0;
			}
			
			for( i = 0 ; i < powersize; i++)
			{
				if( shiftAr[ i] >= 0 && shiftAr[ i] < powersize) fDataCopy2[ shiftAr[ i] +1] += fDataCopy[ i +1];
			}
			
			memcpy( fDataCopy, fDataCopy2, sizeof( double) * (powersize+2));
		}
		else
		{
			if( size != EQPACKET*2*2)
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i +1] *= filter[ (i * EQPACKET*2) / powersize];
			}
			else
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i +1] *= filter[ i];
			}
		}
		
		MADrealft( fDataCopy, powersize/2, false);
		
		// First and last values MUST be zero! // ** //  // ** //  // ** //
		pente = (fDataCopy[ powersize] - fDataCopy[ 1]) / (double) (powersize -1);
		axe = fDataCopy[ 1];
		
		for( i = 1 ; i <= powersize; i++)
		{
			fDataCopy[ i] -= (axe + (double)(i-1) * pente);
		}
		// ** //  // ** //  // ** //  // ** //  // ** //  // ** //  // ** //
		   
		for( i = 1 ; i <= powersize; i++) fDataCopy[ i] /= powersize/2;
		
		// Check data
		for( i = 1 ; i <= powersize; i++)
		{
			if( fDataCopy[ i] > 127) fDataCopy[ i] = 127;
			if( fDataCopy[ i] < -127) fDataCopy[ i] = -127;
		}
		
		// Restore data
		if( nochan == 2)	// STEREO
		{
			if( powersize*2 > size)
			{
				for( i = 0 ; i < size/2; i++) SData[ 2*i] = fDataCopy[ i+1];
			}
			else
			{
				for( i = 0 ; i < powersize; i++) SData[ 2*i] = fDataCopy[ i+1];
			}
		}
		else				// MONO
		{
			if( powersize > size)
			{
				for( i = 0 ; i < size; i++) SData[ i] = fDataCopy[ i+1];
			}
			else
			{
				for( i = 0 ; i < powersize; i++) SData[ i] = fDataCopy[ i+1];
			}
		}
		
		//// ***********************              Now the left channel !
		
		SData++;
	}
	
	if( nochan == 2) SData -= 2;
	else SData--;
	
	if( size != EQPACKET*2*2)
	{
		free( (Ptr) fDataCopy);
		fDataCopy = NULL;
		
		if( shift) free( (Ptr) fDataCopy2);
		
		if( shift) free( (Ptr) shiftAr);
	}
}

//static long PreviousAxe[ 2];
//static long PreviousAxe2[ 2];

void FFT16S( short* SData, size_t size, double *filter, MADDriverRec *intDriver, short nochan, Boolean shift)
{
	long	i, y, powersize, *shiftAr;
	double	pente, axe, *fDataCopy2, *fDataCopy = intDriver->fData;
	
	size /= 2;
	
	if( nochan == 2)	// STEREO
	{
		if( size != EQPACKET*2*2)
		{
			powersize = 1;
			do
			{
				powersize *= 2;
			}while( powersize < size/2);
			
			fDataCopy = (double*) malloc( sizeof( double) * (powersize+2));
		}
		else powersize = EQPACKET*2;
	}
	else
	{
		if( size != EQPACKET*2)
		{
			powersize = 1;
			do
			{
				powersize *= 2;
			}while( powersize < size);
			
			fDataCopy = (double*) malloc( sizeof( double) * (powersize+2));
		}
		else powersize = EQPACKET*2;
	}
	
	if( shift)
	{
		fDataCopy2 = (double*) malloc( sizeof( double) * (powersize+2));
		if( fDataCopy2 == NULL) return;
		
		shiftAr = (long*) calloc( sizeof( long) * (powersize+2), 1);
		if( shiftAr == NULL) return;
	}
	
	if( fDataCopy == NULL) return;
	
	for( y = 0; y < nochan; y++)
	{
		// Copy data
		
		if( nochan == 2)	// STEREO
		{
			if( powersize*2 > size)
			{
				for( i = 0 ; i < size/2; i++) fDataCopy[ i+1] = SData[ 2*i];
				for( i = size/2 ; i < powersize; i++) fDataCopy[ i+1] = 0;
			}
			else
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i+1] = SData[ 2*i];
			}
		}
		else				// MONO
		{
			if( powersize > size)
			{
				for( i = 0 ; i < size; i++) fDataCopy[ i+1] = SData[ i];
				for( i = size ; i < powersize; i++) fDataCopy[ i+1] = 0;
			}
			else
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i+1] = SData[ i];
			}
		}
		
		// First and last values MUST be zero! // ** //  // ** //  // ** //
		
		
		
	//		pente = (fDataCopy[ powersize] - PreviousAxe[ y]) / (double) (powersize -1);
	//		axe = PreviousAxe[ y];
	//		PreviousAxe[ y] = fDataCopy[ powersize];
		
		pente = (fDataCopy[ powersize] - fDataCopy[ 1]) / (double) (powersize -1);
		axe = fDataCopy[ 1];
		
	/*	NumToString( axe, str);
		DebugStr( str);
		NumToString( PreviousAxe[ y], str);
		DebugStr( str);*/
		
		for( i = 1 ; i <= powersize; i++)
		{
			fDataCopy[ i] -= (axe + (double)(i-1) * pente);
		}
		// ** //  // ** //  // ** //  // ** //  // ** //  // ** //  // ** //
		
		MADrealft( fDataCopy, powersize/2, true);
		
		if( shift)
		{
			long shiftval, a, b;
			
			for( i = 0 ; i < powersize; i++)
			{
				a = (i * EQPACKET*2) / powersize;
				b = a+1;
				
				shiftAr[ i] = (EQInterpolate( (double) (i * EQPACKET*2) / (double) powersize, a, a+1, filter[ a], filter[ a+1]) * powersize) / (EQPACKET*2);
				fDataCopy2[ i] = 0;
			}
			
			for( i = 0 ; i < powersize; i++)
			{
				if( shiftAr[ i] >= 0 && shiftAr[ i] < powersize) fDataCopy2[ shiftAr[ i] +1] += fDataCopy[ i +1];
			}
			
			memcpy( fDataCopy, fDataCopy2, sizeof( double) * (powersize+2));
		}
		else
		{
			if( size != EQPACKET*2*2)
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i +1] *= filter[ (i * EQPACKET*2) / powersize];
			}
			else
			{
				for( i = 0 ; i < powersize; i++) fDataCopy[ i +1] *= filter[ i];
			}
		}
		
		MADrealft( fDataCopy, powersize/2, false);
		
		for( i = 1 ; i <= powersize; i++) fDataCopy[ i] /= powersize/2;

		// First and last values MUST be zero! // ** //  // ** //  // ** //
		pente = (fDataCopy[ powersize] - fDataCopy[ 1]) / (double) (powersize -1);
		axe = fDataCopy[ 1];
		
	//		pente = (fDataCopy[ powersize] - PreviousAxe2[ y]) / (double) (powersize -1);
	//		axe = PreviousAxe2[ y];
	//		PreviousAxe2[ y] = fDataCopy[ powersize];
		
		for( i = 1 ; i <= powersize; i++)
		{
			fDataCopy[ i] -= (axe + (double)(i-1) * pente);
		}
		// ** //  // ** //  // ** //  // ** //  // ** //  // ** //  // ** //
		   
		
	/*	if( fDataCopy[ 1] != 0) Debugger();
		if( fDataCopy[ powersize]  != 0)
		{
			NumToString( fDataCopy[ powersize], str);
			DebugStr( str);
		}*/
		
		// Check data
		for( i = 1 ; i <= powersize; i++)
		{
			if( fDataCopy[ i] > 32767) fDataCopy[ i] = 32767;
			if( fDataCopy[ i] < -32767) fDataCopy[ i] = -32767;
		}
		
		// Restore data
		if( nochan == 2)	// STEREO
		{
			if( powersize*2 > size)
			{
				for( i = 0 ; i < size/2; i++) SData[ 2*i] = fDataCopy[ i+1];
			}
			else
			{
				for( i = 0 ; i < powersize; i++) SData[ 2*i] = fDataCopy[ i+1];
			}
		}
		else				// MONO
		{
			if( powersize > size)
			{
				for( i = 0 ; i < size; i++) SData[ i] = fDataCopy[ i+1];
			}
			else
			{
				for( i = 0 ; i < powersize; i++) SData[ i] = fDataCopy[ i+1];
			}
		}
		
		//// ***********************              Now the left channel !
		
		SData++;
	}
	
	if( nochan == 2) SData -= 2;
	else SData--;
	
	if( size != EQPACKET*2*2)
	{
		free( (Ptr) fDataCopy);
		fDataCopy = NULL;
		
		if( shift) free( (Ptr) fDataCopy2);
		if( shift) free( (Ptr) shiftAr);
	}
}

/*void FFT8S( char	*SData, long size, double *filter, MADDriverRec *intDriver, short nochan)
{
	long	i, x, y;
	
	for( x = 0; x < size; x += EQPACKET*2*2)
	{
		for( y = 0; y < 2; y++)
		{
			// Copy data
			
			//ALIGNEMENT DES DATAS: PREMIER ET DERNIER DOIVENT ETRE EGAL A ZERO!
			
			if( x + EQPACKET*2*2 > size)
			{
				for( i = 0 ; i < (size - x)/2; i++) intDriver->fData[ i+1] = SData[ x + 2*i];
				for( i = (size - x)/2 ; i < EQPACKET*2; i++) intDriver->fData[ i+1] = 0;
			}
			else
			{
				for( i = 0 ; i < EQPACKET*2; i++) intDriver->fData[ i+1] = SData[ x + 2*i];
			}
			
			MADrealft( intDriver->fData, EQPACKET, true);
			
			MADrealft( intDriver->fData, EQPACKET, false);
			
			for( i = 1 ; i <= EQPACKET*2; i++) intDriver->fData[ i] /= EQPACKET;
			
			// Check data
			for( i = 1 ; i <= EQPACKET*2; i++)
			{
				if( intDriver->fData[ i] > 127) intDriver->fData[ i] = 127;
				if( intDriver->fData[ i] < -127) intDriver->fData[ i] = -127;
			}
			
			// Restore data
			if( x + EQPACKET*2*2 > size)
			{
				for( i = 0 ; i < (size - x)/2; i++) SData[ x + 2*i] = intDriver->fData[ i+1];
			}
			else
			{
				for( i = 0 ; i < EQPACKET*2; i++) SData[ x + 2*i] = intDriver->fData[ i+1];
			}
			
			//// ***********************              Now the left channel !
			
			SData++;
		}
		
		SData -= 2;
	}
}*/
