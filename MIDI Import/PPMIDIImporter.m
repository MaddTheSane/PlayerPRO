//
//  PPMIDIImportHelper.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#import "PPMIDIImporter.h"

@implementation PPMIDIImporter

+ (instancetype)sharedImporter
{
	static dispatch_once_t onceToken;
	static PPMIDIImporter *shared;
	dispatch_once(&onceToken, ^{
		shared = [PPMIDIImporter new];
	});
	return shared;
}


- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
	newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPMIDIImportHelper)];
	newConnection.exportedObject = self;
	[newConnection resume];
	
	return YES;
}


@end
