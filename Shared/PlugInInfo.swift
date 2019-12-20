//
//  PPPlugInInfo.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/24/14.
//
//

import Cocoa

final class PlugInInfo: Hashable, CustomDebugStringConvertible, CustomStringConvertible {
	let plugName: String
	let authorName: String
	let plugType: String
	let plugCopyright: String
	let plugURL: URL
	
	init(plugName pn: String, author aut: String? = nil, plugType pt: String = "unknown", plugURL pu: URL = Bundle.main.bundleURL) {
		plugName = pn
		authorName = aut ?? "Unknown Author"
		plugType = pt
		plugURL = pu
		if let tmpBundle = Bundle(url: pu), let acopy = tmpBundle.infoDictionary?["NSHumanReadableCopyright"] as? String {
			plugCopyright = acopy
		} else {
			plugCopyright = "No copyright info available"
		}
	}
	
	func hash(into hasher: inout Hasher) {
		plugName.hash(into: &hasher)
		plugType.hash(into: &hasher)
		authorName.hash(into: &hasher)
		plugCopyright.hash(into: &hasher)
	}
	
	var debugDescription: String {
		return "\(plugName): \(authorName) (\(plugCopyright)), \(plugType)"
	}
	
	var description: String {
		return "\(plugName): \(plugType)"
	}
	
	func isEqual(_ object: AnyObject?) -> Bool {
		if object == nil {
			return false
		}
		
		if object === self {
			return true
		}
		
		if let unwrapped = object as? PlugInInfo {
			return self == unwrapped
		} else {
			return false
		}
	}
	
	static func ==(lhs: PlugInInfo, rhs: PlugInInfo) -> Bool {
		if lhs === rhs {
			return true
		}
		
		if lhs.plugName != rhs.plugName {
			return false
		} else if lhs.authorName != rhs.authorName {
			return false
		} else if lhs.plugType != rhs.plugType {
			return false
		} else {
			return true
		}
	}
}
