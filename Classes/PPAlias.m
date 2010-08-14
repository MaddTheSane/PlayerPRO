//
//  Alias.m
//  Slide Master
//
//  Created by C.W. Betts on 11/17/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "PPAlias.h"


@implementation PPAlias

-(id)initWithPath:(NSString *)path
{
	self = [super init];
	
	if (self) {
		FSRef target;
		OSStatus err;
		//Convert this path to an FSRef so you can call Carbon File Manager routines
		err = FSPathMakeRef((UInt8*)[path UTF8String], &target, NULL);
		
		//create a new alias
		if (err == noErr)
		{
			err = FSNewAlias(NULL, &target, &mAliasHandle);
		}
	}
	
	return self;
}

-(BOOL)isEqual:(id)object
{
	if (![object isKindOfClass:[PPAlias class]]) {
		return FALSE;
	}
	NSURL *compareURL = [object url];
	return [compareURL isEqual:[self url]];
}

-(id)initWithURL:(NSURL *)path
{
	self = [super init];
	
	if (self) {
		FSRef target;
		OSStatus err;
		//Convert this URL to an FSRef so you can call Carbon File Manager routines
		err = CFURLGetFSRef((CFURLRef)path, &target);
		
		//create a new alias
		if (err == noErr)
		{
			err = FSNewAlias(NULL, &target, &mAliasHandle);
		}
	}
	
	return self;
}

-(void)finalize
{
	if (mAliasHandle)
		DisposeHandle((Handle)mAliasHandle);
	
	[super finalize];
}

-(void)dealloc
{
	if (mAliasHandle)
		DisposeHandle((Handle)mAliasHandle);
		
	[super dealloc];
}

-(NSURL *)url{
	return [NSURL URLWithString:[self path]];
}

-(NSString *)path
{
	NSString *path = nil;
	FSRef target;
	Boolean wasChanged;
	
	//Resolve this alias and return the result into a Carbon FSRef
	OSStatus err = FSResolveAlias(NULL, mAliasHandle, &target, &wasChanged);
	// initialize a UTF8 buffer from the FSRef's path
	if (err == noErr)
	{
		UInt8 buffer[PATH_MAX * 2];
		
		err = FSRefMakePath(&target, buffer, PATH_MAX * 2);
		
		//create a new path string from the UTF8 buffer
		if (err==noErr)
			path = [NSString stringWithUTF8String:(char*)buffer];
	}
	return path;
}

#pragma mark NSCoding Protocol

-(id)initWithCoder:(NSCoder *)decoder
{
	if ((self = [super init])) 
	{
		NSData *aliasData;
		
		//decode our alias data
		if ([decoder allowsKeyedCoding])
		{	aliasData = [decoder decodeObjectForKey:@"AliasHandle"];}
		else 
		{	aliasData = [decoder decodeObject];}
		
		//convert the alias data into an alias handle
		
		if (aliasData)
		{
			mAliasHandle = (AliasHandle)NewHandle([aliasData length]);
			if (mAliasHandle){
				[aliasData getBytes:*mAliasHandle ];
			}
		}
		// If an error happened, destroy the object and return nil
		if (!mAliasHandle){
			[self autorelease];
			self = nil;
		}
	}
	return self;
}

-(void)encodeWithCoder:(NSCoder *)encoder
{
	//create an NSData object of our alias data
	NSData *aliasData;
	
	aliasData = [NSData dataWithBytes:*mAliasHandle length:GetHandleSize((Handle)mAliasHandle)];
	
	//encode our alias data
	if ([encoder allowsKeyedCoding]) {
		[encoder encodeObject:aliasData forKey:@"AliasHandle"];
	} else {
		[encoder encodeObject:aliasData];
	}
}

@end
