//
//  PPAPPLImporter.h
//  PPMacho
//
//  Created by C.W. Betts on 8/23/14.
//
//

#import <Cocoa/Cocoa.h>
#import "PPAPPLImporter.h"

EXP @interface PPAPPLImporterViewController : NSWindowController
@property (weak) IBOutlet NSTableView *resourceNamesTable;
@property (weak) IBOutlet NSTableView *resourceIDsTable;
@property (copy) PPComplexImportHandler currentBlock;
@property ResFileRefNum resourceReference;

- (IBAction)importMusicObject:(id)sender;
- (IBAction)cancelImport:(id)sender;

@end
