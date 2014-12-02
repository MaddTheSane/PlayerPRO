//
//  PPAPPLImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 8/23/14.
//
//

#import "PPAPPLImporter.h"
#include "APPL.h"
#import "Classic_App-Swift.h"

@interface PPAPPLImporter ()

@end


static const OSType MADTypes[] = {'MADK', 'MADI', 'MADF', 'MADG', 'MADH'};

@implementation PPAPPLImporter

- (instancetype)initForPlugIn
{
	return self = [self init];
}

+ (ResourceCount)resourceCount:(OSType)theType
{
	return Count1Resources(theType);
}

- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler
{
	FSRef			fileRef;
	ResFileRefNum	iFileRefI;
	
	CFURLGetFSRef((__bridge CFURLRef)theURL, &fileRef);
	iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
	UseResFile(iFileRefI);
	NSMutableDictionary *applObjectDict = [[NSMutableDictionary alloc] initWithCapacity:5];
	
	for (int i = 0; i < sizeof(MADTypes) / sizeof(MADTypes[0]); i++) {
		OSType currentType = MADTypes[i];
		ResourceCount n = Count1Resources(currentType);
		NSMutableArray *applObjectArray = [[NSMutableArray alloc] initWithCapacity:n];
		for (ResourceCount j = 1; j <= n; j++) {
			Handle resourceHandle = Get1IndResource(currentType, j);

			APPLObject *obj = [[APPLObject alloc] initWithHandle:resourceHandle];
			[applObjectArray addObject:obj];
			ReleaseResource(resourceHandle);
		}
		if (applObjectArray.count > 0) {
			applObjectDict[CFBridgingRelease(UTCreateStringForOSType(currentType))] = [applObjectArray copy];
		}
	}
	
	ImportWindowController *controller = [[ImportWindowController alloc] initWithWindowNibName:@"PPAPPLImporter"];
	controller.resourceReference = iFileRefI;
	[controller addResourceDictionary:applObjectDict];
	[controller beginImportModalSession];
}

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr
{
	FSRef			fileRef;
	ResFileRefNum	iFileRefI;
	BOOL			canOpen = NO;
	
	if (!CFURLGetFSRef((__bridge CFURLRef)theURL, &fileRef))
		return NO;
	iFileRefI = FSOpenResFile(&fileRef, fsRdPerm);
	if (iFileRefI == -1) {
		return NO;
	} else {
		UseResFile(iFileRefI);
		
		if (Count1Resources('MADK') > 0) {
			canOpen = YES;
		} else if (Count1Resources('MADI') > 0) {
			canOpen = YES;
		} else if (Count1Resources('MADH') > 0) {
			canOpen = YES;
		} else if (Count1Resources('MADF') > 0) {
			canOpen = YES;
		} else if (Count1Resources('MADG') > 0) {
			canOpen = YES;
		}
		
		CloseResFile(iFileRefI);
	}
	
	return canOpen;
}

@end
