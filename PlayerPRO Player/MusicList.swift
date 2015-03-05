//
//  PPMusicList.swift
//  SwiftTest
//
//  Created by C.W. Betts on 7/15/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import Foundation
import SwiftAdditions

private let kMusicListKey1 = "Music List Key1"
private let kMusicListKey2 = "Music List Key2"
private let kMusicListKey3 = "Music List Key 3"

private let kMusicListLocation2 = "Music Key Location2"
private let kMusicListLocation3 = "Music Key Location 3"

private let kMusicListKey4 = "Music List Key 4"
private let kMusicListLocation4 = "Music Key Location 4"

private let kPlayerList = "Player List"

private let PPPPath = NSFileManager.defaultManager().URLForDirectory(.ApplicationSupportDirectory, inDomain:.UserDomainMask, appropriateForURL:nil, create:true, error:nil)!.URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player")

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, CollectionType, Sliceable {
	private(set)	dynamic var musicList = [MusicListObject]()
	private(set)	var lostMusicCount: UInt
	dynamic var		selectedMusic: Int
	
	func countByEnumeratingWithState(state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumeratingWithState(state, objects: buffer, count: len)
	}
	
	func generate() -> IndexingGenerator<[MusicListObject]> {
		return musicList.generate();
	}
	
	subscript (index: Int) -> MusicListObject {
		return musicList[index]
	}

	var startIndex: Int {
		return 0
	}

	var endIndex: Int {
		return musicList.count
	}
	
	subscript (subRange: Range<Int>) -> Slice<MusicListObject> {
		return musicList[subRange]
	}
	
	/// Returns NSNotFound if a URL couldn't be found.
	/// Use indexOfObjectSimilar(URL) on Swift instead of this one.
	func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		return indexOfObjectSimilar(URL: theURL) ?? NSNotFound
	}
	
	/// This cannot be represented in Objective C
	func indexOfObjectSimilar(URL theURL: NSURL) -> Int? {
		for (i, obj) in enumerate(musicList) {
			if obj == theURL {
				return i
			}
		}
	
		return nil
	}
	
	func clearMusicList() {
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
	
	func sortMusicListByName() {
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
	
	func addMusicURL(theURL: NSURL?) -> Bool {
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
	
	func saveMusicListToURL(URL: NSURL) -> Bool {
		var theList = NSKeyedArchiver.archivedDataWithRootObject(self)
		return theList.writeToURL(URL, atomically: true)
	}
	
	func saveApplicationMusicList() -> Bool {
		let manager = NSFileManager.defaultManager()

		if (!PPPPath.checkResourceIsReachableAndReturnError(nil)) {
			//Just making sure...
			manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil, error:nil)
		}
		
		return self.saveMusicListToURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false))
	}
	
	override init() {
		lostMusicCount = 0
		selectedMusic = -1
		
		super.init();
	}
	
	// MARK: - NSCoding
	
	required init(coder aDecoder: NSCoder) {
		lostMusicCount = 0;
		if let BookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey4) as? [MusicListObject] {
			selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation4);
			for book in BookmarkArray {
				if (!book.checkIsReachableAndReturnError(nil)) {
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
				musicList.append(book)
			}
		} else if let bookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey3) as? [NSURL] {
			selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation3);
			for bookURL in bookmarkArray {
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
		} else if let bookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey2) as? [NSData] {
			if let curSel = aDecoder.decodeObjectForKey(kMusicListLocation2) as? Int {
				selectedMusic = curSel
			} else {
				selectedMusic = -1
			}
			let aHomeURL = NSURL(fileURLWithPath: NSHomeDirectory(), isDirectory: true)
			for bookData in bookmarkArray {
				if let fullURL = MusicListObject(bookmarkData: bookData, resolutionOptions: .WithoutUI, relativeURL: aHomeURL) {
					musicList.append(fullURL)
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
		} else if let bookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey1) as? [NSData] {
			for bookData in bookmarkArray {
				if let fullURL = MusicListObject(bookmarkData: bookData, resolutionOptions: .WithoutUI) {
					musicList.append(fullURL)
				} else {
					lostMusicCount++
				}
			}
			selectedMusic = -1;
		} else {
			selectedMusic = -1
		}
		
		super.init()
	}

	func encodeWithCoder(aCoder: NSCoder) {
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation4)
		aCoder.encodeObject(musicList as NSArray, forKey: kMusicListKey4)
	}
	
	class func supportsSecureCoding() -> Bool {
		return true
	}
	
	//MARK: -
	func URLAtIndex(index: Int) -> NSURL? {
		if index >= musicList.count {
			return nil
		}
		return musicList[index].musicURL
	}
	
	// MARK: - saving/loading
	private func loadMusicList(newArray: [MusicListObject]) {
		self.willChangeValueForKey(kMusicListKVO)
		musicList = newArray
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	private func loadMusicListFromData(theData: NSData) -> Bool {
		if let postList = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as? MusicList {
			lostMusicCount = postList.lostMusicCount
			loadMusicList(postList.musicList)
			self.selectedMusic = postList.selectedMusic
			return true
		} else {
			return false
		}
	}
	
	func loadMusicListFromURL(fromURL: NSURL) -> Bool {
		if let unWrappedListData = NSData(contentsOfURL:fromURL) {
			return loadMusicListFromData(unWrappedListData)
		} else {
			return false
		}
	}
	
	func loadApplicationMusicList() -> Bool {
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
	func addMusicListObject(object: MusicListObject) {
		if !contains(musicList, object) {
			musicList.append(object)
		}
	}
	
	var countOfMusicList: Int {
		return musicList.count
	}
	
	func replaceObjectInMusicListAtIndex(index: Int, withObject object: MusicListObject) {
		musicList[index] = object;
	}
	
	func objectInMusicListAtIndex(index: Int) -> MusicListObject {
		return musicList[index];
	}
	
	func removeMusicListAtIndexes(idxSet: NSIndexSet) {
		if idxSet.containsIndex(selectedMusic) {
			selectedMusic = -1;
		}
		
		removeObjects(inArray: &musicList, atIndexes: idxSet)
	}
	
	func removeObjectInMusicListAtIndex(atIndex: Int) {
		musicList.removeAtIndex(atIndex);
	}
	
	func insertObject(object: MusicListObject, inMusicListAtIndex index: Int) {
		musicList.insert(object, atIndex: index)
	}
	
	func arrayOfObjectsInMusicListAtIndexes(theSet : NSIndexSet) -> [MusicListObject] {
		return musicList.filter({ (include) -> Bool in
			var idx = find(self.musicList, include)!
			return theSet.containsIndex(idx)
		})
	}
	
	func insertMusicLists(anObj: [MusicListObject], atIndexes indexes: NSIndexSet) {
		var idx = anObj.endIndex
		for var i = indexes.lastIndex; i != NSNotFound; i = indexes.indexLessThanIndex(i) {
			musicList.insert(anObj[--idx], atIndex: i)
		}
	}
	
	func insertObjects(objs: [MusicListObject], inMusicListAtIndexes indexes: NSIndexSet) {
		insertMusicLists(objs, atIndexes: indexes)
	}
	
	func removeObjectsInMusicListAtIndexes(idxSet: NSIndexSet) {
		removeMusicListAtIndexes(idxSet)
	}
	
	func insertObjects(anObj: [MusicListObject], inMusicListAtIndex idx:Int) {
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
		conn.remoteObjectInterface = NSXPCInterface(withProtocol: PPSTImporterHelper.self)
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[NSObject : AnyObject]?, error: NSError?) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if error != nil {
					theHandle(theErr: error)
				} else {
					let invalidAny = bookmarkData!["lostMusicCount"] as? UInt
					let selectedAny = bookmarkData!["SelectedMusic"] as? Int
					let pathsAny = bookmarkData!["MusicPaths"] as? NSArray as? [String]
					if (invalidAny == nil || selectedAny == nil || pathsAny == nil) {
						let lolwut = createErrorFromMADErrorType(.UnknownErr)!
						theHandle(theErr: lolwut)
					} else {
						var pathsURL = [MusicListObject]()
						for aPath in pathsAny! {
							if let tmpURL = NSURL.fileURLWithPath(aPath) {
								let tmpObj = MusicListObject(URL: tmpURL)
								pathsURL.append(tmpObj)
							}
						}
						self.loadMusicList(pathsURL)
						self.lostMusicCount = invalidAny!
						self.selectedMusic = selectedAny!
						
						theHandle(theErr: nil)
					}
				}
				conn.invalidate()
			})
		})
	}
	#endif
}
