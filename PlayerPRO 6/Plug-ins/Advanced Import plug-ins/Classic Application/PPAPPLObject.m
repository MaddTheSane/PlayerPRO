//
//  PPAPPLObject.m
//  PPMacho
//
//  Created by C.W. Betts on 9/15/14.
//
//

#import "PPAPPLObject.h"

@interface PPAPPLObject ()
@property (readwrite, copy) NSString *name;
@property (readwrite) ResID resourceID;
@property (readwrite) ResType resourceType;
@property (readwrite) long size;
@end

@implementation PPAPPLObject

- (instancetype)initWithHandle:(Handle)resHandle resourceIndex:(ResourceCount)resIdx
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

- (NSData*)data
{
	Handle myRes = Get1Resource(self.resourceType, self.resourceID);
	DetachResource(myRes);
	
	HLock(myRes);
	NSData *ourData = [[NSData alloc] initWithBytes:*myRes length:GetHandleSize(myRes)];
	HUnlock(myRes);
	DisposeHandle(myRes);

	return ourData;
}

@end
