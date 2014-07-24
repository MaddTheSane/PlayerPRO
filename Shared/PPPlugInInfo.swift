//
//  PPPlugInInfo.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

class PPPlugInInfo: NSObject {
	private(set) var plugName: String;
	private(set) var authorName: String;
	private(set) var plugType: String
	private(set) var plugCopyright: String
	private(set) var plugURL: NSURL
	
	
	init(plugName pn: String, author aut: String, plugType pt: String, plugURL pu: NSURL) {
		var tmpBundle = NSBundle(URL: pu).infoDictionary
		plugName = pn
		authorName = aut
		plugType = pt
		plugURL = pu
		var tmpCopy: AnyObject? = tmpBundle["NSHumanReadableCopyright"]
		if (tmpCopy) {
			plugCopyright = tmpCopy! as String
		} else {
			plugCopyright = "No Copyright Info available"
		}
		super.init()
	}
	
	override var hash: Int {
	get {
		return plugName.hash ^ plugType.hash ^ authorName.hash ^ plugCopyright.hash
	}
	}
	
	override var description: String {
	get {
		return "\(plugName): \(authorName) (\(plugCopyright)), \(plugType)"
	}
	}
	
	override func isEqual(object: AnyObject!) -> Bool {
		if (!object) {
			return false
		}
		if (object === self) {
			return true;
		}
		if (object.isKindOfClass(PPPlugInInfo)) {
			if (plugName != (object as PPPlugInInfo).plugName) {
				return false;
			} else if (authorName != (object as PPPlugInInfo).authorName) {
				return false;
			} else if (plugType != (object as PPPlugInInfo).plugType) {
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	}
}
