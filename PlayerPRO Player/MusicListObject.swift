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
private let kMusicListURLBookmark	= "Bookmark"
private let kMusicListDateAddedKey	= "DateAdded"
private let MusicListUUIDKey		= "UUID"

#if os(OSX)
internal var homeURL: URL {
	return URL(fileURLWithPath: NSHomeDirectory())
}
#else
	internal var homeURL: URL? {
		return nil
	}
#endif

internal func URLsPointingToTheSameFile(_ urlA: URL, _ urlB: URL) -> Bool {
	var dat1: (NSCopying & NSSecureCoding & NSObjectProtocol)? = nil
	var dat2: (NSCopying & NSSecureCoding & NSObjectProtocol)? = nil
	var bothAreValid = true
	var theSame = false
	do {
		let vals = try urlA.resourceValues(forKeys: [.fileResourceIdentifierKey])
		dat1 = vals.fileResourceIdentifier
		
		let vals2 = try urlB.resourceValues(forKeys: [.fileResourceIdentifierKey])
		dat2 = vals2.fileResourceIdentifier
	} catch _ {
		bothAreValid = false
	}
	if bothAreValid, let dat1 = dat1, let dat2 = dat2 {
		theSame = dat1.isEqual(dat2)
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

@objc(PPMusicListObject) final class MusicListObject: NSObject, NSCopying, NSSecureCoding {
	let musicURL: URL
	let addedDate: Date
	let uuid: UUID

	#if os(OSX)
	@objc private(set) lazy var fileIcon: NSImage = {
		let image = NSWorkspace.shared.icon(forFile: self.musicURL.path)
		image.size = NSSize(width: 16, height: 16)
		return image
	}()
	#endif
	
	@objc private(set) lazy var fileName: String = {
		do {
			var values = try self.musicURL.resourceValues(forKeys: [URLResourceKey.localizedNameKey])
			guard let retStr = values.localizedName else {
				// only used to assure we get to the catch statement: it doesn't mean anything
				throw NSError(domain: NSCocoaErrorDomain, code: -1, userInfo: nil)
			}
			return retStr
		} catch _ {
			return self.musicURL.lastPathComponent
		}
	}()
	
	@objc private(set) lazy var fileSize: UInt64 = {
		var val: AnyObject? = nil
		do {
			var values = try self.musicURL.resourceValues(forKeys: [URLResourceKey.totalFileSizeKey])
			guard let val1 = values.totalFileSize else {
				// only used to assure we get to the catch statement: it doesn't mean anything
				throw NSError(domain: NSCocoaErrorDomain, code: -1, userInfo: nil)
			}
			return UInt64(val1)
		} catch {
			let manager = FileManager.default
			do {
				let theparam = try manager.attributesOfItem(atPath: self.musicURL.path)
				guard let tmpfilesize: Any = theparam[FileAttributeKey.size],
					let aFileSize = tmpfilesize as? NSNumber else {
						// only used to assure we get to the catch statement: it doesn't mean anything
						throw NSError(domain: NSCocoaErrorDomain, code: -1, userInfo: nil)
				}
				return aFileSize.uint64Value
			} catch _ {
				return 0
			}
		}
	}()
	
	init(url: URL, date: Date = Date(), uuid aUUID: UUID = UUID()) {
		musicURL = url
		addedDate = date
		uuid = aUUID
		super.init()
	}
	
	convenience init(bookmarkData: Data, resolutionOptions: NSURL.BookmarkResolutionOptions = [], relativeURL: URL? = nil, date: Date? = Date()) throws {
		var unusedStale = false
		guard let resolvedURL = try URL(resolvingBookmarkData: bookmarkData, options: resolutionOptions, relativeTo: relativeURL, bookmarkDataIsStale: &unusedStale) else {
			throw NSError(domain: NSCocoaErrorDomain, code: NSFileNoSuchFileError)
		}
		self.init(url: resolvedURL, date: date ?? Date())
	}
	
	func checkIsReachableAndReturnError(error: NSErrorPointer) -> Bool {
		do {
			return try checkIsReachable()
		} catch let swiftError as NSError {
			error?.pointee = swiftError
			return false
		} catch {
			return false
		}
	}
	
	@nonobjc func checkIsReachable() throws -> Bool {
		return try musicURL.checkResourceIsReachable()
	}
	
	override var hash: Int {
		return self.hashValue
	}
	
	override var hashValue: Int {
		return musicURL.absoluteString.hashValue ^ addedDate.hashValue
	}

	override var description: String {
		return "\(musicURL.path): '\(fileName)'"
	}
	
	override var debugDescription: String {
		return "\(musicURL.description) \(musicURL.path): '\(fileName)' size: \(fileSize), added: \(addedDate)"
	}

	override func isEqual(_ object: Any?) -> Bool {
		if object == nil {
			return false
		}
		
		if let unwrapped = object as? MusicListObject {
			if self === unwrapped {
				return true
			}
			return self == unwrapped
		} else {
			return false
		}
	}

	@objc(pointsToFileAtURL:)
	func pointsToFile(at url: URL?) -> Bool {
		if let unwrapped = url {
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
	func localizedStandardCompareFileName(_ other: MusicListObject) -> ComparisonResult {
		return fileName.localizedStandardCompare(other.fileName)
	}
	
	///Compare this music list object to another by the file size.
	///
	///- parameter other: The other `MusicListObject` to compare to.
	///- returns: How this object compares to the other one.
	func compareByFileSize(_ other: MusicListObject) -> ComparisonResult {
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
	func compareByAddedDate(_ other: MusicListObject) -> ComparisonResult {
		return addedDate.compare(other.addedDate)
	}
	
	// MARK: NSCopying protocol
	func copy(with: NSZone?) -> Any {
		return MusicListObject(url: musicURL)
	}

	// MARK: NSSecureCoding protocol
	static var supportsSecureCoding: Bool {
		return true
	}
	
	func encode(with aCoder: NSCoder) {
		#if os(OSX)
		if let bookmark = try? musicURL.bookmarkData(options: [], includingResourceValuesForKeys: [.volumeURLKey, .volumeUUIDStringKey, .volumeURLForRemountingKey], relativeTo: homeURL) {
			aCoder.encode(bookmark, forKey: kMusicListURLBookmark)
		}
		#endif
		aCoder.encode(musicURL, forKey: kMusicListURLKey)
		aCoder.encode(addedDate, forKey: kMusicListDateAddedKey)
		aCoder.encode(uuid, forKey: MusicListUUIDKey)
	}
	
	convenience required init?(coder aDecoder: NSCoder) {
		var fileURL: URL?
		
		#if os(OSX)
		if let bookmarkData = aDecoder.decodeObject(forKey: kMusicListURLBookmark) as? Data {
			var unusedStale: Bool = false
			if let hi = try? URL(resolvingBookmarkData: bookmarkData, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &unusedStale) {
				fileURL = hi
			}
		}
		#endif
		
		if fileURL == nil {
			fileURL = aDecoder.decodeObject(forKey: kMusicListURLKey) as? URL
		}
		
		guard let aURL = fileURL,
			let aaddedDate = aDecoder.decodeObject(forKey: kMusicListDateAddedKey) as? Date else {
				return nil
		}
		let aUUID = (aDecoder.decodeObject(forKey: MusicListUUIDKey) as? UUID) ?? UUID()
		
		self.init(url: aURL, date: aaddedDate, uuid: aUUID)
	}
}

extension MusicListObject: Codable {
	enum CodingKeys: String, CodingKey {
		case musicURL = "url"
		case dateAdded = "date_added"
		case uuid = "uuid"
		
		case bookmarkData = "bookmark_data"
	}

	func encode(to encoder: Encoder) throws {
		var container = encoder.container(keyedBy: CodingKeys.self)
		#if os(OSX)
			if let bookmark = try? musicURL.bookmarkData(includingResourceValuesForKeys: [.volumeURLKey, .volumeUUIDStringKey, .volumeURLForRemountingKey], relativeTo: homeURL) {
				try container.encode(bookmark, forKey: .bookmarkData)
			}
		#endif
		try container.encode(musicURL, forKey: .musicURL)
		try container.encode(addedDate, forKey: .dateAdded)
		try container.encode(uuid, forKey: .uuid)
	}
	
	convenience init(from decoder: Decoder) throws {
		let values = try decoder.container(keyedBy: CodingKeys.self)
		let dateAdded = try values.decode(Date.self, forKey: .dateAdded)
		var url: URL?
		
		#if os(OSX)
		if values.contains(.bookmarkData) {
			var unusedStale = false
			if let bookDat = try values.decodeIfPresent(Data.self, forKey: .bookmarkData),
				let url2 = try? URL(resolvingBookmarkData: bookDat, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &unusedStale) {
				url = url2
			}
		}
		#endif
		
		if url == nil {
			url = try values.decode(URL.self, forKey: .musicURL)
		}
		if let aUUID = try values.decodeIfPresent(UUID.self, forKey: .uuid) {
			self.init(url: url!, date: dateAdded, uuid: aUUID)
		} else {
			self.init(url: url!, date: dateAdded)
		}
	}
}
