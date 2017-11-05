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
	
	class func readableTypes(for pasteboard: NSPasteboard) -> [NSPasteboard.PasteboardType] {
		return [.PPMLDCUTI]
	}
	
	func writableTypes(for pasteboard: NSPasteboard) -> [NSPasteboard.PasteboardType] {
		return [.PPMLDCUTI]
	}

	class func readingOptions(forType type: NSPasteboard.PasteboardType, pasteboard: NSPasteboard) -> NSPasteboard.ReadingOptions {
		if type == .PPMLDCUTI {
			return .asKeyedArchive
		} else {
			return []
		}
	}
	
	override init() {
		theIndexSet = IndexSet()
		
		super.init()
	}
	
	convenience required init?(pasteboardPropertyList propertyList: Any, ofType type: NSPasteboard.PasteboardType) {
		if type == .PPMLDCUTI {
			if let plistData = propertyList as? Data, let unArchive = NSKeyedUnarchiver(forReadingWith: plistData).decodeObject(forKey: NSPasteboard.PasteboardType.PPMLDCUTI.rawValue) as? IndexSet {
				self.init(indexSet: unArchive)
				return
			}
		}
			
		return nil
	}
	
	func pasteboardPropertyList(forType type: NSPasteboard.PasteboardType) -> Any? {
		if type == .PPMLDCUTI {
			return NSKeyedArchiver.archivedData(withRootObject: self)
		} else {
			return nil
		}
	}

	// MARK: Secure Coding protocol
	static let supportsSecureCoding: Bool = true
	
	func encode(with aCoder: NSCoder) {
		aCoder.encode(theIndexSet, forKey: NSPasteboard.PasteboardType.PPMLDCUTI.rawValue)
	}
	
	convenience required init?(coder aDecoder: NSCoder) {
		guard let decoded = aDecoder.decodeObject(forKey: NSPasteboard.PasteboardType.PPMLDCUTI.rawValue) as? IndexSet else {
			return nil
		}
		self.init(indexSet: decoded)
	}
}
