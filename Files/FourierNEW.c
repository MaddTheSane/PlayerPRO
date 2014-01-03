#include "Shuddup.h"

#include "MAD.h"
#include "RDriver.h"
#include "VA.h"
#include <math.h>

static		Byte			*xInt;
static		float			*x;
static		short			*logoScale;

#define		PI      		M_PI
#define		PI2				(M_PI * 2)

void InitFourier(void)
{
	short	i;
	Handle	rsrc;

	x = (float*)			NewPtrClear(512 * sizeof(float));
	xInt = (Byte*)			NewPtrClear(512 * sizeof(char));
	logoScale = (short*)	NewPtrClear(256 * sizeof(short));
	
	rsrc = GetResource('LogC', 128);
	DetachResource(rsrc);
	
	HLock(rsrc);
	BlockMoveData(*rsrc, logoScale, 256 * sizeof(short));
	HUnlock(rsrc);
	DisposeHandle(rsrc);
	
/*	for (i = 0; i < 256; i++)
	{
		double t = i;
		
		logoScale[ 255 - i] = 255 - (255. * (log10(1. + (t * 22254.) / 255.)))/4.34740808371;
	}*/
}

long LogChangePos(long x)
{
	return logoScale[ x];
}

void CloseFourier(void)
{
	if (x != NULL) DisposePtr((Ptr) x);					x = NULL;
	if (xInt != NULL) DisposePtr((Ptr) xInt);				xInt = NULL;
	if (logoScale != NULL) DisposePtr((Ptr) logoScale);	logoScale = NULL;
}

#define SWAP(a, b) tempr = (a); (a) = (b);  (b) = tempr
void four1(float *data, unsigned long nn, int isign)
{
	unsigned long	n, mmax, m, j, istep, i;
	float			wtemp, wr, wpr, wpi, wi, theta;
	float			tempr, tempi;
	
	n = nn << 1;
	j = 1;
	for (i = 1; i < n; i+= 2)
	{
		if (j > i)
		{
			SWAP(data[ j], data[ i]);
			SWAP(data[ j+1], data[ i+1]);	
		}
		
		m = n >> 1;
		while (m >= 2 && j > m)
		{
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	
	mmax = 2;
	while (n > mmax)
	{
		istep = mmax << 1;
		theta = isign * (PI2 / mmax);
		wtemp = sin(0.5 * theta);
		wpr = -2.0*wtemp*wtemp;
		
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		
		for (m = 1; m < mmax; m+= 2)
		{
			for (i=m; i <= n; i+= istep)
			{
				j = i + mmax;
				tempr = wr*data[ j] - wi*data[ j + 1];
				tempi = wr*data[ j + 1] + wi*data[ j];
				
				data[ j] 		= data[ i] - tempr;
				data[ j + 1] 	= data[ i + 1] - tempi;
				data[ i] 		+= tempr;
				data[ i+1] 		+= tempi;
			}
			wr = (wtemp=wr)*wpr - wi*wpi + wr;
			wi = wi*wpr + wtemp*wpi + wi;
		}
		mmax = istep;
	}
}

void realft(float *data, unsigned long n, int isign)
{
	unsigned long	i, i1, i2, i3, i4, np3;
	float			c1 = 0.5, c2, h1r, h1i, h2r, h2i;
	float			wr, wi, wpr, wpi, wtemp, theta;
	
	theta = PI / (double) (n>> 1);
	
	if (isign == 1)
	{
		c2 = -0.5;
		four1(data, n>>1, 1);
	}
	else
	{
		c2 = 0.5;
		theta = -theta;
	}
	
	wtemp	= sin(0.5*theta);
	wpr		= -2.0*wtemp*wtemp;
	wpi		= sin(theta);
	wr		= 1.0 + wpr;
	wi 		= wpi;
	np3		= n + 3;
	for (i = 2; i <= (n>>2); i++)
	{
		i4	= 1 + (i3=np3 - (i2=1 +(i1=i+i-1)));
		
		h1r	= c1*(data[ i1] + data[i3]);
		h1i	= c1*(data[ i2] - data[i4]);
		
		h2r	= -c2*(data[ i2] + data[ i4]);
		h2i	= c2*(data[ i1] - data[ i3]);
		
		data[ i1] = h1r + wr*h2r - wi*h2i;
		data[ i2] = h1i + wr*h2i + wi*h2r;
		data[ i3] = h1r - wr*h2r + wi*h2i;
		data[ i4] = -h1i + wr*h2i + wi*h2r;
		
		wr	= (wtemp = wr)*wpr - wi*wpi + wr;
		wi	= wi*wpr + wtemp*wpi + wi;
	}
	
	if (isign == 1)
	{
		data[ 1]	= (h1r=data[1]) + data[ 2];
		data[ 2]	= h1r - data[ 2];
	}
	else
	{
		data[ 1]	= c1*((h1r = data[1]) + data[2]);
		data[ 2]	= c1*(h1r - data[ 2]);
		four1(data, n>>1, -1);
	}
}

Ptr MakeCalculusSpectrum(Ptr srcPtr, Boolean logScale)
{
	short				i, xx, lastLog, temp;
	
	for(i=0; i<256; i++) x[ i + 1] = *((Byte*)srcPtr++);
	
	realft(x, 256, 1);
	
	for(i= 1; i<=256; i++)
	{
		temp = x[ i];
		temp >>= 3;
		
		if (temp < 0) temp = -temp;
		
		if (temp > 255) temp = 255;
		//	else temp = logoScale[ temp];
		
		xInt[ i-1] = temp;
	}
	xInt[ 0] = 0;
	
	if (logScale)
	{
		long	sumL;
		
		lastLog = logoScale[ 255];
		
		for(i= 255; i >= 0; i--)
		{
			
			if (logoScale[ i] == lastLog) temp = xInt[ logoScale[ i]];
			else for (xx = logoScale[ i], temp = 0; xx < lastLog; xx++) temp += xInt[ xx];
			
			//	FINIR ICI
			
			xInt[ i]  = temp;
			
			lastLog = logoScale[ i];
		}
		
		/*	for (xx = 0, sumL = 0, i = 0; i < 256; i++)
		 {
		 if (xx == 16)
		 {
		 sumL /= 16;
		 
		 for (xx = i - 16; xx < i; xx++)
		 {
		 xInt[ xx] = sumL;
		 }
		 
		 xx = 0;
		 sumL = 0;
		 }
		 
		 xx++;
		 sumL += xInt[ i];
		 }*/
	}
	
	return (Ptr) xInt;
}
