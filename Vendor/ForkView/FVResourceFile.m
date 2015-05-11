//
//  FVResourceFile.m
//  ForkView
//
//  Created by Kevin Wojniak on 5/25/11.
//  Copyright 2011 Kevin Wojniak. All rights reserved.
//

#import "FVResourceFile.h"
#import "FVResourceFilePriv.h"
#import "Sys7SwiftSound-Swift.h"

@interface NSError (FVResource)

+ (NSError *)errorWithDescription:(NSString *)description;

@end

@implementation NSError (FVResource)

+ (NSError *)errorWithDescription:(NSString *)description
{
	return [NSError errorWithDomain:@"FVResourceErrorDomain" code:-1 userInfo:[NSDictionary dictionaryWithObject:description forKey:NSLocalizedFailureReasonErrorKey]];
}

@end

struct FVResourceHeader {
	uint32_t dataOffset;
	uint32_t mapOffset;
	uint32_t dataLength;
	uint32_t mapLength;
};

struct FVResourceMap {
	FVResourceHeader headerCopy;
	uint32_t nextMap;
	uint16_t fileRef;
	uint16_t attributes;
	uint16_t typesOffset;
	uint16_t namesOffset;
};

@implementation FVResourceFile
{
    FVDataReader *dataReader_;
    BOOL isResourceFork_;
    FVResourceHeader *header;
    FVResourceMap *map;
    NSArray *types;
}

@synthesize types;

#pragma mark -
#pragma mark Public Methods

- (instancetype)initWithContentsOfURL:(NSURL *)fileURL error:(NSError **)error resourceFork:(BOOL)resourceFork
{
	self = [super init];
	if (self == nil) {
		return nil;
	}
	
	dataReader_ = [FVDataReader dataReader:fileURL resourceFork:resourceFork];
	if (!dataReader_) {
		if (error) {
			*error = [NSError errorWithDescription:@"Bad file."];
		}
		return nil;
	}
	
	header = calloc(1, sizeof(FVResourceHeader));
	if ([self readHeader:header] == NO) {
		if (error) {
			*error = [NSError errorWithDescription:@"Invalid header."];
		}
		return nil;
	}
	
	//NSLog(@"HEADER (%u, %u), (%u, %u)", header->dataOffset, header->dataLength, header->mapOffset, header->mapLength);
	
	if ([self readMap] == NO) {
		if (error) {
			*error = [NSError errorWithDescription:@"Invalid map."];
		}
		return nil;
	}
	
	if ([self readTypes] == NO) {
		if (error) {
			*error = [NSError errorWithDescription:@"Invalid types list."];
		}
		return nil;
	}
	
	// Don't open empty (but valid) resource forks
	if (!types || ![types count]) {
		if (error) {
			*error = [NSError errorWithDescription:@"No resources."];
		}
		return nil;
	}
	
	return self;
}

- (instancetype)initWithContentsOfURL:(NSURL *)fileURL error:(NSError **)error
{
	NSError *tmpError = nil;
    FVResourceFile *file = [[[self class] alloc] initWithContentsOfURL:fileURL error:&tmpError resourceFork:YES];
    if (file) {
        file->isResourceFork_ = YES;
    } else {
		file = [[[self class] alloc] initWithContentsOfURL:fileURL error:&tmpError resourceFork:NO];
	}
	if (file) {
		return file;
	}
	if (error) {
		*error = tmpError;
	}
	return nil;
}

+ (instancetype)resourceFileWithContentsOfURL:(NSURL *)fileURL error:(NSError **)error
{
	return [(FVResourceFile*)[self alloc] initWithContentsOfURL:fileURL error:error];
}

#pragma mark -
#pragma mark Private Methods

- (void)dealloc
{
	if (header) {
		free(header);
	}
	if (map) {
		free(map);
	}
}

- (BOOL)readHeader:(FVResourceHeader *)aHeader
{
	// read the header values
	if (![dataReader_ read:sizeof(aHeader->dataOffset) into:&aHeader->dataOffset] ||
		![dataReader_ read:sizeof(aHeader->mapOffset) into:&aHeader->mapOffset] ||
		![dataReader_ read:sizeof(aHeader->dataLength) into:&aHeader->dataLength] ||
		![dataReader_ read:sizeof(aHeader->mapLength) into:&aHeader->mapLength]) {
		return NO;
	}
	
	// swap from big endian to host
	aHeader->dataOffset = CFSwapInt32BigToHost(aHeader->dataOffset);
	aHeader->mapOffset = CFSwapInt32BigToHost(aHeader->mapOffset);
	aHeader->dataLength = CFSwapInt32BigToHost(aHeader->dataLength);
	aHeader->mapLength = CFSwapInt32BigToHost(aHeader->mapLength);
	
	unsigned fileLength = (unsigned)[dataReader_ length];
	if ((aHeader->dataOffset + aHeader->dataLength > fileLength) || (aHeader->mapOffset + aHeader->mapLength > fileLength)) {
		return NO;
	}
	
	return YES;
}

- (BOOL)readMap
{
	// seek to the map offset
	if (![dataReader_ seekTo:header->mapOffset]) {
		return NO;
	}
	
	// read the map values
	map = calloc(1, sizeof(FVResourceMap));
	if ([self readHeader:&map->headerCopy] == NO) {
		return NO;
	}
	
	// the map contains a copy of the header, or all 0s
	char zeros[16] = {0};
	if (memcmp(&map->headerCopy, header, sizeof(FVResourceHeader)) != 0 && memcmp(&map->headerCopy, zeros, sizeof(zeros)) != 0) {
		printf("Bad match!\n");
	}
	
	if (![dataReader_ read:sizeof(map->nextMap) into:&map->nextMap] ||
		![dataReader_ read:sizeof(map->fileRef) into:&map->fileRef] ||
		![dataReader_ read:sizeof(map->attributes) into:&map->attributes] ||
		![dataReader_ read:sizeof(map->typesOffset) into:&map->typesOffset] ||
		![dataReader_ read:sizeof(map->namesOffset) into:&map->namesOffset]) {
		return NO;
	}
	
	// swap from big endian to host
	map->nextMap = CFSwapInt32BigToHost(map->nextMap);
	map->fileRef = CFSwapInt16BigToHost(map->fileRef);
	map->attributes = CFSwapInt16BigToHost(map->attributes);
	map->typesOffset = CFSwapInt16BigToHost(map->typesOffset);
	map->namesOffset = CFSwapInt16BigToHost(map->namesOffset);
	
	//NSLog(@"MAP (%u, %u, %x, %u, %u)", map->nextMap, map->fileRef, map->attributes, map->typesOffset, map->namesOffset);
	
	return YES;
}

- (BOOL)readTypes
{
	unsigned typesOffset = (unsigned)[dataReader_ position];
	
	uint16_t numberOfTypes;
	if (![dataReader_ read:sizeof(numberOfTypes) into:&numberOfTypes]) {
		return NO;
	}
	numberOfTypes = CFSwapInt16BigToHost(numberOfTypes) + 1;
	
	NSMutableArray *typesTemp = [NSMutableArray array];
	
	for (uint16_t typeIndex=0; typeIndex<numberOfTypes; typeIndex++) {
		uint32_t type;
		uint16_t numberOfResources;
		uint16_t referenceListOffset;
		
		if (![dataReader_ read:sizeof(type) into:&type] ||
			![dataReader_ read:sizeof(numberOfResources) into:&numberOfResources] ||
			![dataReader_ read:sizeof(referenceListOffset) into:&referenceListOffset]) {
			return NO;
		}
		
		type = CFSwapInt32BigToHost(type);
		numberOfResources = CFSwapInt16BigToHost(numberOfResources) + 1;
		referenceListOffset = CFSwapInt16BigToHost(referenceListOffset);
		
		FVResourceType *obj = [[FVResourceType alloc] init];
		obj.type = type;
		obj.count = numberOfResources;
		obj.offset = referenceListOffset;
		[typesTemp addObject:obj];
		
		//NSLog(@"[%u]: %@, %u, %u", typeIndex+1, NSFileTypeForHFSTypeCode(type), numberOfResources, referenceListOffset);
	}
	
	for (FVResourceType *obj in typesTemp) {
		NSMutableArray *resourcesTemp = [NSMutableArray array];
		
		for (uint16_t resIndex=0; resIndex<obj.count; resIndex++) {
			if (![dataReader_ seekTo:typesOffset + obj.offset + (resIndex * 12)]) {
				return NO;
			}
			
			uint16_t resourceID;
			int16_t nameOffset;
			uint8_t attributes;
			uint8_t dataOffsetBytes[3];
			uint32_t resHandle;
			if (![dataReader_ read:sizeof(resourceID) into:&resourceID] ||
				![dataReader_ read:sizeof(nameOffset) into:&nameOffset] ||
				![dataReader_ read:sizeof(attributes) into:&attributes] ||
				![dataReader_ read:sizeof(dataOffsetBytes) into:&dataOffsetBytes] ||
				![dataReader_ read:sizeof(resHandle) into:&resHandle]) {
					return NO;
			}
			
			resourceID = CFSwapInt16BigToHost(resourceID);
			nameOffset = CFSwapInt16BigToHost(nameOffset);
			resHandle = CFSwapInt32BigToHost(resHandle);
			
			uint32_t dataOffset = 0;
			dataOffset |= dataOffsetBytes[0] << 16;
			dataOffset |= dataOffsetBytes[1] << 8;
			dataOffset |= dataOffsetBytes[2];
			
			char name[256];
			uint8_t nameLength = 0;
			if ((nameOffset != -1) && ([dataReader_ seekTo:header->mapOffset + map->namesOffset + nameOffset])) {
				if (![dataReader_ read:sizeof(nameLength) into:&nameLength] || ![dataReader_ read:nameLength into:name]) {
					nameLength = 0;
				}
			}
			name[nameLength] = 0;
			
			uint32_t dataLength = 0;
			if ([dataReader_ seekTo:header->dataOffset + dataOffset] && [dataReader_ read:sizeof(dataLength) into:&dataLength]) {
				dataLength = CFSwapInt32BigToHost(dataLength);
			}
			
			//NSLog(@"%@[%u] %u %s", obj.typeString, resourceID, dataLength, name);
			FVResource *resource = [[FVResource alloc] init];
            resource.ident = resourceID;
            resource.dataSize = dataLength;
            resource.dataOffset = dataOffset + sizeof(dataOffset);
			if (strlen(name)) {
				[resource setName:[NSString stringWithCString:name encoding:NSMacOSRomanStringEncoding]];
			}
			resource.file = self;
			resource.type = obj;
			[resourcesTemp addObject:resource];
		}
		
		NSSortDescriptor *sort = [[NSSortDescriptor alloc] initWithKey:@"ident" ascending:YES];
		[resourcesTemp sortUsingDescriptors:[NSArray arrayWithObject:sort]];
		
		obj.resources = resourcesTemp;
	}		
	
	NSSortDescriptor *sort = [[NSSortDescriptor alloc] initWithKey:@"typeString" ascending:YES selector:@selector(caseInsensitiveCompare:)];
	[typesTemp sortUsingDescriptors:[NSArray arrayWithObject:sort]];
	
	types = typesTemp;
	
	return YES;
}

- (NSData *)dataForResource:(FVResource *)resource
{
	if (![dataReader_ seekTo:header->dataOffset + resource.dataOffset]) {
		return nil;
	}
	NSMutableData *data = [NSMutableData dataWithLength:resource.dataSize];
	if (!data) {
		return nil;
	}
	if (![dataReader_ read:resource.dataSize into:[data mutableBytes]]) {
		return nil;
	}
	return data;
}

- (BOOL)isResourceFork
{
    return isResourceFork_;
}

@end
