//
//  PPAPPLObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/15/14.
//
//

#import "PPAPPLObject.h"

@interface PPAPPLObject ()
@property (readwrite) ResID resourceID;
@property (readwrite) ResType resourceType;
@property (readwrite) long size;
@property (readwrite, copy) NSString *name;
@end

@implementation PPAPPLObject

- (instancetype)initWithHandle:(Handle)resHandle
{
	if (self = [super init]) {
		Str255 nameStr;
		ResID resIDShort;
		ResType resTypeCode;
		GetResInfo(resHandle, &resIDShort, &resTypeCode, nameStr);
		self.size = GetResourceSizeOnDisk(resHandle);
		self.name = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, nameStr, kCFStringEncodingMacRoman));
		self.resourceID = resIDShort;
		self.resourceType = resTypeCode;
	}
	
	return self;
}

@end
