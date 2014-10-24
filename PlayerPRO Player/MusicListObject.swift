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

private let kMusicListURLKey = "URLKey";

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
		theSame = (dat1 as NSData) == (dat2 as NSData)
	}
	return theSame
}

func ==(lhs: MusicListObject, rhs: MusicListObject) -> Bool {
	if lhs === rhs {
		return true
	}
	return URLsPointingToTheSameFile(lhs.musicURL, rhs.musicURL)
}

func ==(lhs: MusicListObject, rhs: NSURL) -> Bool {
	return URLsPointingToTheSameFile(lhs.musicURL, rhs)
}

func ==(lhs: NSURL, rhs: MusicListObject) -> Bool {
	return rhs == lhs
}

@objc(PPMusicListObject) class MusicListObject: NSObject, NSCopying, NSSecureCoding, Hashable, DebugPrintable, Printable {
	let musicURL: NSURL

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
			return self.musicURL.lastPathComponent
		} else {
			let retStr = val as NSString as String
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
					let aFileSize = tmpfilesize as NSNumber
					return aFileSize.unsignedLongLongValue
				} else {
					return 0
				}
			} else {
				return 0
			}
		} else {
			let retNum = val as NSNumber
			return retNum.unsignedLongLongValue
		}
	}()
	
	init(URL: NSURL) {
		if (URL.isFileReferenceURL()) {
			musicURL = URL;
		} else {
			let tmpURL = URL.fileReferenceURL()
			musicURL = tmpURL ?? URL
		}
		super.init();
	}
	
	convenience init?(bookmarkData: NSData, resolutionOptions: NSURLBookmarkResolutionOptions = nil, relativeURL: NSURL? = nil) {
		if let resolvedURL = NSURL(byResolvingBookmarkData: bookmarkData, options: resolutionOptions, relativeToURL: relativeURL, bookmarkDataIsStale: nil, error: nil) {
			self.init(URL: resolvedURL)
		} else {
			self.init(URL: NSURL(fileURLWithPath: "/dev/null")!)

			return nil
		}
	}
	
	override var hash: Int {
		return self.hashValue
	}
	
	override var hashValue: Int {
		if let absURL = musicURL.absoluteString {
			return absURL.hash
		} else {
			return super.hash
		}
	}

	override var description: String {
		return "\(musicURL.path): \(self.fileName)"
	}
	
	override var debugDescription: String {
		return "\(musicURL.description) \(musicURL.path): \(self.fileName)"
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
		} else if let unwrapped = object as? NSURL {
			return self == unwrapped
		} else {
			return false
		}
	}

	func copyWithZone(zone: NSZone) -> AnyObject {
		//this class is immutable
		return self;
	}

	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func encodeWithCoder(aCoder: NSCoder) {
		aCoder.encodeObject(musicURL, forKey: kMusicListURLKey)
	}
	
	convenience required init(coder aDecoder: NSCoder) {
		self.init(URL:aDecoder.decodeObjectForKey(kMusicListURLKey) as NSURL);
	}
}
