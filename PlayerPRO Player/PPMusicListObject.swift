//
//  PPMusicListObject.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation

private let kMusicListURLKey = "URLKey";

func ==(lhs: PPMusicListObject, rhs: PPMusicListObject) -> Bool {
	var dat1: AnyObject? = nil
	var dat2: AnyObject? = nil
	var bothAreValid = true
	var theSame = false
	if (!lhs.musicURL.getResourceValue(&dat1, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
		bothAreValid = false;
	}
	if (!rhs.musicURL.getResourceValue(&dat2, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
		bothAreValid = false;
	}
	if (bothAreValid) {
		theSame = (dat1 as NSData) == (dat2 as NSData)
	}
	return theSame
}

func ==(lhs: PPMusicListObject, rhs: NSURL) -> Bool {
	var dat1: AnyObject? = nil
	var dat2: AnyObject? = nil
	var bothAreValid = true
	var theSame = false
	if (!lhs.musicURL.getResourceValue(&dat1, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
		bothAreValid = false;
	}
	if (!rhs.getResourceValue(&dat2, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
		bothAreValid = false;
	}
	if (bothAreValid) {
		theSame = (dat1 as NSData) == (dat2 as NSData)
	}
	return theSame
}

func ==(lhs: NSURL, rhs: PPMusicListObject) -> Bool {
	var dat1: AnyObject? = nil
	var dat2: AnyObject? = nil
	var bothAreValid = true
	var theSame = false
	if (!lhs.getResourceValue(&dat1, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
		bothAreValid = false;
	}
	if (!rhs.musicURL.getResourceValue(&dat2, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
		bothAreValid = false;
	}
	if (bothAreValid) {
		theSame = (dat1 as NSData) == (dat2 as NSData)
	}
	return theSame
}

class PPMusicListObject: NSObject, NSCopying, NSSecureCoding, Hashable, DebugPrintable, Printable {
	private(set) var musicURL: NSURL! = nil;

	var fileName: String {get {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		
		var isValid = musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)
		
		if (!musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)) {
			println("PPMusicListObject: Could not find out if extension is hidden in file \(musicURL.path), error: \(err!.localizedDescription)");
			return musicURL.lastPathComponent;
		} else {
			var retStr = val as String;
			return retStr;
		}
	}}
	
	internal var stashedFileSize: UInt64 = 0
	var fileSize : UInt64 {get {
		if stashedFileSize == 0 {
			var val: AnyObject? = nil;
			var err: NSError? = nil;
			if (!musicURL.getResourceValue(&val, forKey:NSURLTotalFileSizeKey, error: &err)) {
				var manager = NSFileManager.defaultManager();
				var theparam = manager.attributesOfItemAtPath(musicURL.path, error: nil)
				if (theparam == nil) {
					return 0;
				}
				stashedFileSize = (theparam as NSDictionary).fileSize()
			} else {
				var retNum = val as NSNumber
				stashedFileSize = val!.unsignedLongLongValue
			}
		}
		return stashedFileSize
		}}

	override class func initialize() {
		NSKeyedUnarchiver.setClass(self, forClassName: "PPMusicListObject")
		NSKeyedArchiver.setClassName("PPMusicListObject", forClass: self)
	}
	
	init(URL: NSURL!) {
		if (!URL) {
			// How to fail?
			//return nil;
		}
		if (URL.isFileReferenceURL()) {
			self.musicURL = URL;
		} else {
			var tmpURL = URL.fileReferenceURL();
			self.musicURL = tmpURL ? tmpURL : URL;

		}
		super.init();
	}
	
	override var hash: Int { get {
		return self.hashValue
	}}
	
	override var hashValue: Int { get {
		return musicURL.filePathURL.path.hashValue
	}}

	override var description: String { get {
		return "\(musicURL.description) \(musicURL.path)"
	}}
	
	override var debugDescription: String { get {
		return "\(musicURL.description) \(musicURL.path): \(self.fileName)"
	}}

	override func isEqual(object: AnyObject!) -> Bool {
		var dat1: AnyObject? = nil
		var dat2: AnyObject? = nil
		var bothAreValid = true
		var theSame = false
		if (!object) {
			return false;
		}
		
		if (self === object) {
			return true
		}
		
		if (object.isKindOfClass(PPMusicListObject)) {
			if (!musicURL.getResourceValue(&dat1, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
				bothAreValid = false;
			}
			if (!(object as PPMusicListObject).musicURL.getResourceValue(&dat2, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
				bothAreValid = false;
			}
			if (bothAreValid) {
				theSame = dat1 as NSData == dat2 as NSData
			}
			return theSame
		} else if (object.isKindOfClass(NSURL)) {
			if (!musicURL.getResourceValue(&dat1, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
				bothAreValid = false;
			}
			if (!(object as NSURL).getResourceValue(&dat2, forKey:NSURLFileResourceIdentifierKey, error:nil)) {
				bothAreValid = false;
			}
			if (bothAreValid) {
				theSame = (dat1 as NSData) == (dat2 as NSData)
			}
			return theSame
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
	
	func encodeWithCoder(aCoder: NSCoder!) {
		aCoder.encodeObject(musicURL, forKey: kMusicListURLKey)
	}
	
	convenience required init(coder aDecoder: NSCoder!) {
		self.init(URL:aDecoder.decodeObjectForKey(kMusicListURLKey) as NSURL);
	}
}
