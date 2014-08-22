//
//  PPMusicList.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation

private let kMUSICLISTKEY = "Music List Key1"
private let kMUSICLISTKEY2 = "Music List Key2"
private let kMusicListLocation2 = "Music Key Location2"

private let kMusicListLocation3 = "Music Key Location 3";
private let kMusicListKey3 = "Music List Key 3"
private let kPlayerList = "Player List"

private func homeURL() -> NSURL {
	return NSURL(fileURLWithPath: NSHomeDirectory(), isDirectory: true)
}

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, SequenceType {
	private(set)	dynamic var musicList = [MusicListObject]()
	private(set)	var lostMusicCount:UInt = 0;
	
	var		selectedMusic = -1;
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len);
	}
	
	func generate() -> IndexingGenerator<[MusicListObject]> {
		return musicList.generate();
	}
	
	@objc subscript (index: Int) -> MusicListObject {
		return musicList[index]
	}
	
	func encodeWithCoder(aCoder: NSCoder) {
		var BookmarkArray: [NSURL] = [];
		for obj in musicList {
			var bookData : NSURL = obj.musicURL;
			BookmarkArray.append(bookData)
		}
		//TODO: check for failed data initialization, and decrement changedIndex to match.
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation3);
		aCoder.encodeObject(BookmarkArray, forKey: kMusicListKey3)
	}
	
	func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		for (i, obj) in enumerate(musicList) {
			if obj == theURL {
				return i
			}
		}
		
		return NSNotFound
	}
	
	func clearMusicList() {
		let theIndex = NSIndexSet(indexesInRange: NSMakeRange(0, musicList.count))
		self.willChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.removeAll()
		self.didChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
	}
	
	func sortMusicListByName() {
		musicList.sort({
			(var1:MusicListObject, var2:MusicListObject) -> Bool in
			let rhsString: NSString = var1.fileName
			let lhsString: NSString = var2.fileName
			let result = rhsString.localizedStandardCompare(lhsString)
			return result == NSComparisonResult.OrderedAscending;
			})
	}
	
	func addMusicURL(theURL: NSURL) -> Bool {
		var obj: MusicListObject! = MusicListObject(URL: theURL);
		
		if (obj == nil) {
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

		var PPPPath = manager.URLForDirectory(NSSearchPathDirectory.ApplicationSupportDirectory, inDomain:NSSearchPathDomainMask.UserDomainMask, appropriateForURL:nil, create:true, error:nil)!.URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player");
		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			//Just making sure...
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil, error:nil);
		}
		
		return self.saveMusicListToURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false));
	}
	
	override init() {
		
		
		super.init();
	}
	
	required init(coder aDecoder: NSCoder) {
		super.init()
		lostMusicCount = 0;
		var BookmarkArray: AnyObject? = aDecoder.decodeObjectForKey(kMusicListKey3);
		if (BookmarkArray == nil) {
			BookmarkArray = aDecoder.decodeObjectForKey(kMUSICLISTKEY2)
			if (BookmarkArray == nil) {
				BookmarkArray = aDecoder.decodeObjectForKey(kMUSICLISTKEY)
				if (BookmarkArray == nil) {
					return;
				}
				let dataBookArray = BookmarkArray as [NSData]
				
				//musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
				for bookData in dataBookArray {
					var isStale: ObjCBool = false;
					let fullURL = NSURL.URLByResolvingBookmarkData(bookData, options: .WithoutUI, relativeToURL: nil, bookmarkDataIsStale: &isStale, error: nil)
					#if DEBUG
						if (isStale) {
						NSLog("Bookmark pointing to %@ is stale", fullURL.path);
						}
					#endif
					if (fullURL == nil) {
						lostMusicCount++;
						continue;
					}
					let obj = MusicListObject(URL: fullURL);
					musicList.append(obj)
				}
				selectedMusic = -1;
			} else {
				var curSel = aDecoder.decodeObjectForKey(kMusicListLocation2) as NSNumber?
				if curSel != nil {
					selectedMusic = curSel!.integerValue
				} else {
					selectedMusic = -1
				}
				//musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
				let dataBookArray = BookmarkArray as [NSData]
				for bookData in dataBookArray {
					var isStale: ObjCBool = false;
					let fullURL = NSURL.URLByResolvingBookmarkData(bookData, options: .WithoutUI, relativeToURL: homeURL(), bookmarkDataIsStale: &isStale, error: nil)
					#if DEBUG
						if (isStale) {
						NSLog("Bookmark pointing to \(fullURL.path) is stale.");
						}
					#endif
					if (fullURL == nil) {
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
					let obj = MusicListObject(URL: fullURL);
					musicList.append(obj)
				}
			}
		} else {
			selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation3);
			//musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
			let URLBookArray = BookmarkArray as [NSURL]
			for bookURL in URLBookArray {
				if (!bookURL.checkResourceIsReachableAndReturnError(nil)) {
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
				var obj = MusicListObject(URL: bookURL);
				musicList.append(obj)
			}
			
		}
	}

	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	func URLAtIndex(index :Int) -> NSURL {
		return musicList[index].musicURL;
	}
	
	func loadMusicList(newArray: [MusicListObject])
	{
		self.willChangeValueForKey(kMusicListKVO);
		musicList = newArray;
		self.didChangeValueForKey(kMusicListKVO);
	}
	
	func loadMusicListFromData(theData: NSData) -> Bool {
		var preList = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as MusicList?
		if (preList == nil) {
			return false
		}
		
		lostMusicCount = preList!.lostMusicCount
		loadMusicList(preList!.musicList)
		self.selectedMusic = preList!.selectedMusic
		return true
	}
	
	func loadMusicListAtURL(fromURL: NSURL) -> Bool {
		var listData = NSData.dataWithContentsOfURL(fromURL, options: NSDataReadingOptions(0), error: nil)
		if (listData == nil) {
			return false
		}
		return loadMusicListFromData(listData)
	}
	
	func loadApplicationMusicList() -> Bool {
		var manager = NSFileManager.defaultManager();
		var PPPPath = manager.URLForDirectory(NSSearchPathDirectory.ApplicationSupportDirectory, inDomain:NSSearchPathDomainMask.UserDomainMask, appropriateForURL:nil, create:true, error:nil)!.URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player");
		if (PPPPath.checkResourceIsReachableAndReturnError(nil) == false) {
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories: true, attributes: nil, error: nil)
			return false;
		}
		return loadMusicListAtURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// Key-valued Coding
	
	func addMusicListObject(object: MusicListObject)
	{
		if ((musicList as NSArray).containsObject(object) == false) {
			musicList.append(object)
		}
	}
	
	var countOfMusicList: Int {get {
		return musicList.count
		}}
	
	func replaceObjectInMusicListAtIndex(index: Int, withObject object: MusicListObject)
	{
		musicList[index] = object;
	}
	
	func objectInMusicListAtIndex(index: Int) -> MusicListObject
	{
		return musicList[index];
	}
	
	func removeObjectInMusicListAtIndex(atIndex: Int)
	{
		musicList.removeAtIndex(atIndex);
	}
	
	func insertObject(object: MusicListObject, inMusicListAtIndex index:Int)
	{
		musicList.insert(object, atIndex: index)
	}
	
	func arrayOfObjectsInMusicListAtIndexes(theSet : NSIndexSet) -> [MusicListObject] {
		var tmpList = [MusicListObject]()
		for (i, obj) in enumerate(musicList) {
			if theSet.containsIndex(i) {
				tmpList.append(obj)
			}
		}
		return tmpList
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
	
	func insertObjects(anObj: [MusicListObject], inMusicListAtIndex idx:Int) {
		let theIndexSet = NSIndexSet(indexesInRange: NSRange(location: idx, length: anObj.count))
		self.willChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
		var currentIndex = theIndexSet.firstIndex;
		var count = theIndexSet.count;
		
		for (var i = 0; i < count; i++) {
			let tempObj = anObj[i]
			musicList.insert(tempObj, atIndex: currentIndex)
			currentIndex = theIndexSet.indexGreaterThanIndex(currentIndex);
		}

		self.didChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
	}
	
	#if os(OSX)
	func beginLoadingOfMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError!) ->Void) {
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
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
						var pathsURL = [MusicListObject]()
						self.lostMusicCount = invalidAny as UInt;
						self.selectedMusic = selectedAny as Int;
						for aPath in pathsAny as NSArray {
							var tmpURL = NSURL.fileURLWithPath(aPath as String)
							if (tmpURL == nil) {
								continue;
							}
							var tmpObj = MusicListObject(URL: tmpURL)
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
