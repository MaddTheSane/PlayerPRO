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
		try urlA.getResourceValue(&dat1, forKey: URLResourceKey.fileResourceIdentifierKey)
	} catch _ {
		bothAreValid = false;
	}
	do {
		try urlB.getResourceValue(&dat2, forKey: URLResourceKey.fileResourceIdentifierKey)
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
	let musicURL: URL
	let addedDate: Date

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
			var values = try self.musicURL.resourceValues(forKeys: [URLResourceKey.localizedNameKey])
			let retStr = values.localizedName!
			return retStr
		} catch {
			return self.musicURL.lastPathComponent!
		}
		}()
	
	@objc private(set) lazy var fileSize: UInt64 = {
		var val: AnyObject? = nil;
		do {
			var values = try self.musicURL.resourceValues(forKeys: [URLResourceKey.totalFileSizeKey])
			return UInt64(values.totalFileSize!)
		} catch {
			let manager = FileManager.default();
			do {
				let theparam = try manager.attributesOfItem(atPath: self.musicURL.path!)
				if let tmpfilesize: AnyObject = theparam[FileAttributeKey.size.rawValue] {
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
	
	init(url URL: Foundation.URL, date: Date = Date()) {
		if URL.isFileReferenceURL {
			musicURL = URL;
		} else {
			let tmpURL = try? URL.fileReferenceURL()
			musicURL = tmpURL ?? URL
		}
		addedDate = date
		super.init()
	}
	
	convenience init?(bookmarkData: Data, resolutionOptions: NSURL.BookmarkResolutionOptions = [], relativeURL: URL? = nil, date: Date? = Date()) {
		do {
			var unusedStale = false
			let resolvedURL = try URL(resolvingBookmarkData: bookmarkData, options: resolutionOptions, relativeTo: relativeURL, bookmarkDataIsStale: &unusedStale)!
			self.init(url: resolvedURL, date: date ?? Date())
		} catch _ {
			self.init(url: URL(fileURLWithPath: "/dev/null"))

			return nil
		}
	}
	
	func checkIsReachableAndReturnError(error: NSErrorPointer) -> Bool {
		do {
			return try checkIsReachable()
		} catch let swiftError as NSError {
			if let error = error {
				error.pointee = swiftError
			}
			return false
		} catch {
			return false
		}
	}
	
	func checkIsReachable() throws -> Bool {
		return try musicURL.checkResourceIsReachable()
	}
	
	override var hash: Int {
		return self.hashValue
	}
	
	override var hashValue: Int {
		return musicURL.absoluteString!.hashValue ^ (addedDate as NSDate).hash
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
	func localizedStandardCompareFileName(other: MusicListObject) -> ComparisonResult {
		return fileName.localizedStandardCompare(other.fileName)
	}
	
	///Compare this music list object to another by the file size.
	///
	///- parameter other: The other `MusicListObject` to compare to.
	///- returns: How this object compares to the other one.
	func compareByFileSize(other: MusicListObject) -> ComparisonResult {
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
	func compareByAddedDate(other: MusicListObject) -> ComparisonResult {
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
		
		self.init(url: aURL as URL, date: aaddedDate as Date)
	}
}
