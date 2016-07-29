//
//  PPMusicListDragClass.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

final class MusicListDragClass: NSObject, NSPasteboardReading, NSPasteboardWriting, NSSecureCoding {
	let theIndexSet: IndexSet
	
	@objc init(indexSet: IndexSet) {
		theIndexSet = indexSet
		
		super.init()
	}
	
	class func readableTypes(for pasteboard: NSPasteboard) -> [String] {
		return [PPMLDCUTI]
	}
	
	func writableTypes(for pasteboard: NSPasteboard) -> [String] {
		return [PPMLDCUTI]
	}
	
	class func readingOptions(forType type: String, pasteboard: NSPasteboard) -> NSPasteboardReadingOptions {
		if type == PPMLDCUTI {
			return .asKeyedArchive;
		} else {
			return [];
		}
	}
	
	override init() {
		theIndexSet = IndexSet()
		
		super.init()
	}
	
	@objc convenience required init?(pasteboardPropertyList propertyList: AnyObject, ofType type: String) {
		if type == PPMLDCUTI {
			if let plistData = propertyList as? Data, let unArchive = NSKeyedUnarchiver(forReadingWith: plistData).decodeObject(forKey: PPMLDCUTI) as? IndexSet {
				self.init(indexSet: unArchive)
				return
			}
		}
			
		return nil
	}
	
	@objc func pasteboardPropertyList(forType type: String) -> AnyObject? {
		if type == PPMLDCUTI {
			return NSKeyedArchiver.archivedData(withRootObject: self);
		} else {
			return nil;
		}
	}

	// MARK: Secure Coding protocol
	static let supportsSecureCoding: Bool = true
	
	func encode(with aCoder: NSCoder) {
		aCoder.encode(theIndexSet as NSIndexSet, forKey: PPMLDCUTI)
	}
	
	convenience required init?(coder aDecoder: NSCoder) {
		guard let decoded = aDecoder.decodeObject(forKey: PPMLDCUTI) as? IndexSet else {
			return nil
		}
		self.init(indexSet: decoded)
	}
}
