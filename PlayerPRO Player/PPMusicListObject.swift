//
//  PPMusicListObject.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Cocoa

let kURLKey = "URLKey";

class PPMusicListObject: NSObject, NSCopying, NSSecureCoding {
	var musicURL: NSURL! = nil;
	init(URL: NSURL!)
	{
		if (URL?) {
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
	
	var fileIcon: NSImage {get {
		var image = NSWorkspace.sharedWorkspace().iconForFile(musicURL.path);
		image.size = NSMakeSize(16, 16);
		return image;
	}}
	
	var fileName: String {get {
		var val: AnyObject? = nil;
		var err: NSError? = nil;
		
		var isValid = musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)
		
		if (!musicURL.getResourceValue(&val, forKey:NSURLLocalizedNameKey, error: &err)) {
			//NSLog("PPMusicListObject: Could not find out if extension is hidden in file \"%@\", error: %@", musicUrl.path, err.localizedDescription);
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

	func copyWithZone(zone: NSZone) -> AnyObject! {
		return self;
	}
	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func encodeWithCoder(aCoder: NSCoder!) {
		aCoder.encodeObject(musicURL, forKey: kURLKey)
	}
	
	convenience init(coder aDecoder: NSCoder!) {
		self.init(URL:aDecoder.decodeObjectForKey(kURLKey) as NSURL);
	}

	
}
