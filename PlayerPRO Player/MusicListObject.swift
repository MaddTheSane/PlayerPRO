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
	if !urlA.getResourceValue(&dat1, forKey:NSURLFileResourceIdentifierKey, error:nil) {
		bothAreValid = false;
	}
	if !urlB.getResourceValue(&dat2, forKey:NSURLFileResourceIdentifierKey, error:nil) {
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
	return URLsPointingToTheSameFile(lhs, rhs.musicURL)
}

@objc(PPMusicListObject) class MusicListObject: NSObject, NSCopying, NSSecureCoding, Hashable, DebugPrintable, Printable {
	let musicURL: NSURL

	#if os(OSX)
	var fileIcon: NSImage { get {
		let image = NSWorkspace.sharedWorkspace().iconForFile(musicURL.path);
		image.size = NSSize(width: 16, height: 16)
		return image
	}}
	#endif
	
	var fileName: String { get {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		
		if (!musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)) {
			println("PPMusicListObject: Could not find out if extension is hidden in file \(musicURL.path), error: \(err!.localizedDescription)");
			return musicURL.lastPathComponent;
		} else {
			let retStr = val! as String;
			return retStr;
		}
	}}
	
	internal var stashedFileSize: UInt64 = 0
	var fileSize : UInt64 { get {
		if stashedFileSize == 0 {
			var val: AnyObject? = nil;
			var err: NSError? = nil;
			if (!musicURL.getResourceValue(&val, forKey:NSURLTotalFileSizeKey, error: &err)) {
				let manager = NSFileManager.defaultManager();
				let theparam = manager.attributesOfItemAtPath(musicURL.path, error: nil)
				if (theparam == nil) {
					return 0;
				}
				stashedFileSize = (theparam! as NSDictionary).fileSize()
			} else {
				let retNum = val as NSNumber
				stashedFileSize = val!.unsignedLongLongValue
			}
		}
		return stashedFileSize
		}}
	
	init(URL: NSURL!) {
		if (URL != nil) {
			// How to fail?
			//return nil;
		}
		if (URL.isFileReferenceURL()) {
			self.musicURL = URL;
		} else {
			let tmpURL = URL.fileReferenceURL();
			self.musicURL = tmpURL == nil ? tmpURL : URL;

		}
		super.init();
	}
	
	override var hash: Int { get {
		return self.hashValue
	}}
	
	override var hashValue: Int { get {
		return musicURL.filePathURL!.path!.hashValue
	}}

	override var description: String { get {
		return "\(musicURL.path): \(self.fileName)"
	}}
	
	override var debugDescription: String { get {
		return "\(musicURL.description) \(musicURL.path): \(self.fileName)"
	}}

	override func isEqual(object: AnyObject!) -> Bool {
		var dat1: AnyObject? = nil
		var dat2: AnyObject? = nil
		var bothAreValid = true
		var theSame = false
		if (object == nil) {
			return false;
		}
		
		if (self === object) {
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

	func copyWithZone(zone: NSZone) -> AnyObject! {
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
