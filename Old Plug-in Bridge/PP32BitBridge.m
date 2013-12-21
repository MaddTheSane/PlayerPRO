//
//  PP32BitBridge.m
//  PPMacho
//
//  Created by C.W. Betts on 12/21/13.
//
//

#import "PP32BitBridge.h"

@implementation PP32BitBridge

- (id)init
{
	if (self = [super init]) {
		const SInt32 mytab[12] =
		{
			1712*16, 1616*16, 1524*16, 1440*16, 1356*16, 1280*16,
			1208*16, 1140*16, 1076*16, 1016*16, 960*16, 907*16
		};
		the32BitLibrary = (MADLibrary*)calloc(sizeof(MADLibrary), 1);
		
		if (the32BitLibrary == NULL) {
			[self autorelease];
			return nil;
		}
		
		the32BitLibrary->IDType = 'MADD';
		
		//for (int i = 0; i < 12; i++)
		//	the32BitLibrary->mytab[i] = mytab[i];
		memcpy(the32BitLibrary->mytab, mytab, sizeof(mytab));
		
		the32BitLibrary->ThePlug = calloc(sizeof(MADLibrary), MAXPLUG);
		the32BitLibrary->TotalPlug = 0;
	}
	return self;
}

- (BOOL)addPlugInAtPath:(NSURL*)plugPath
{
	return MakeMADPlugFromNSURL(the32BitLibrary, plugPath);
}

- (void)dealloc
{
	if (the32BitLibrary) {
		MADDisposeLibrary(the32BitLibrary);
	}
	
	[super dealloc];
}

@end
