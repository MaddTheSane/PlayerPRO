//
//  PPAPPLImporter.m
//  PPMacho
//
//  Created by C.W. Betts on 8/23/14.
//
//

#import "PPAPPLImporter.h"

@interface PPAPPLImporter ()

@end

@implementation PPAPPLImporter

- (void)windowDidLoad {
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (BOOL)canImportURL:(NSURL *)theURL
{
	return NO;
}

- (void)beginImportOfURL:(NSURL*)theURL withHandler:(PPComplexImportHandler)handler
{
	
}

@end
