//
//  ImportWindowController.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/29/14.
//
//

import Cocoa
import PlayerPROKit

class ImportWindowController: NSWindowController, NSTableViewDelegate {
	@IBOutlet weak var resourceNamesTable: NSTableView? = nil
	@IBOutlet weak var resourceIDsTable: NSTableView? = nil
	var currentBlock: PPComplexImportHandler!
	var resourceReference: ResFileRefNum = 0
	var resourceArray = [APPLObject]()
	var resourceDictionary = [String: [APPLObject]]()
	
	@IBAction func importMusicObject(sender: AnyObject?) {
		
	}
	
	@IBAction func cancelImport(sender: AnyObject?) {
		
	}

	func addResourceDictionary(theDict: NSDictionary) {
		if let aDict = theDict as? [String: [APPLObject]] {
			resourceDictionary = aDict
			for (key, array) in aDict {
				resourceArray += array
			}
		}
	}
	
	/*
	- (void)addResourceDictionary:(NSDictionary*)theDict;
	{
	_resources = theDict;
	NSMutableArray *tmpArray = [[NSMutableArray alloc] initWithCapacity: [_resources.allKeys count]];
	for (NSString *key in _resources) {
	[tmpArray addObjectsFromArray:theDict[key]];
	}
	}
*/
	
    override func windowDidLoad() {
        super.windowDidLoad()
    
        // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    }

	deinit {
		PPCloseResFile(resourceReference)
	}
	
}
