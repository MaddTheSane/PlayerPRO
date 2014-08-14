//
//  PPMusicList.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation
import PlayerPROKit

let kMusicListLocation3 = "Music Key Location 3";
let kMusicListKey3 = "Music List Key 3"
let kPlayerList = "Player List"

class PPMusicList: NSObject, NSSecureCoding, NSFastEnumeration {
	private(set) var musicList = [PPMusicListObject]()
	internal(set) var lostMusicCount:UInt = 0;
	internal(set) var selectedMusic = -1;
	
	override class func initialize() {
		NSKeyedUnarchiver.setClass(self, forClassName: "PPMusicList")
		NSKeyedArchiver.setClassName("PPMusicList", forClass: self)
	}
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len);
	}
	
	func encodeWithCoder(aCoder: NSCoder!) {
		var BookmarkArray: [NSURL] = [];
		for obj in musicList {
			var bookData : NSURL! = obj.musicURL;
			if (bookData != nil) {
				BookmarkArray.append(bookData)
			}
		}
		//TODO: check for failed data initialization, and decrement changedIndex to match.
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation3);
		aCoder.encodeObject(BookmarkArray, forKey: kMusicListKey3)

	}
	
	func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		return (musicList as NSArray).indexOfObject(theURL)
	}
	
	func clearMusicList() {
		let theIndex = NSIndexSet(indexesInRange: NSMakeRange(0, musicList.count))
		self.willChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.removeAll()
		self.didChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
	}
	
	func sortMusicListByName() {
		musicList.sort({
			(isOrderedBefore: (var1:PPMusicListObject, var2:PPMusicListObject)) -> Bool in
			let rhsString: NSString = isOrderedBefore.var1.fileName
			let lhsString: NSString = isOrderedBefore.var2.fileName
			var result = rhsString.localizedStandardCompare(lhsString)
			return result == NSComparisonResult.OrderedAscending;
			})
	}
	
	func addMusicURL(theURL: NSURL) -> Bool {
		var obj: PPMusicListObject! = PPMusicListObject(URL: theURL);
		
		if (!obj) {
			return false;
		}
		
		if ((musicList as NSArray).containsObject(obj)) {
			return false;
		}
		
		var theIndex = NSIndexSet(index:musicList.count);
		self.willChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		return true;
	}
	
	func saveMusicListToURL(URL : NSURL) -> Bool {
		var theList = NSKeyedArchiver.archivedDataWithRootObject(self);
		return theList.writeToURL(URL, atomically: true)
	}
	
	func saveApplicationMusicList() -> Bool {
		var manager = NSFileManager.defaultManager();

		var PPPPath = manager.URLForDirectory(NSSearchPathDirectory.ApplicationSupportDirectory, inDomain:NSSearchPathDomainMask.UserDomainMask, appropriateForURL:nil, create:true, error:nil).URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player");
		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			//Just making sure...
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil, error:nil);
		}
		
		return self.saveMusicListToURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false));
	}
	
	override init() {
		
		
		super.init();
	}
	
	required init(coder aDecoder: NSCoder!) {
		lostMusicCount = 0;
		var BookmarkArray : NSArray = aDecoder.decodeObjectForKey(kMusicListKey3) as NSArray;
		selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation3);
		musicList.removeAll();
		for bookURL : AnyObject in BookmarkArray {
			var BookURLURL = bookURL as NSURL;
			if (!BookURLURL.checkResourceIsReachableAndReturnError(nil)) {
				if (selectedMusic == -1) {
					//Do nothing
				} else if (selectedMusic == musicList.count + 1) {
					selectedMusic = -1;
				} else if (selectedMusic > musicList.count + 1) {
					selectedMusic--;
				}
				lostMusicCount++;
				continue;
			}
			var obj = PPMusicListObject(URL:BookURLURL);
			musicList.append(obj)
		}
		super.init()
	}

	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func URLAtIndex(index :Int) -> NSURL {
		return musicList[index].musicURL;
	}
	
	func loadMusicList(newArray: [PPMusicListObject])
	{
		self.willChangeValueForKey(kMusicListKVO);
		musicList = newArray;
		self.didChangeValueForKey(kMusicListKVO);
	}
	
	func loadMusicListFromData(theData: NSData) -> Bool {
		var preList: PPMusicList! = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as PPMusicList
		if (!preList) {
			return false
		}
		
		lostMusicCount = preList.lostMusicCount
		loadMusicList(preList.musicList)
		self.selectedMusic = preList.selectedMusic
		return true
	}
	
	func loadMusicListAtURL(fromURL: NSURL) -> Bool {
		var listData: NSData! = NSData(contentsOfURL: fromURL)
		if (!listData) {
			return false
		}
		return loadMusicListFromData(listData)
	}
	
	func loadApplicationMusicList() -> Bool {
		var manager = NSFileManager.defaultManager();
		var PPPPath = manager.URLForDirectory(NSSearchPathDirectory.ApplicationSupportDirectory, inDomain:NSSearchPathDomainMask.UserDomainMask, appropriateForURL:nil, create:true, error:nil).URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player");
		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories: true, attributes: nil, error: nil)
			return false;
		}
		return loadMusicListAtURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// Key-valued Coding
	
	func addMusicListObject(object:PPMusicListObject)
	{
		if ((musicList as NSArray).containsObject(object) == false) {
			musicList.append(object)
		}
	}
	
	var countOfMusicList: Int {get {
		return musicList.count
		}}
	
	func replaceObjectInMusicListAtIndex(index: Int, withObject object: PPMusicListObject)
	{
		musicList[index] = object;
	}
	
	func objectInMusicListAtIndex(index: Int) -> PPMusicListObject
	{
		return musicList[index];
	}
	
	func removeObjectInMusicListAtIndex(atIndex: Int)
	{
		musicList.removeAtIndex(atIndex);
	}
	
	func insertObject(object :PPMusicListObject, inMusicListAtIndex index:Int)
	{
		musicList.insert(object, atIndex: index)
	}
	
	func arrayOfObjectsInMusicListAtIndexes(theSet : NSIndexSet) -> NSArray {
		return (musicList as NSArray).objectsAtIndexes(theSet)
	}
	
	func removeObjectsInMusicListAtIndexes(idxSet: NSIndexSet) {
		if (idxSet.containsIndex(selectedMusic)) {
			self.selectedMusic = -1;
		}
		let musicArray = musicList as NSArray
		self.willChange(.Removal, valuesAtIndexes: idxSet, forKey: kMusicListKVO)
		musicList = musicList.filter({
			let idx = musicArray.indexOfObject($0)
			if (idxSet.containsIndex(idx)) {
				return false
			} else {
				return true
			}
			})
		self.didChange(.Removal, valuesAtIndexes: idxSet, forKey: kMusicListKVO)
	}
	
	func insertObjects(anObj: NSArray, inMusicListAtIndex idx:Int) {
		let theIndexSet = NSIndexSet(indexesInRange: NSMakeRange(idx, anObj.count))
		self.willChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
		var currentIndex = theIndexSet.firstIndex;
		var count = theIndexSet.count;
		
		for (var i = 0; i < count; i++) {
			var tempObj = anObj.objectAtIndex(i) as PPMusicListObject
			musicList.insert(tempObj, atIndex: currentIndex)
			currentIndex = theIndexSet.indexGreaterThanIndex(currentIndex);
		}

		self.didChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
	}
	
	#if os(OSX)
	func beginLoadingOfMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError!) ->Void) {
		var conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(`protocol`: PPSTImporterHelper.self);
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[NSObject : AnyObject]!, error: NSError!) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if (error != nil) {
					theHandle(theErr: error)
				} else {
					var invalidAny: AnyObject? = bookmarkData["lostMusicCount"];
					var selectedAny: AnyObject? = bookmarkData["SelectedMusic"]
					var pathsAny: AnyObject? = bookmarkData["MusicPaths"]
					if (invalidAny == nil || selectedAny == nil || pathsAny == nil) {
						var lolwut = CreateErrorFromMADErrorType(.UnknownErr)
						theHandle(theErr: lolwut)
					} else {
						var pathsURL: [PPMusicListObject] = []
						self.lostMusicCount = invalidAny as UInt;
						self.selectedMusic = selectedAny as Int;
						for aPath in pathsAny as NSArray {
							var tmpURL = NSURL.fileURLWithPath(aPath as String)
							if (!tmpURL) {
								continue;
							}
							var tmpObj = PPMusicListObject(URL: tmpURL)
							pathsURL.append(tmpObj)
						}
						self.loadMusicList(pathsURL)
						
						theHandle(theErr: nil)
					}
				}
				
				conn.invalidate();
			})
		})
	}
	#endif

}
