//
//  PPMusicList.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation

private let kMusicListKey1 = "Music List Key1"
private let kMusicListKey2 = "Music List Key2"

private let kMusicListLocation2 = "Music Key Location2"

private let kMusicListKey3 = "Music List Key 3"
private let kMusicListLocation3 = "Music Key Location 3"

private let kPlayerList = "Player List"

private let PPPPath = NSFileManager.defaultManager().URLForDirectory(.ApplicationSupportDirectory, inDomain:.UserDomainMask, appropriateForURL:nil, create:true, error:nil)!.URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player")

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, SequenceType {
	@objc private(set)	dynamic var musicList = [MusicListObject]()
	@objc private(set)	var lostMusicCount: UInt
	@objc dynamic var	selectedMusic: Int
	
	@objc func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len);
	}
	
	func generate() -> IndexingGenerator<[MusicListObject]> {
		return musicList.generate();
	}
	
	@objc subscript (index: Int) -> MusicListObject {
		return musicList[index]
	}
	
	@objc func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		let theInd: Int? = indexOfObjectSimilarToURL(theURL)
		if theInd != nil {
			return theInd!
		} else {
			return NSNotFound
		}
	}
	
	//This cannot be represented in Objective C
	func indexOfObjectSimilarToURL(theURL: NSURL) -> Int? {
		for (i, obj) in enumerate(musicList) {
			if obj == theURL {
				return i
			}
		}
	
		return nil
	}
	
	@objc func clearMusicList() {
		let theIndex = NSIndexSet(indexesInRange: NSRange(location: 0, length: musicList.count))
		self.willChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.removeAll()
		self.didChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
	}
	
	@objc func sortMusicListUsingComparator(comparator: NSComparator) {
		musicList.sort { (obj1, obj2) -> Bool in
			return comparator(obj1, obj2) == NSComparisonResult.OrderedAscending
		}
	}
	
	@objc func sortMusicListByName() {
		musicList.sort({
			(var1, var2) -> Bool in
			let result = var1.fileName.localizedStandardCompare(var2.fileName)
			return result == NSComparisonResult.OrderedAscending;
			})
	}
	
	@objc func addMusicURL(theURL: NSURL) -> Bool {
		var obj: MusicListObject! = MusicListObject(URL: theURL);
		
		if (obj == nil) {
			return false;
		}
		
		if ((musicList as NSArray).containsObject(obj)) {
			return false;
		}
		
		let theIndex = NSIndexSet(index:musicList.count);
		self.willChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		return true;
	}
	
	@objc func saveMusicListToURL(URL: NSURL) -> Bool {
		var theList = NSKeyedArchiver.archivedDataWithRootObject(self);
		return theList.writeToURL(URL, atomically: true)
	}
	
	@objc func saveApplicationMusicList() -> Bool {
		let manager = NSFileManager.defaultManager();

		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			//Just making sure...
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil, error:nil);
		}
		
		return self.saveMusicListToURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false));
	}
	
	@objc override init() {
		lostMusicCount = 0
		selectedMusic = -1
		
		super.init();
	}
	
	required init(coder aDecoder: NSCoder) {
		lostMusicCount = 0;
		var BookmarkArray: AnyObject? = aDecoder.decodeObjectForKey(kMusicListKey3);
		if (BookmarkArray == nil) {
			BookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey2)
			if (BookmarkArray == nil) {
				BookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey1)
				if (BookmarkArray == nil) {
					selectedMusic = -1
					super.init()
					return;
				}
				let dataBookArray = BookmarkArray! as [NSData]
				
				//musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
				for bookData in dataBookArray {
					var isStale: ObjCBool = false;
					let fullURL = NSURL.URLByResolvingBookmarkData(bookData, options: .WithoutUI, relativeToURL: nil, bookmarkDataIsStale: &isStale, error: nil)
					#if DEBUG
						if isStale {
							println("Bookmark pointing to \(fullURL.path) is stale");
						}
					#endif
					if fullURL == nil {
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
				let dataBookArray = BookmarkArray! as [NSData]
				let aHomeURL = NSURL(fileURLWithPath: NSHomeDirectory(), isDirectory: true)
				for bookData in dataBookArray {
					var isStale: ObjCBool = false;
					let fullURL = NSURL.URLByResolvingBookmarkData(bookData, options: .WithoutUI, relativeToURL: aHomeURL, bookmarkDataIsStale: &isStale, error: nil)
					#if DEBUG
						if isStale {
							println("Bookmark pointing to \(fullURL.path) is stale.");
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
			let URLBookArray = BookmarkArray! as [NSURL]
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
				let obj = MusicListObject(URL: bookURL);
				musicList.append(obj)
			}
			
		}
		super.init()
	}

	func encodeWithCoder(aCoder: NSCoder) {
		var BookmarkArray = [NSURL]()
		for obj in musicList {
			let bookData = obj.musicURL;
			BookmarkArray.append(bookData)
		}
		//TODO: check for failed data initialization, and decrement changedIndex to match.
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation3);
		aCoder.encodeObject(BookmarkArray, forKey: kMusicListKey3)
	}
	
	class func supportsSecureCoding() -> Bool {
		return true;
	}
	
	@objc func URLAtIndex(index: Int) -> NSURL {
		return musicList[index].musicURL;
	}
	
	@objc func loadMusicList(newArray: [MusicListObject]) {
		self.willChangeValueForKey(kMusicListKVO);
		musicList = newArray;
		self.didChangeValueForKey(kMusicListKVO);
	}
	
	private func loadMusicListFromData(theData: NSData) -> Bool {
		var preList = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as MusicList?
		if let postList = preList {
			lostMusicCount = postList.lostMusicCount
			loadMusicList(postList.musicList)
			self.selectedMusic = postList.selectedMusic
			return true
		} else {
			return false
		}
	}
	
	@objc func loadMusicListAtURL(fromURL: NSURL) -> Bool {
		var listData: NSData? = NSData(contentsOfURL:fromURL)
		if let unWrappedListData = listData {
			return loadMusicListFromData(unWrappedListData)
		} else {
			return false
		}
	}
	
	private func clearMusicListInDefaults() {
		let musListDefName = "PlayerPRO Music List"
		let defaults = NSUserDefaults.standardUserDefaults()
		if let listData = defaults.dataForKey(musListDefName) {
			defaults.removeObjectForKey(musListDefName)
		}
	}
	
	@objc func loadApplicationMusicList() -> Bool {
		clearMusicListInDefaults() //Otherwise the preference file is abnormally large.
		let manager = NSFileManager.defaultManager();
		if (PPPPath.checkResourceIsReachableAndReturnError(nil) == false) {
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories: true, attributes: nil, error: nil)
			return false;
		}
		return loadMusicListAtURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// MARK: - Key-valued Coding
	@objc dynamic func addMusicListObject(object: MusicListObject) {
		if (musicList as NSArray).containsObject(object) == false {
			musicList.append(object)
		}
	}
	
	@objc dynamic var countOfMusicList: Int {
		get {
			return musicList.count
		}
	}
	
	@objc dynamic func replaceObjectInMusicListAtIndex(index: Int, withObject object: MusicListObject) {
		musicList[index] = object;
	}
	
	@objc dynamic func objectInMusicListAtIndex(index: Int) -> MusicListObject {
		return musicList[index];
	}
	
	@objc dynamic func removeObjectInMusicListAtIndex(atIndex: Int) {
		musicList.removeAtIndex(atIndex);
	}
	
	@objc dynamic func insertObject(object: MusicListObject, inMusicListAtIndex index: Int) {
		musicList.insert(object, atIndex: index)
	}
	
	@objc dynamic func arrayOfObjectsInMusicListAtIndexes(theSet : NSIndexSet) -> [MusicListObject] {
		var tmpList = [MusicListObject]()
		for (i, obj) in enumerate(musicList) {
			if theSet.containsIndex(i) {
				tmpList.append(obj)
			}
		}
		return tmpList
	}
	
	@objc dynamic func removeObjectsInMusicListAtIndexes(idxSet: NSIndexSet) {
		if idxSet.containsIndex(selectedMusic) {
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
	
	@objc dynamic func insertObjects(anObj: [MusicListObject], inMusicListAtIndex idx:Int) {
		let theIndexSet = NSIndexSet(indexesInRange: NSRange(location: idx, length: anObj.count))
		self.willChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
		var currentIndex = theIndexSet.firstIndex;
		let count = theIndexSet.count;
		
		for (var i = 0; i < count; i++) {
			let tempObj = anObj[i]
			musicList.insert(tempObj, atIndex: currentIndex)
			currentIndex = theIndexSet.indexGreaterThanIndex(currentIndex);
		}

		self.didChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
	}
	
	#if os(OSX)
	@objc func beginLoadingOfOldMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError?) ->Void) {
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(`protocol`: PPSTImporterHelper.self);
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[NSObject : AnyObject]!, error: NSError!) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if (error != nil) {
					theHandle(theErr: error)
				} else {
					let invalidAny: AnyObject? = bookmarkData["lostMusicCount"];
					let selectedAny: AnyObject? = bookmarkData["SelectedMusic"]
					let pathsAny: AnyObject? = bookmarkData["MusicPaths"]
					if (invalidAny == nil || selectedAny == nil || pathsAny == nil) {
						let lolwut = CreateErrorFromMADErrorType(.UnknownErr)
						theHandle(theErr: lolwut)
					} else {
						var pathsURL = [MusicListObject]()
						self.lostMusicCount = invalidAny as UInt;
						self.selectedMusic = selectedAny as Int;
						for aPath in pathsAny as NSArray as [NSString] {
							let tmpURL = NSURL.fileURLWithPath(aPath)
							if (tmpURL == nil) {
								continue;
							}
							let tmpObj = MusicListObject(URL: tmpURL!)
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
