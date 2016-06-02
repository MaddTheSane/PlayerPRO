//
//  PPMusicListObject.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation
#if os(OSX)
	import Cocoa
#endif

private let kMusicListURLKey		= "URLKey"
private let kMusicListDateAddedKey	= "DateAdded"

private func URLsPointingToTheSameFile(_ urlA: NSURL, _ urlB: NSURL) -> Bool {
	var dat1: AnyObject? = nil
	var dat2: AnyObject? = nil
	var bothAreValid = true
	var theSame = false
	do {
		try urlA.getResourceValue(&dat1, forKey: NSURLFileResourceIdentifierKey)
	} catch _ {
		bothAreValid = false;
	}
	do {
		try urlB.getResourceValue(&dat2, forKey: NSURLFileResourceIdentifierKey)
	} catch _ {
		bothAreValid = false;
	}
	if bothAreValid {
		theSame = (dat1 as! NSObject) == (dat2 as! NSObject)
	}
	return theSame
}

func ==(lhs: MusicListObject, rhs: MusicListObject) -> Bool {
	if lhs === rhs {
		return true
	}
	
	if !URLsPointingToTheSameFile(lhs.musicURL, rhs.musicURL) {
		return false
	}
	
	if lhs.addedDate != rhs.addedDate {
		return false
	}
	
	return true
}

@objc(PPMusicListObject) class MusicListObject: NSObject, NSCopying, NSSecureCoding {
	let musicURL: NSURL
	let addedDate: NSDate

	#if os(OSX)
	@objc private(set) lazy var fileIcon: NSImage = {
		let image = NSWorkspace.shared().icon(forFile: self.musicURL.path!)
		image.size = NSSize(width: 16, height: 16)
		return image
		}()
	#endif
	
	@objc private(set) lazy var fileName: String = {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		
		do {
			try self.musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey)
			let retStr = val as! String
			return retStr
		} catch {
			return self.musicURL.lastPathComponent!
		}
		}()
	
	@objc private(set) lazy var fileSize: UInt64 = {
		var val: AnyObject? = nil;
		do {
			try self.musicURL.getResourceValue(&val, forKey:NSURLTotalFileSizeKey)
			let retNum = val as! NSNumber
			return retNum.uint64Value
		} catch {
			let manager = NSFileManager.default();
			do {
				let theparam = try manager.attributesOfItem(atPath: self.musicURL.path!)
				if let tmpfilesize: AnyObject = theparam[NSFileSize] {
					let aFileSize = tmpfilesize as! NSNumber
					return aFileSize.uint64Value
				} else {
					return 0
				}
			} catch _ {
				return 0
			}
		}
		}()
	
	init(url URL: NSURL, date: NSDate = NSDate()) {
		if URL.isFileReferenceURL() {
			musicURL = URL;
		} else {
			let tmpURL = URL.fileReferenceURL()
			musicURL = tmpURL ?? URL
		}
		addedDate = date
		super.init()
	}
	
	convenience init?(bookmarkData: NSData, resolutionOptions: NSURLBookmarkResolutionOptions = [], relativeURL: NSURL? = nil, date: NSDate? = NSDate()) {
		do {
			let resolvedURL = try NSURL(resolvingBookmarkData: bookmarkData, options: resolutionOptions, relativeTo: relativeURL, bookmarkDataIsStale: nil)
			self.init(url: resolvedURL, date: date ?? NSDate())
		} catch _ {
			self.init(url: NSURL(fileURLWithPath: "/dev/null"))

			return nil
		}
	}
	
	func checkIsReachableAndReturnError(error: NSErrorPointer) -> Bool {
		return musicURL.checkResourceIsReachableAndReturnError(error)
	}
	
	override var hash: Int {
		return self.hashValue
	}
	
	override var hashValue: Int {
		return musicURL.absoluteString.hashValue ^ addedDate.hash
	}

	override var description: String {
		return "\(musicURL.path!): \(fileName)"
	}
	
	override var debugDescription: String {
		return "\(musicURL.description) \(musicURL.path!): \(fileName) size: \(fileSize), added: \(addedDate)"
	}

	override func isEqual(_ object: AnyObject?) -> Bool {
		if object == nil {
			return false;
		}
		
		if (self === object!) {
			return true
		}
		
		if let unwrapped = object as? MusicListObject {
			return self == unwrapped
		} else {
			return false
		}
	}

	@objc(pointsToFileAtURL:) func pointsToFile(url URL: NSURL?) -> Bool {
		if let unwrapped = URL {
			return URLsPointingToTheSameFile(musicURL, unwrapped)
		} else {
			return false
		}
	}
	
	// MARK: - Sorting helper functions
	
	///Compare this music list object to another by the file name via `localizedStandardCompare`.
	///
	///- parameter other: The other `MusicListObject` to compare to.
	///- returns: How this object compares to the other one.
	func localizedStandardCompareFileName(other: MusicListObject) -> NSComparisonResult {
		return fileName.localizedStandardCompare(other.fileName)
	}
	
	///Compare this music list object to another by the file size.
	///
	///- parameter other: The other `MusicListObject` to compare to.
	///- returns: How this object compares to the other one.
	func compareByFileSize(other: MusicListObject) -> NSComparisonResult {
		let otherSize = other.fileSize
		if fileSize > otherSize {
			return .orderedAscending
		} else if fileSize < otherSize {
			return .orderedDescending
		} else {
			// Should be equal
			return .orderedSame
		}
	}
	
	///Compare this music list object to another by the date added to the list.
	///
	///- parameter other: The other `MusicListObject` to compare to.
	///- returns: How this object compares to the other one.
	func compareByAddedDate(other: MusicListObject) -> NSComparisonResult {
		return addedDate.compare(other.addedDate)
	}
	
	// MARK: NSCopying protocol
	func copy(with: NSZone?) -> AnyObject {
		return MusicListObject(url: musicURL)
	}

	// MARK: NSSecureCoding protocol
	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func encode(with aCoder: NSCoder) {
		aCoder.encode(musicURL, forKey: kMusicListURLKey)
		aCoder.encode(addedDate, forKey: kMusicListDateAddedKey)
	}
	
	convenience required init?(coder aDecoder: NSCoder) {
		guard let aURL = aDecoder.decodeObject(forKey: kMusicListURLKey) as? NSURL,
			aaddedDate = aDecoder.decodeObject(forKey: kMusicListDateAddedKey) as? NSDate else {
				return nil
		}
		
		self.init(url: aURL, date: aaddedDate)
	}
}
