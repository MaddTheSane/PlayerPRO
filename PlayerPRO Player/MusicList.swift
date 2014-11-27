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
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
	
	func generate() -> IndexingGenerator<[MusicListObject]> {
		return musicList.generate();
	}
	
	@objc subscript (index: Int) -> MusicListObject {
		return musicList[index]
	}
	
	@objc func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		if let theInd = indexOfObjectSimilar(toURL: theURL) {
			return theInd
		} else {
			return NSNotFound
		}
	}
	
	//This cannot be represented in Objective C
	func indexOfObjectSimilar(toURL theURL: NSURL) -> Int? {
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
	
	@objc(sortMusicListUsingBlock:) func sortMusicList(#block: (lhs: MusicListObject, rhs: MusicListObject) -> Bool) {
		self.willChangeValueForKey(kMusicListKVO)
		musicList.sort(block)
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@objc(sortMusicListUsingComparator:) func sortMusicList(#comparator: NSComparator) {
		self.willChangeValueForKey(kMusicListKVO)
		musicList.sort { (obj1, obj2) -> Bool in
			return comparator(obj1, obj2) == NSComparisonResult.OrderedAscending
		}
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@objc func sortMusicListByName() {
		self.willChangeValueForKey(kMusicListKVO)
		musicList.sort({ (var1, var2) -> Bool in
			let result = var1.fileName.localizedStandardCompare(var2.fileName)
			return result == NSComparisonResult.OrderedAscending
			})
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@objc(sortMusicListUsingDescriptors:) func sortMusicList(#descriptors: [NSSortDescriptor]) {
		for descriptor in descriptors {
			#if os(OSX)
			let descriptorKey = descriptor.key()!
				#elseif os(iOS)
				let descriptorKey = descriptor.key!
				#endif
			switch descriptorKey {
			case "fileName":
				sortMusicList(block: { (lhs, rhs) -> Bool in
					let result = lhs.fileName.localizedStandardCompare(rhs.fileName)
					if descriptor.ascending {
						return result == NSComparisonResult.OrderedAscending
					} else {
						return result == NSComparisonResult.OrderedDescending
					}
				})
				
			case "fileSize":
				sortMusicList(block: { (lhs, rhs) -> Bool in
					if descriptor.ascending {
						return lhs.fileSize < rhs.fileSize
					} else {
						return lhs.fileSize > rhs.fileSize
					}
				})
				
			default:
				break
			}
		}
	}
	
	@objc func addMusicURL(theURL: NSURL?) -> Bool {
		if theURL == nil {
			return false
		}
		
		let obj = MusicListObject(URL: theURL!)
		
		if contains(musicList, obj) {
			return false
		}
		
		let theIndex = NSIndexSet(index:musicList.count);
		self.willChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		return true
	}
	
	@objc func saveMusicListToURL(URL: NSURL) -> Bool {
		var theList = NSKeyedArchiver.archivedDataWithRootObject(self)
		return theList.writeToURL(URL, atomically: true)
	}
	
	@objc func saveApplicationMusicList() -> Bool {
		let manager = NSFileManager.defaultManager()

		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			//Just making sure...
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil, error:nil)
		}
		
		return self.saveMusicListToURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false))
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
					return
				}
				let dataBookArray = BookmarkArray! as [NSData]
				
				//musicList = [[NSMutableArray alloc] initWithCapacity:[BookmarkArray count]];
				for bookData in dataBookArray {
					var isStale: ObjCBool = false;
					if let fullURL = NSURL(byResolvingBookmarkData: bookData, options: .WithoutUI, relativeToURL: nil, bookmarkDataIsStale: &isStale, error: nil) {
						let obj = MusicListObject(URL: fullURL)
						musicList.append(obj)
					} else {
						lostMusicCount++
					}
				}
				selectedMusic = -1;
			} else {
				var curSel = aDecoder.decodeObjectForKey(kMusicListLocation2) as NSNumber?
				if curSel != nil {
					selectedMusic = curSel!.integerValue
				} else {
					selectedMusic = -1
				}
				let dataBookArray = BookmarkArray! as [NSData]
				let aHomeURL = NSURL(fileURLWithPath: NSHomeDirectory(), isDirectory: true)
				for bookData in dataBookArray {
					var isStale: ObjCBool = false
					if let fullURL = NSURL(byResolvingBookmarkData: bookData, options: .WithoutUI, relativeToURL: aHomeURL, bookmarkDataIsStale: &isStale, error: nil) {
						musicList.append(MusicListObject(URL: fullURL))
					} else {
						if (selectedMusic == -1) {
							//Do nothing
						} else if (selectedMusic == musicList.count + 1) {
							selectedMusic = -1
						} else if (selectedMusic > musicList.count + 1) {
							selectedMusic--
						}
						lostMusicCount++
					}
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
						selectedMusic = -1
					} else if (selectedMusic > musicList.count + 1) {
						selectedMusic--
					}
					lostMusicCount++
					continue
				}
				let obj = MusicListObject(URL: bookURL)
				musicList.append(obj)
			}
			
		}
		super.init()
	}

	func encodeWithCoder(aCoder: NSCoder) {
		var BookmarkArray = [NSURL]()
		for obj in musicList {
			let bookData = obj.musicURL
			BookmarkArray.append(bookData)
		}
		//TODO: check for failed data initialization, and decrement changedIndex to match.
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation3)
		aCoder.encodeObject(BookmarkArray, forKey: kMusicListKey3)
	}
	
	class func supportsSecureCoding() -> Bool {
		return true
	}
	
	@objc func URLAtIndex(index: Int) -> NSURL? {
		if index >= musicList.count {
			return nil
		}
		return musicList[index].musicURL
	}
	
	private func loadMusicList(newArray: [MusicListObject]) {
		self.willChangeValueForKey(kMusicListKVO)
		musicList = newArray
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	private func loadMusicListFromData(theData: NSData) -> Bool {
		if let postList = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as MusicList? {
			lostMusicCount = postList.lostMusicCount
			loadMusicList(postList.musicList)
			self.selectedMusic = postList.selectedMusic
			return true
		} else {
			return false
		}
	}
	
	@objc func loadMusicListFromURL(fromURL: NSURL) -> Bool {
		if let unWrappedListData = NSData(contentsOfURL:fromURL) {
			return loadMusicListFromData(unWrappedListData)
		} else {
			return false
		}
	}
	
	@objc func loadApplicationMusicList() -> Bool {
		let musListDefName = "PlayerPRO Music List"
		let defaults = NSUserDefaults.standardUserDefaults()
		let manager = NSFileManager.defaultManager()
		if let listData = defaults.dataForKey(musListDefName) {
			if loadMusicListFromData(listData) {
				defaults.removeObjectForKey(musListDefName) //Otherwise the preference file is abnormally large.
				saveApplicationMusicList()
				//Technically we did succeed...
				return true
			}
			defaults.removeObjectForKey(musListDefName)
		}
		if (PPPPath.checkResourceIsReachableAndReturnError(nil) == false) {
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories: true, attributes: nil, error: nil)
			return false
		}
		return loadMusicListFromURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// MARK: - Key-valued Coding
	@objc dynamic func addMusicListObject(object: MusicListObject) {
		if !contains(musicList, object) {
			musicList.append(object)
		}
	}
	
	@objc dynamic var countOfMusicList: Int {
		return musicList.count
	}
	
	@objc dynamic func replaceObjectInMusicListAtIndex(index: Int, withObject object: MusicListObject) {
		musicList[index] = object;
	}
	
	@objc dynamic func objectInMusicListAtIndex(index: Int) -> MusicListObject {
		return musicList[index];
	}
	
	@objc dynamic func removeMusicListAtIndexes(idxSet: NSIndexSet) {
		if idxSet.containsIndex(selectedMusic) {
			selectedMusic = -1;
		}
		
		for var i = idxSet.lastIndex; i != NSNotFound; i = idxSet.indexLessThanIndex(i) {
			musicList.removeAtIndex(i)
		}
	}
	
	@objc dynamic func removeObjectInMusicListAtIndex(atIndex: Int) {
		musicList.removeAtIndex(atIndex);
	}
	
	@objc dynamic func insertObject(object: MusicListObject, inMusicListAtIndex index: Int) {
		musicList.insert(object, atIndex: index)
	}
	
	@objc dynamic func arrayOfObjectsInMusicListAtIndexes(theSet : NSIndexSet) -> [MusicListObject] {
		return musicList.filter({ (include) -> Bool in
			var idx = find(self.musicList, include)
			return theSet.containsIndex(idx!)
		})
	}
	
	func insertMusicLists(anObj: Array<MusicListObject>, atIndexes indexes: NSIndexSet) {
		var idx = anObj.endIndex
		for var i = indexes.lastIndex; i != NSNotFound; i = indexes.indexLessThanIndex(i) {
			musicList.insert(anObj[--idx], atIndex: i)
		}
	}
	
	func insertObjects(objs: [MusicListObject], inMusicListAtIndexes indexes: NSIndexSet) {
		insertMusicLists(objs, atIndexes: indexes)
	}
	
	@objc func removeObjectsInMusicListAtIndexes(idxSet: NSIndexSet) {
		removeMusicListAtIndexes(idxSet)
	}
	
	@objc dynamic func insertObjects(anObj: [MusicListObject], inMusicListAtIndex idx:Int) {
		let theIndexSet = NSIndexSet(indexesInRange: NSRange(location: idx, length: anObj.count))
		self.willChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
		var currentIndex = theIndexSet.firstIndex
		let count = theIndexSet.count
		
		for i in 0 ..< count {
			let tempObj = anObj[i]
			musicList.insert(tempObj, atIndex: currentIndex)
			currentIndex = theIndexSet.indexGreaterThanIndex(currentIndex)
		}

		self.didChange(.Insertion, valuesAtIndexes: theIndexSet, forKey: kMusicListKVO)
	}
	
	#if os(OSX)
	@objc func beginLoadingOfOldMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError?) ->Void) {
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(`protocol`: PPSTImporterHelper.self)
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[NSObject : AnyObject]!, error: NSError?) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if error != nil {
					theHandle(theErr: error)
				} else {
					let invalidAny: AnyObject? = bookmarkData["lostMusicCount"]
					let selectedAny: AnyObject? = bookmarkData["SelectedMusic"]
					let pathsAny: AnyObject? = bookmarkData["MusicPaths"]
					if (invalidAny == nil || selectedAny == nil || pathsAny == nil) {
						let lolwut = CreateErrorFromMADErrorType(.UnknownErr)
						theHandle(theErr: lolwut)
					} else {
						var pathsURL = [MusicListObject]()
						self.lostMusicCount = invalidAny as NSNumber as UInt
						self.selectedMusic = selectedAny as NSNumber as Int
						for aPath in pathsAny as NSArray as [NSString] {
							if let tmpURL = NSURL.fileURLWithPath(aPath) {
								let tmpObj = MusicListObject(URL: tmpURL)
								pathsURL.append(tmpObj)
							}
						}
						self.loadMusicList(pathsURL)
						
						theHandle(theErr: nil)
					}
				}
				conn.invalidate()
			})
		})
	}
	#endif
}
