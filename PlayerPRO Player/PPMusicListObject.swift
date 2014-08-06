//
//  PPMusicListObject.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation

private let kMusicListURLKey = "URLKey";

class PPMusicListObject: NSObject, NSCopying, NSSecureCoding {
	private(set) var musicURL: NSURL! = nil;

	var fileName: String {get {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		
		var isValid = musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)
		
		if (!musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)) {
			NSLog("PPMusicListObject: Could not find out if extension is hidden in file \"%@\", error: %@", musicURL.path, err!.localizedDescription);
			return musicURL.lastPathComponent;
		} else {
			var retStr = val as String;
			return retStr;
		}
	}}
	
	var fileSize : UInt64 {get {
		var manager = NSFileManager.defaultManager();
		var theparam = manager.attributesOfItemAtPath(musicURL.path, error: nil)
		if (!theparam) {
			return 0;
		}
		return theparam.bridgeToObjectiveC().fileSize()
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
	
	override var hash: Int {
	get {
		return musicURL.filePathURL.path.hash
	}
	}
	
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
	
	convenience init(coder aDecoder: NSCoder!) {
		self.init(URL:aDecoder.decodeObjectForKey(kMusicListURLKey) as NSURL);
	}
}
