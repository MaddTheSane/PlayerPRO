//
//  PPPlugInInfo.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

func ==(lhs: PlugInInfo, rhs: PlugInInfo) -> Bool {
	if (lhs === rhs) {
		return true;
	}
	
	if (lhs.plugName != rhs.plugName) {
		return false;
	} else if (lhs.authorName != rhs.authorName) {
		return false;
	} else if (lhs.plugType != rhs.plugType) {
		return false;
	} else {
		return true;
	}
}

final class PlugInInfo: Hashable, DebugPrintable, Printable {
	let plugName: String
	let authorName: String
	let plugType: String
	let plugCopyright: String
	let plugURL: NSURL
	
	init(plugName pn: String, author aut: String? = nil, plugType pt: String = "unknown", plugURL pu: NSURL = NSBundle.mainBundle().bundleURL) {
		plugName = pn
		authorName = aut ?? "Unknown Author"
		plugType = pt
		plugURL = pu
		if let tmpBundle = NSBundle(URL: pu) {
			if let acopy = tmpBundle.infoDictionary!["NSHumanReadableCopyright"] as? String {
				plugCopyright = acopy
			} else {
				plugCopyright = "No copyright info available"
			}
		} else {
			plugCopyright = "No copyright info available"
		}
	}
	
	var hash: Int {
		return self.hashValue
	}
	
	var hashValue: Int {
		return plugName.hashValue ^ plugType.hashValue ^ authorName.hashValue ^ plugCopyright.hashValue
	}
	
	var debugDescription: String {
		return "\(plugName): \(authorName) (\(plugCopyright)), \(plugType)"
	}
	
	var description: String {
		return "\(plugName): \(plugType)"
	}
	
	func isEqual(object: AnyObject?) -> Bool {
		if (object == nil) {
			return false
		}
		
		if (object === self) {
			return true;
		}
		
		if let unwrapped = object as? PlugInInfo {
			return self == unwrapped
		} else {
			return false;
		}
	}
}
