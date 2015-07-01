//
//  StcfImporter.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/1/15.
//
//

import Foundation
import Carbon
import SwiftAdditions

private func pascalStringFromData(aResource: NSData, index indexID: Int) -> [UInt8]? {
	let handSize = aResource.length
	var curSize = 2
	var aId = indexID
	
	var data = UnsafePointer<UInt8>(aResource.bytes)
	let count = UnsafePointer<Int16>(aResource.bytes).memory.bigEndian
	
	// First 2 bytes are the count of strings that this resource has.
	if Int(count) < aId {
		return nil
	}
	
	// skip count
	data += 2
	
	// looking for data.  data is in order
	while (--aId >= 0) {
		let toAdd = Int(data.memory) + 1;
		curSize += toAdd
		if (curSize >= handSize) {
			return nil;
		}
		data += toAdd
	}
	
	return {
		var aRet = [UInt8]()
		for i in 0...Int(data.memory) {
			aRet.append(data[i])
		}
		
		return aRet
		}()
}

private func pascalStringToString(aStr: UnsafePointer<UInt8>) -> String? {
	if let CFaStr = (CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, CFStringBuiltInEncodings.MacRoman.rawValue) as CFString?) as? String {
		return CFaStr
		// Perhaps the string is in another encoding. Try using the system's encoding to test this theory.
	} else if let CFaStr = (CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, CFStringGetMostCompatibleMacStringEncoding(CFStringGetSystemEncoding())) as CFString?) as? String {
		return CFaStr
		// Maybe GetApplicationTextEncoding can get the right format?
	} else if let CFaStr = (CFStringCreateWithPascalString(kCFAllocatorDefault, aStr, GetApplicationTextEncoding()) as CFString?) as? String {
		return CFaStr
	}
	
	return nil
}


public class StcfImporter: NSObject, NSXPCListenerDelegate, PPSTImporterHelper {
	
	static var sharedImporter: StcfImporter = {
		return StcfImporter()
	}()
	
	private func loadOldMusicListAtURL(toOpen: NSURL, inout toDictionary outDict: [NSObject : AnyObject]) -> NSError? {
		var iErr: NSError? = nil
		if let resource = FVResourceFile.resourceFileWithContentsOfURL(toOpen, error: &iErr) {
			if let playListRes = resource.resource(type: "STR#", index: 128), posRes = resource.resource(type: "selc", index: 128), playListDat = playListRes.data, posDat = posRes.data {
				var location = UnsafePointer<Int16>(posDat.bytes).memory.bigEndian
				var theNo = UnsafePointer<UInt16>(playListDat.bytes).memory.bigEndian / 2
				var strIdx = 0
				var newArray = [String]()
				var lostMusicCount = 0
				
				for var i = 0; i < Int(theNo) * 2; i += 2 {
					var aStr = pascalStringFromData(playListDat, index: i)
					var aStr2 = pascalStringFromData(playListDat, index: i + 1)
					if aStr == nil || aStr2 == nil {
						break
					}
					if let aStr = aStr, aStr2 = aStr2, CFaStr = pascalStringToString(aStr), CFaStr2 = pascalStringToString(aStr2) {
						var fullPath: NSURL?
						
						let tmpPath = CreateURLWithHFSPathStyle(kCFAllocatorDefault, CFaStr, true) as NSURL
						fullPath = tmpPath.URLByAppendingPathComponent(CFaStr2)
						if !fullPath!.checkResourceIsReachableAndReturnError(nil) {
							fullPath = nil
						}
						
						if fullPath == nil {
							let together = ":".join([CFaStr, CFaStr2])
							
							fullPath = CreateURLWithHFSPathStyle(kCFAllocatorDefault, together, true)
						}
						
						if let fullPath = fullPath where fullPath.checkResourceIsReachableAndReturnError(nil) {
							newArray.append(fullPath.path!)
							continue
						}
					}
					
					if (location != -1) && (location == Int16(i / 2)) {
						location = -1;
					} else if (location != -1 && location > Int16(i / 2)) {
						location--;
					}
					lostMusicCount++;
				}
				let selectedMusic = (Int(location) <= newArray.count) ? Int(location) : -1;

				outDict = ["MusicPaths":		newArray,
				 "SelectedMusic":	selectedMusic,
				 "lostMusicCount":	lostMusicCount]
			} else {
				return NSError(domain: NSCocoaErrorDomain, code: NSFileReadCorruptFileError, userInfo: nil)
			}
		} else {
			return iErr
		}
		
		return nil
	}
	
	public func loadStcfAtURL(theURL: NSURL, withReply reply: ([NSObject : AnyObject]?, NSError?) -> Void) {
		var myDict = [NSObject : AnyObject]()
		if let myErr = loadOldMusicListAtURL(theURL, toDictionary: &myDict) {
			reply(nil, myErr)
		} else {
			reply(myDict, nil)
		}
	}
	
	public func listener(listener: NSXPCListener, shouldAcceptNewConnection newConnection: NSXPCConnection) -> Bool {
		newConnection.exportedInterface = NSXPCInterface(withProtocol: PPSTImporterHelper.self)
		newConnection.exportedObject = self
		newConnection.resume()
		
		return true
	}
}
