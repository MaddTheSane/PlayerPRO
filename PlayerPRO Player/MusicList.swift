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
private let kMusicListName4 = "Music List Name 4"

private let kPlayerList = "Player List"

#if os(OSX)
	private let PPPPath = (try! NSFileManager.defaultManager().URLForDirectory(.ApplicationSupportDirectory, inDomain:.UserDomainMask, appropriateForURL:nil, create:true)).URLByAppendingPathComponent("PlayerPRO").URLByAppendingPathComponent("Player", isDirectory: true)
	#elseif os(iOS)
	private let listExtension = "pplist"
	private let PPPPath = (try! NSFileManager.defaultManager().URLForDirectory(.DocumentDirectory, inDomain: .UserDomainMask, appropriateForURL: nil, create: true)).URLByAppendingPathComponent("Playlists", isDirectory: true)
#endif

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, CollectionType {
	private(set)	dynamic var musicList = [MusicListObject]()
	private(set)	var lostMusicCount: UInt
	dynamic var		selectedMusic: Int
	#if os(iOS)
	dynamic var		name = "New Music List"
	private(set)	var fileUUID = NSUUID()
	#endif
	
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
	
	subscript (subRange: Range<Int>) -> ArraySlice<MusicListObject> {
		return musicList[subRange]
	}
	
	/// Returns `NSNotFound` if the URL couldn't be found.
	///
	/// Use `indexOfObjectSimilar(URL:)` on Swift instead.
	func indexOfObjectSimilarToURL(theURL: NSURL) -> Int {
		return indexOfObjectSimilar(URL: theURL) ?? NSNotFound
	}
	
	/// Returns `nil` if the URL couldn't be found.
	///
	/// This cannot be represented in Objective C.
	func indexOfObjectSimilar(URL theURL: NSURL) -> Int? {
		for (i, obj) in musicList.enumerate() {
			if obj.pointsToFile(URL: theURL) {
				return i
			}
		}
	
		return nil
	}
	
	/// Returns `nil` if the URL couldn't be found.
	@objc(indexesOfObjectsSimilarToURL:) func indexesOfObjectsSimilar(URL theURL: NSURL) -> NSIndexSet? {
		let anIDXSet = NSMutableIndexSet()
		
		for (i, obj) in musicList.enumerate() {
			if obj.pointsToFile(URL: theURL) {
				anIDXSet.addIndex(i)
			}
		}
		
		if anIDXSet.count == 0 {
			return nil
		} else {
			return anIDXSet
		}
	}
	
	func clearMusicList() {
		let theIndex = NSIndexSet(indexesInRange: NSRange(location: 0, length: musicList.count))
		self.willChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.removeAll()
		self.didChange(.Removal, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingBlock:) func sortMusicList(block block: (lhs: MusicListObject, rhs: MusicListObject) -> Bool) {
		self.willChangeValueForKey(kMusicListKVO)
		musicList.sortInPlace(block)
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@objc(sortMusicListUsingComparator:) func sortMusicList(comparator comparator: NSComparator) {
		self.willChangeValueForKey(kMusicListKVO)
		musicList.sortInPlace { (obj1, obj2) -> Bool in
			return comparator(obj1, obj2) == .OrderedAscending
		}
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	func sortMusicListByName() {
		self.willChangeValueForKey(kMusicListKVO)
		musicList.sortInPlace({ (var1, var2) -> Bool in
			let result = var1.fileName.localizedStandardCompare(var2.fileName)
			return result == .OrderedAscending
			})
		self.didChangeValueForKey(kMusicListKVO)
	}
	
	@objc(sortMusicListUsingDescriptors:) func sortMusicList(descriptors descriptors: [NSSortDescriptor]) {
		let anArray = sortedArray(musicList, usingDescriptors: descriptors)
		musicList = anArray as! [MusicListObject]
	}
	
	enum AddMusicStatus {
		case Failure
		case Success
		case SimilarURL
	}
	
	func addMusicURL(theURL: NSURL?, force: Bool = false) -> AddMusicStatus {
		if theURL == nil {
			return .Failure
		}
		
		let obj = MusicListObject(URL: theURL!)
		
		if !force {
			let tmpArray = musicList.filter({ (obj2) -> Bool in
				return obj2.pointsToFile(URL: obj.musicURL)
			})
			if tmpArray.count > 0 {
				return .SimilarURL
			}
		}
		
		let theIndex = NSIndexSet(index:musicList.count);
		self.willChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.Insertion, valuesAtIndexes: theIndex, forKey: kMusicListKVO)
		return .Success
	}
	
	override init() {
		lostMusicCount = 0
		selectedMusic = -1
		
		super.init()
	}
	
	#if os(iOS)
	class func availablePlaylistUUIDs() -> [NSUUID]! {
		let fm = NSFileManager.defaultManager()
		let docDir = try! fm.URLForDirectory(.DocumentDirectory, inDomain: .UserDomainMask, appropriateForURL: nil, create: true)
		let playlistDir = docDir.URLByAppendingPathComponent("Playlists")
		do {
			try fm.createDirectoryAtURL(playlistDir, withIntermediateDirectories: true, attributes: nil)
			let dirConts = try fm.contentsOfDirectoryAtURL(playlistDir, includingPropertiesForKeys: nil, options: [])
			var toRetUUIDs = [NSUUID]()
			for url in dirConts {
				if let fileUUIDStr = (url.lastPathComponent as NSString?)?.stringByDeletingPathExtension, fileUUID = NSUUID(UUIDString: fileUUIDStr) {
					toRetUUIDs.append(fileUUID)
				}
			}
			return toRetUUIDs
			
		} catch {
			return nil
		}
	}
	
	convenience init?(UUID: NSUUID) {
		let path = PPPPath.URLByAppendingPathComponent(UUID.UUIDString).URLByAppendingPathExtension(listExtension)
		self.init()
		fileUUID = UUID
		if !loadMusicListFromURL(path) {
			return nil
		}
	}
	
	func save() -> Bool {
		let saveURL = PPPPath.URLByAppendingPathComponent(fileUUID.UUIDString).URLByAppendingPathExtension(listExtension)
		return saveMusicList(URL: saveURL)
	}
	#endif
	
	// MARK: - NSCoding
	
	required init?(coder aDecoder: NSCoder) {
		lostMusicCount = 0;
		if let BookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey4) as? [MusicListObject] {
			selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation4)
			#if os(iOS)
				if let aName = aDecoder.decodeObjectForKey(kMusicListName4) as? String {
					name = aName
				}
			#endif
			for book in BookmarkArray {
				if !book.checkIsReachableAndReturnError(nil) {
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
			selectedMusic = aDecoder.decodeIntegerForKey(kMusicListLocation3)
			// Have all the new MusicListObjects use the same date
			let currentDate = NSDate()
			for bookURL in bookmarkArray {
				if !bookURL.checkResourceIsReachableAndReturnError(nil) {
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
				let obj = MusicListObject(URL: bookURL, date: currentDate)
				musicList.append(obj)
			}
		} else if let bookmarkArray = aDecoder.decodeObjectForKey(kMusicListKey2) as? [NSData] {
			if let curSel = aDecoder.decodeObjectForKey(kMusicListLocation2) as? Int {
				selectedMusic = curSel
			} else {
				selectedMusic = -1
			}
			let aHomeURL = NSURL(fileURLWithPath: NSHomeDirectory(), isDirectory: true)
			// Have all the new MusicListObjects use the same date
			let currentDate = NSDate()
			for bookData in bookmarkArray {
				if let fullURL = MusicListObject(bookmarkData: bookData, resolutionOptions: .WithoutUI, relativeURL: aHomeURL, date: currentDate) {
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
			// Have all the new MusicListObjects use the same date
			let currentDate = NSDate()
			for bookData in bookmarkArray {
				if let fullURL = MusicListObject(bookmarkData: bookData, resolutionOptions: .WithoutUI, date: currentDate) {
					musicList.append(fullURL)
				} else {
					lostMusicCount++
				}
			}
			selectedMusic = -1;
		} else {
			selectedMusic = -1
			super.init()
			return nil
		}
		
		super.init()
	}

	func encodeWithCoder(aCoder: NSCoder) {
		aCoder.encodeInteger(selectedMusic, forKey: kMusicListLocation4)
		aCoder.encodeObject(musicList as NSArray, forKey: kMusicListKey4)
		#if os(iOS)
			aCoder.encodeObject(name, forKey: kMusicListName4)
		#endif
	}
	
	class func supportsSecureCoding() -> Bool {
		return true
	}
	
	//MARK: -
	func URLAtIndex(index: Int) -> NSURL {
		assert(index < musicList.count)
		return musicList[index].musicURL
	}
	
	// MARK: - saving/loading
	private func loadMusicList(newArray: [MusicListObject]) {
		willChangeValueForKey(kMusicListKVO)
		musicList = newArray
		didChangeValueForKey(kMusicListKVO)
	}
	
	private func loadMusicListFromData(theData: NSData) -> Bool {
		if let postList = NSKeyedUnarchiver.unarchiveObjectWithData(theData) as? MusicList {
			lostMusicCount = postList.lostMusicCount
			loadMusicList(postList.musicList)
			self.selectedMusic = postList.selectedMusic
			#if os(iOS)
				name = postList.name
			#endif
			return true
		} else {
			return false
		}
	}
	
	func loadMusicListFromURL(fromURL: NSURL) -> Bool {
		if let unWrappedListData = NSData(contentsOfURL: fromURL) {
			return loadMusicListFromData(unWrappedListData)
		} else {
			return false
		}
	}
	
	func loadApplicationMusicList() -> Bool {
		let manager = NSFileManager.defaultManager()
		#if os(OSX)
			let musListDefName = "PlayerPRO Music List"
			let defaults = NSUserDefaults.standardUserDefaults()
			if let listData = defaults.dataForKey(musListDefName) {
				if loadMusicListFromData(listData) {
					defaults.removeObjectForKey(musListDefName) //Otherwise the preference file is abnormally large.
					saveApplicationMusicList()
					//Technically we did succeed...
					return true
				}
				//We couldn't load it, but it's still there, taking up space...
				defaults.removeObjectForKey(musListDefName)
			}
		#endif
		if !PPPPath.checkResourceIsReachableAndReturnError(nil) {
			do {
				try manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories: true, attributes: nil)
			} catch _ {
				return false
			}
			
		}
		return loadMusicListFromURL(PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	@objc(saveMusicListToURL:) func saveMusicList(URL URL: NSURL) -> Bool {
		let theList = NSKeyedArchiver.archivedDataWithRootObject(self)
		return theList.writeToURL(URL, atomically: true)
	}
	
	func saveApplicationMusicList() -> Bool {
		let manager = NSFileManager.defaultManager()
		
		if !PPPPath.checkResourceIsReachableAndReturnError(nil) {
			do {
				//Just making sure...
				try manager.createDirectoryAtURL(PPPPath, withIntermediateDirectories:true, attributes:nil)
			} catch _ {
				return false
			}
		}
		
		return self.saveMusicList(URL: PPPPath.URLByAppendingPathComponent(kPlayerList, isDirectory:false))
	}
	
	// MARK: - Key-valued Coding
	func addMusicListObject(object: MusicListObject) {
		if !musicList.contains(object) {
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
			let idx = self.musicList.indexOf(include)!
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
	@objc func beginLoadingOfOldMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError?) -> Void) {
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(withProtocol: PPSTImporterHelper.self)
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[String : AnyObject]?, error: NSError?) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if error != nil {
					theHandle(theErr: error)
				} else {
					let invalidAny = bookmarkData!["lostMusicCount"] as? UInt
					let selectedAny = bookmarkData!["SelectedMusic"] as? Int
					let pathsAny = bookmarkData!["MusicPaths"] as? NSArray as? [String]
					if (invalidAny == nil || selectedAny == nil || pathsAny == nil) {
						let lolwut = NSError(domain: NSCocoaErrorDomain, code: NSXPCConnectionReplyInvalid, userInfo: [NSLocalizedDescriptionKey: "Invalid data returned from helper"])
						theHandle(theErr: lolwut)
					} else {
						var pathsURL = [MusicListObject]()
						// Have all the new MusicListObjects use the same date
						let currentDate = NSDate()
						for aPath in pathsAny! {
							let tmpURL = NSURL.fileURLWithPath(aPath)
							let tmpObj = MusicListObject(URL: tmpURL, date: currentDate)
							pathsURL.append(tmpObj)
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
