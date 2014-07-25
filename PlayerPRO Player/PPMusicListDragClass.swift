//
//  PPMusicListDragClass.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

private let UTIArray = [PPMLDCUTI];

class PPMusicListDragClass: NSObject, NSPasteboardReading, NSPasteboardWriting, NSSecureCoding {
	private(set) var theIndexSet: NSIndexSet
	init(indexSet: NSIndexSet!) {
		theIndexSet = indexSet
		
		super.init()
	}
	
	class func readableTypesForPasteboard(pasteboard: NSPasteboard!) -> [AnyObject]! {
		return UTIArray
	}
	
	func writableTypesForPasteboard(pasteboard: NSPasteboard!) -> [AnyObject]! {
		return UTIArray
	}
	
	class func readingOptionsForType(type: String!, pasteboard: NSPasteboard!) -> NSPasteboardReadingOptions {
		if (type == PPMLDCUTI) {
			return .AsKeyedArchive;
		} else {
			return .AsData;
		}
	}
	
	init() {
		theIndexSet = NSIndexSet()
		
		super.init()
	}
	
	convenience init(pasteboardPropertyList propertyList: AnyObject!, ofType type: String!) {
		if (type == PPMLDCUTI) {
			var unArchive = NSUnarchiver(forReadingWithData: propertyList as NSData)
			self.init(indexSet:unArchive.decodeObjectForKey(PPMLDCUTI) as NSIndexSet)
		} else {
			self.init()
		}
	}
	
	func pasteboardPropertyListForType(type: String) -> AnyObject? {
		if (type == PPMLDCUTI) {
			return NSKeyedArchiver.archivedDataWithRootObject(self);
		} else {
			return nil;
		}
	}

	class func supportsSecureCoding() -> Bool {
		return true
	}
	
	func encodeWithCoder(aCoder: NSCoder) {
		aCoder.encodeObject(theIndexSet, forKey: PPMLDCUTI)
	}
	
	convenience init(coder aDecoder: NSCoder!) {
		self.init(indexSet:aDecoder.decodeObjectForKey(PPMLDCUTI) as NSIndexSet)
	}
	

}
