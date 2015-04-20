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

private func URLsPointingToTheSameFile(urlA: NSURL, urlB: NSURL) -> Bool {
	var dat1: AnyObject? = nil
	var dat2: AnyObject? = nil
	var bothAreValid = true
	var theSame = false
	if !urlA.getResourceValue(&dat1, forKey: NSURLFileResourceIdentifierKey, error:nil) {
		bothAreValid = false;
	}
	if !urlB.getResourceValue(&dat2, forKey: NSURLFileResourceIdentifierKey, error:nil) {
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

@objc(PPMusicListObject) class MusicListObject: NSObject, NSCopying, NSSecureCoding, Hashable, DebugPrintable, Printable {
	let musicURL: NSURL
	let addedDate: NSDate

	#if os(OSX)
	@objc private(set) lazy var fileIcon: NSImage = {
		let image = NSWorkspace.sharedWorkspace().iconForFile(self.musicURL.path!)
		image.size = NSSize(width: 16, height: 16)
		return image
		}()
	#endif
	
	@objc private(set) lazy var fileName: String = {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		
		if (!self.musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)) {
			println("PPMusicListObject: Could not find out if extension is hidden in file \(self.musicURL.path), error: \(err!.localizedDescription)")
			return self.musicURL.lastPathComponent!
		} else {
			let retStr = val as! String
			return retStr
		}
		}()
	
	@objc private(set) lazy var fileSize: UInt64 = {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		if (!self.musicURL.getResourceValue(&val, forKey:NSURLTotalFileSizeKey, error: &err)) {
			let manager = NSFileManager.defaultManager();
			if let theparam = manager.attributesOfItemAtPath(self.musicURL.path!, error: nil) {
				if let tmpfilesize: AnyObject = theparam[NSFileSize] {
					let aFileSize = tmpfilesize as! NSNumber
					return aFileSize.unsignedLongLongValue
				} else {
					return 0
				}
			} else {
				return 0
			}
		} else {
			let retNum = val as! NSNumber
			return retNum.unsignedLongLongValue
		}
		}()
	
	init(URL: NSURL, date: NSDate = NSDate()) {
		if (URL.isFileReferenceURL()) {
			musicURL = URL;
		} else {
			let tmpURL = URL.fileReferenceURL()
			musicURL = tmpURL ?? URL
		}
		addedDate = date
		super.init();
	}
	
	convenience init?(bookmarkData: NSData, resolutionOptions: NSURLBookmarkResolutionOptions = nil, relativeURL: NSURL? = nil, date: NSDate? = NSDate()) {
		if let resolvedURL = NSURL(byResolvingBookmarkData: bookmarkData, options: resolutionOptions, relativeToURL: relativeURL, bookmarkDataIsStale: nil, error: nil) {
			self.init(URL: resolvedURL, date: date ?? NSDate())
		} else {
			self.init(URL: NSURL(fileURLWithPath: "/dev/null")!)

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
		if let absURL = musicURL.absoluteString {
			return absURL.hashValue ^ addedDate.hash
		} else {
			return super.hash ^ addedDate.hash
		}
	}

	override var description: String {
		return "\(musicURL.path!): \(fileName)"
	}
	
	override var debugDescription: String {
		return "\(musicURL.description) \(musicURL.path!): \(fileName) size: \(fileSize), added: \(addedDate)"
	}

	override func isEqual(object: AnyObject?) -> Bool {
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

	@objc(pointsToFileAtURL:) func pointsToFile(#URL: NSURL?) -> Bool {
		if let unwrapped = URL {
			return URLsPointingToTheSameFile(musicURL, unwrapped)
		} else {
			return false
		}
	}
	
	// MARK: - Sorting helper functions
	
	///Compare this music list object to another by the file name via localizedStandardCompare
	///
	///:param: other The other `MusicListObject` to compare to.
	///:returns: How this object compares to the other one.
	func localizedStandardCompareFileName(other: MusicListObject) -> NSComparisonResult {
		return fileName.localizedStandardCompare(other.fileName)
	}
	
	///Compare this music list object to another by the file size
	///
	///:param: other The other `MusicListObject` to compare to.
	///:returns: How this object compares to the other one.
	func compareByFileSize(other: MusicListObject) -> NSComparisonResult {
		let otherSize = other.fileSize
		if fileSize > otherSize {
			return .OrderedAscending
		} else if fileSize < otherSize {
			return .OrderedDescending
		} else {
			// Should be equal
			return .OrderedSame
		}
	}
	
	///Compare this music list object to another by the date added to the list
	///
	///:param: other The other `MusicListObject` to compare to.
	///:returns: How this object compares to the other one.
	func compareByAddedDate(other: MusicListObject) -> NSComparisonResult {
		return addedDate.compare(other.addedDate)
	}
	
	// MARK: NSCopying protocol
	func copyWithZone(zone: NSZone) -> AnyObject {
		return MusicListObject(URL: musicURL)
	}

	// MARK: NSSecureCoding protocol
	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func encodeWithCoder(aCoder: NSCoder) {
		aCoder.encodeObject(musicURL, forKey: kMusicListURLKey)
		aCoder.encodeObject(addedDate, forKey: kMusicListDateAddedKey)
	}
	
	convenience required init(coder aDecoder: NSCoder) {
		let aURL = aDecoder.decodeObjectForKey(kMusicListURLKey) as! NSURL
		let aaddedDate = aDecoder.decodeObjectForKey(kMusicListDateAddedKey) as! NSDate
		
		self.init(URL: aURL, date: aaddedDate)
	}
}
