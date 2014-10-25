//
//  PPAPPLImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 8/23/14.
//
//

#import <Cocoa/Cocoa.h>
#import "PPAPPLImporter.h"

@interface PPAPPLImporterViewController : NSWindowController <NSTableViewDelegate>
@property (weak) IBOutlet NSTableView *resourceNamesTable;
@property (weak) IBOutlet NSTableView *resourceIDsTable;
@property (copy) PPComplexImportHandler currentBlock;
@property ResFileRefNum resourceReference;
@property (strong) NSArray *resourceIDs;
@property (strong) NSArray *resourceTypes;
//- (void)addResourceDictionary:(NSDictionary*)theDict;

- (IBAction)importMusicObject:(id)sender;
- (IBAction)cancelImport:(id)sender;

- (void)addResourceDictionary:(NSDictionary*)theDict;

@end
