//
//  PPMusicListDragClass.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

@objc(PPMusicListDragClass) class MusicListDragClass: NSObject, NSPasteboardReading, NSPasteboardWriting, NSSecureCoding {
	let theIndexSet: NSIndexSet
	init(indexSet: NSIndexSet!) {
		theIndexSet = indexSet
		
		super.init()
	}
	
	class func readableTypesForPasteboard(pasteboard: NSPasteboard!) -> [AnyObject]! {
		return [PPMLDCUTI]
	}
	
	func writableTypesForPasteboard(pasteboard: NSPasteboard!) -> [AnyObject]! {
		return [PPMLDCUTI]
	}
	
	class func readingOptionsForType(type: String!, pasteboard: NSPasteboard!) -> NSPasteboardReadingOptions {
		if (type == PPMLDCUTI) {
			return .AsKeyedArchive;
		} else {
			return .AsData;
		}
	}
	
	override init() {
		theIndexSet = NSIndexSet()
		
		super.init()
	}
	
	convenience required init(pasteboardPropertyList propertyList: AnyObject!, ofType type: String!) {
		if (type == PPMLDCUTI) {
			var unArchive = NSKeyedUnarchiver(forReadingWithData: propertyList as NSData)
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
	
	convenience required init(coder aDecoder: NSCoder) {
		self.init(indexSet:aDecoder.decodeObjectForKey(PPMLDCUTI) as NSIndexSet)
	}
	

}
