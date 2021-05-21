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

private var bookGenOpts: Set<URLResourceKey> {
	return [.volumeURLKey, .volumeUUIDStringKey, .volumeURLForRemountingKey]
}
	
internal let homeURL: URL = {
	return FileManager.default.homeDirectoryForCurrentUser
}()
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

@objc(PPMusicListObject) final class MusicListObject: NSObject, NSCopying, NSSecureCoding {
	@objc private(set) var musicURL: URL
	@objc let addedDate: Date
	@objc let uuid: UUID
	#if os(OSX)
	private var bookData: Data?
	#endif

	static func ==(lhs: MusicListObject, rhs: MusicListObject) -> Bool {
		if lhs === rhs {
			return true
		}
		
		if !URLsPointingToTheSameFile(lhs.musicURL, rhs.musicURL) {
			return false
		}
		
		return true
	}
	
	#if os(OSX)
	@objc private(set) lazy var fileIcon: NSImage = {
		let image = NSWorkspace.shared.icon(forFile: self.musicURL.path).copy() as! NSImage
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
	
	convenience init(bookmarkData: Data, resolutionOptions: NSURL.BookmarkResolutionOptions = [], relativeURL: URL? = nil, date: Date? = Date(), uuid aUUID: UUID = UUID()) throws {
		var unusedStale = false
		let resolvedURL = try URL(resolvingBookmarkData: bookmarkData, options: resolutionOptions, relativeTo: relativeURL, bookmarkDataIsStale: &unusedStale)
		self.init(url: resolvedURL, date: date ?? Date(), uuid: aUUID)
		#if os(OSX)
			bookData = Data(bookmarkData)
		#endif
	}
	
	@objc(checkIsReachableAndReturnError:)
	func checkIsReachableAndReturnError(_ error: NSErrorPointer) -> Bool {
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
		var hasher = Hasher()
		musicURL.absoluteString.hash(into: &hasher)
		addedDate.hash(into: &hasher)
		return hasher.finalize()
	}
	
	override var description: String {
		return "\(musicURL.path): '\(fileName)'"
	}
	
	override var debugDescription: String {
		return "\(musicURL.description) \(musicURL.path): '\(fileName)' size: \(fileSize), added: \(addedDate), UUID: \(uuid)"
	}

	override func isEqual(_ object: Any?) -> Bool {
		if object == nil {
			return false
		}
		
		if let unwrapped = object as? MusicListObject {
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
	@objc func localizedStandardCompareFileName(_ other: MusicListObject) -> ComparisonResult {
		return fileName.localizedStandardCompare(other.fileName)
	}
	
	///Compare this music list object to another by the file size.
	///
	///- parameter other: The other `MusicListObject` to compare to.
	///- returns: How this object compares to the other one.
	@objc func compareByFileSize(_ other: MusicListObject) -> ComparisonResult {
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
	@objc func compareByAddedDate(_ other: MusicListObject) -> ComparisonResult {
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
			var derefURL = musicURL
			// Verify the location of the URL hasn't changed
			// ...but only if we can track it.
			// If some part of the process fails, just use the (possibly bad) URL.
			// If no bookmark data, just generate the data as usual.
			if let data = bookData {
				var stale = false
				if let url1 = try? URL(resolvingBookmarkData: data, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &stale), stale {
					derefURL = url1
				}
			}
			
			aCoder.encode(derefURL, forKey: kMusicListURLKey)
			if let bookmark = try? derefURL.bookmarkData(includingResourceValuesForKeys: bookGenOpts, relativeTo: homeURL) {
				aCoder.encode(bookmark, forKey: kMusicListURLBookmark)
			}
		#else
			aCoder.encode(musicURL, forKey: kMusicListURLKey)
		#endif

		aCoder.encode(addedDate, forKey: kMusicListDateAddedKey)
		aCoder.encode(uuid, forKey: MusicListUUIDKey)
	}
	
	convenience required init?(coder aDecoder: NSCoder) {
		var fileURL: URL?
		
		#if os(OSX)
			var data: Data? = nil
		if let bookmarkData = aDecoder.decodeObject(of: NSData.self, forKey: kMusicListURLBookmark) as Data? {
			var stale: Bool = false
			if let hi = try? URL(resolvingBookmarkData: bookmarkData, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &stale) {
				fileURL = hi
				if stale {
					// Regenerate bookmark data
					if let dat2 = try? hi.bookmarkData(includingResourceValuesForKeys: bookGenOpts, relativeTo: homeURL) {
						data = dat2
					}
				} else {
					data = bookmarkData
				}
			}
		}
		#endif
		
		if fileURL == nil {
			fileURL = aDecoder.decodeObject(of: NSURL.self, forKey: kMusicListURLKey) as URL?
		}
		
		guard let aURL = fileURL else {
			aDecoder.failWithError(NSError(domain: NSOSStatusErrorDomain, code: paramErr))
			return nil
		}
		guard let aaddedDate = aDecoder.decodeObject(of: NSDate.self, forKey: kMusicListDateAddedKey) as Date? else {
				aDecoder.failWithError(NSError(domain: NSOSStatusErrorDomain, code: paramErr))
				return nil
		}
		let aUUID = (aDecoder.decodeObject(of: NSUUID.self, forKey: MusicListUUIDKey) as UUID?) ?? UUID()
		
		self.init(url: aURL, date: aaddedDate, uuid: aUUID)
		#if os(OSX)
			if let data = data {
				bookData = data
			}
		#endif
	}
	
	#if os(OSX)
	/// - returns: `true` if the URL needed updating, `false` otherwise, throws if it can't be validated.
	func validateURL() throws -> Bool {
		guard let bookData = bookData else {
			throw NSError(domain: NSOSStatusErrorDomain, code: paramErr, userInfo: nil)
		}
		var stale = false
		let url2 = try URL(resolvingBookmarkData: bookData, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &stale)
		if stale {
			musicURL = url2
			
			// Regenerate bookmark data
			let dat2 = try url2.bookmarkData(includingResourceValuesForKeys: bookGenOpts, relativeTo: homeURL)
			self.bookData = dat2
			
			return true
		}

		return false
	}
	#endif
}

//MARK: - Codable

extension MusicListObject: Codable {
	enum CodingKeys: String, CodingKey {
		case musicURL = "url"
		case dateAdded = "date_added"
		case uuid
		
		case bookmarkData = "bookmark_data"
	}

	func encode(to encoder: Encoder) throws {
		var container = encoder.container(keyedBy: CodingKeys.self)
		try container.encode(uuid, forKey: .uuid)
		try container.encode(addedDate, forKey: .dateAdded)
		#if os(OSX)
			var derefURL = musicURL
			// Verify the location of the URL hasn't changed
			// ...but only if we can track it.
			// If some part of the process fails, just use the (possibly bad) URL.
			// If no bookmark data, just generate the data as usual.
			if let data = bookData {
				var stale = false
				if let url1 = try? URL(resolvingBookmarkData: data, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &stale), stale {
					derefURL = url1
				}
			}
			
			try container.encode(derefURL, forKey: .musicURL)
			if let bookmark = try? derefURL.bookmarkData(includingResourceValuesForKeys: bookGenOpts, relativeTo: homeURL) {
				try container.encode(bookmark, forKey: .bookmarkData)
			}
		#else
			try container.encode(musicURL, forKey: .musicURL)
		#endif
	}
	
	convenience init(from decoder: Decoder) throws {
		let values = try decoder.container(keyedBy: CodingKeys.self)
		let dateAdded = try values.decode(Date.self, forKey: .dateAdded)
		var url: URL?
		
		#if os(OSX)
			var data: Data? = nil
			do {
				var stale = false
				if let bookDat = try values.decodeIfPresent(Data.self, forKey: .bookmarkData),
					let url2 = try? URL(resolvingBookmarkData: bookDat, options: [.withoutUI], relativeTo: homeURL, bookmarkDataIsStale: &stale) {
					url = url2
					if stale {
						// Regenerate bookmark data
						if let dat2 = try? url?.bookmarkData(includingResourceValuesForKeys: bookGenOpts, relativeTo: homeURL) {
							data = dat2
						}
					} else {
						data = bookDat
					}
				}
			}
		#endif
		
		if url == nil {
			url = try values.decode(URL.self, forKey: .musicURL)
		}
		let aUUID = try values.decode(UUID.self, forKey: .uuid)
		self.init(url: url!, date: dateAdded, uuid: aUUID)
		#if os(OSX)
			if let data = data {
				bookData = data
			}
		#endif
	}
}
