//
//  PPAPPLImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 8/23/14.
//
//

#import <Cocoa/Cocoa.h>
#import "PPComplexImportPlugInterface.h"

@interface PPAPPLImporter : NSWindowController <PPComplexImportPlugInterface, NSTableViewDelegate>

- (IBAction)importMusicObject:(id)sender;
- (IBAction)cancelImport:(id)sender;

- (BOOL)canImportURL:(NSURL*)theURL error:(out NSError**)outErr;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL;
- (NSDictionary*)getTrackerInformationFromURL:(NSURL*)theURL error:(out NSError**)outErr;


@end
