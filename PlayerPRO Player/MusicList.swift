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
	private let PPPPath = (try! NSFileManager.default().urlForDirectory(.applicationSupportDirectory, in:.userDomainMask, appropriateFor:nil, create:true)).appendingPathComponent("PlayerPRO").appendingPathComponent("Player", isDirectory: true)
#elseif os(iOS)
	private let listExtension = "pplist"
	private let PPPPath = (try! NSFileManager.defaultManager().URLForDirectory(.DocumentDirectory, inDomain: .UserDomainMask, appropriateForURL: nil, create: true)).URLByAppendingPathComponent("Playlists", isDirectory: true)
#endif

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, Collection {
	private(set)	dynamic var musicList = [MusicListObject]()
	private(set)	var lostMusicCount: UInt
	dynamic var		selectedMusic: Int
	#if os(iOS)
	dynamic var		name = "New Music List"
	private(set)	var fileUUID = NSUUID()
	#endif
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject>!, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
	
	func makeIterator() -> IndexingIterator<[MusicListObject]> {
		return musicList.makeIterator();
	}
	
	subscript (index: Int) -> MusicListObject {
		return musicList[index]
	}

	func index(after: Int) -> Int {
		return after + 1
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
		for (i, obj) in musicList.enumerated() {
			if obj.pointsToFile(url: theURL) {
				return i
			}
		}
	
		return nil
	}
	
	/// Returns `nil` if the URL couldn't be found.
	@objc(indexesOfObjectsSimilarToURL:) func indexesOfObjectsSimilar(URL theURL: NSURL) -> NSIndexSet? {
		let anIDXSet = NSMutableIndexSet()
		
		for (i, obj) in musicList.enumerated() {
			if obj.pointsToFile(url: theURL) {
				anIDXSet.add(i)
			}
		}
		
		if anIDXSet.count == 0 {
			return nil
		} else {
			return anIDXSet
		}
	}
	
	func clearMusicList() {
		let theIndex = NSIndexSet(indexesIn: NSRange(location: 0, length: musicList.count))
		self.willChange(.removal, valuesAt: theIndex, forKey: kMusicListKVO)
		musicList.removeAll()
		self.didChange(.removal, valuesAt: theIndex, forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingBlock:) func sortMusicList(block: (lhs: MusicListObject, rhs: MusicListObject) -> Bool) {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(isOrderedBefore: block)
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingComparator:) func sortMusicList(comparator: NSComparator) {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort( isOrderedBefore: { (obj1, obj2) -> Bool in
			return comparator(obj1, obj2) == .orderedAscending
		})
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	func sortMusicListByName() {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(isOrderedBefore: { (var1, var2) -> Bool in
			let result = var1.fileName.localizedStandardCompare(var2.fileName)
			return result == .orderedAscending
			})
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingDescriptors:) func sortMusicList(descriptors: [NSSortDescriptor]) {
		let anArray = musicList.sort(using: descriptors)
		musicList = anArray
	}
	
	enum AddMusicStatus {
		case Failure
		case Success
		case SimilarURL
	}
	
	func addMusicURL(_ theURL: NSURL?, force: Bool = false) -> AddMusicStatus {
		if theURL == nil {
			return .Failure
		}
		
		let obj = MusicListObject(url: theURL!)
		
		if !force {
			let tmpArray = musicList.filter({ (obj2) -> Bool in
				return obj2.pointsToFile(url: obj.musicURL)
			})
			if tmpArray.count > 0 {
				return .SimilarURL
			}
		}
		
		let theIndex = NSIndexSet(index:musicList.count);
		self.willChange(.insertion, valuesAt: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.insertion, valuesAt: theIndex, forKey: kMusicListKVO)
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
		if let BookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey4) as? [MusicListObject] {
			selectedMusic = aDecoder.decodeInteger(forKey: kMusicListLocation4)
			#if os(iOS)
				if let aName = aDecoder.decodeObjectForKey(kMusicListName4) as? String {
					name = aName
				}
			#endif
			for book in BookmarkArray {
				if !book.checkIsReachableAndReturnError(error: nil) {
					if (selectedMusic == -1) {
						//Do nothing
					} else if (selectedMusic == musicList.count + 1) {
						selectedMusic = -1
					} else if (selectedMusic > musicList.count + 1) {
						selectedMusic -= 1
					}
					lostMusicCount += 1
					continue
				}
				musicList.append(book)
			}
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey3) as? [NSURL] {
			selectedMusic = aDecoder.decodeInteger(forKey: kMusicListLocation3)
			// Have all the new MusicListObjects use the same date
			let currentDate = NSDate()
			for bookURL in bookmarkArray {
				if !bookURL.checkResourceIsReachableAndReturnError(nil) {
					if (selectedMusic == -1) {
						//Do nothing
					} else if (selectedMusic == musicList.count + 1) {
						selectedMusic = -1
					} else if (selectedMusic > musicList.count + 1) {
						selectedMusic -= 1
					}
					lostMusicCount += 1
					continue
				}
				let obj = MusicListObject(url: bookURL, date: currentDate)
				musicList.append(obj)
			}
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey2) as? [NSData] {
			if let curSel = aDecoder.decodeObject(forKey: kMusicListLocation2) as? Int {
				selectedMusic = curSel
			} else {
				selectedMusic = -1
			}
			let aHomeURL = NSURL(fileURLWithPath: NSHomeDirectory(), isDirectory: true)
			// Have all the new MusicListObjects use the same date
			let currentDate = NSDate()
			for bookData in bookmarkArray {
				if let fullURL = MusicListObject(bookmarkData: bookData, resolutionOptions: .withoutUI, relativeURL: aHomeURL, date: currentDate) {
					musicList.append(fullURL)
				} else {
					if (selectedMusic == -1) {
						//Do nothing
					} else if (selectedMusic == musicList.count + 1) {
						selectedMusic = -1
					} else if (selectedMusic > musicList.count + 1) {
						selectedMusic -= 1
					}
					lostMusicCount += 1
				}
			}
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey1) as? [NSData] {
			// Have all the new MusicListObjects use the same date
			let currentDate = NSDate()
			for bookData in bookmarkArray {
				if let fullURL = MusicListObject(bookmarkData: bookData, resolutionOptions: .withoutUI, date: currentDate) {
					musicList.append(fullURL)
				} else {
					lostMusicCount += 1
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

	func encode(with aCoder: NSCoder) {
		aCoder.encode(selectedMusic, forKey: kMusicListLocation4)
		aCoder.encode(musicList as NSArray, forKey: kMusicListKey4)
		#if os(iOS)
			aCoder.encodeObject(name, forKey: kMusicListName4)
		#endif
	}
	
	class func supportsSecureCoding() -> Bool {
		return true
	}
	
	//MARK: -
	func URLAtIndex(_ index: Int) -> NSURL {
		assert(index < musicList.count)
		return musicList[index].musicURL
	}
	
	// MARK: - saving/loading
	private func loadMusicList(_ newArray: [MusicListObject]) {
		willChangeValue(forKey: kMusicListKVO)
		musicList = newArray
		didChangeValue(forKey: kMusicListKVO)
	}
	
	private func loadMusicList(from theData: NSData) -> Bool {
		if let postList = NSKeyedUnarchiver.unarchiveObject(with: theData) as? MusicList {
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
	
	func loadMusicList(from fromURL: NSURL) -> Bool {
		if let unWrappedListData = NSData(contentsOf: fromURL) {
			return loadMusicList(from: unWrappedListData)
		} else {
			return false
		}
	}
	
	func loadApplicationMusicList() -> Bool {
		let manager = NSFileManager.default()
		#if os(OSX)
			let musListDefName = "PlayerPRO Music List"
			let defaults = NSUserDefaults.standard()
			if let listData = defaults.data(forKey: musListDefName) {
				if loadMusicList(from: listData) {
					defaults.removeObject(forKey: musListDefName) //Otherwise the preference file is abnormally large.
					_ = saveApplicationMusicList()
					//Technically we did succeed...
					return true
				}
				//We couldn't load it, but it's still there, taking up space...
				defaults.removeObject(forKey: musListDefName)
			}
		#endif
		if !PPPPath.checkResourceIsReachableAndReturnError(nil) {
			do {
				try manager.createDirectory(at: PPPPath, withIntermediateDirectories: true, attributes: nil)
			} catch _ {
				return false
			}
			
		}
		return loadMusicList(from: PPPPath.appendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	@objc(saveMusicListToURL:) func saveMusicList(url URL: NSURL) -> Bool {
		let theList = NSKeyedArchiver.archivedData(withRootObject: self)
		return theList.write(to: URL, atomically: true)
	}
	
	func saveApplicationMusicList() -> Bool {
		let manager = NSFileManager.default()
		
		if !PPPPath.checkResourceIsReachableAndReturnError(nil) {
			do {
				//Just making sure...
				try manager.createDirectory(at: PPPPath, withIntermediateDirectories: true, attributes:nil)
			} catch _ {
				return false
			}
		}
		
		return self.saveMusicList(url: PPPPath.appendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// MARK: - Key-valued Coding
	@objc(addMusicListObject:) func add(_ object: MusicListObject) {
		if !musicList.contains(object) {
			musicList.append(object)
		}
	}
	
	var countOfMusicList: Int {
		return musicList.count
	}
	
	@objc(replaceObjectInMusicListAtIndex:withObject:) func replaceObjectInMusicList(at index: Int, with object: MusicListObject) {
		musicList[index] = object;
	}
	
	@objc(objectInMusicListAtIndex:) func objectInMusicList(at index: Int) -> MusicListObject {
		return musicList[index];
	}
	
	@objc(removeMusicListAtIndexes:) func remove(at idxSet: NSIndexSet) {
		if idxSet.contains(selectedMusic) {
			selectedMusic = -1;
		}
		
		_ = musicList.remove(indexes: idxSet)
	}
	
	@objc(removeObjectInMusicListAtIndex:) func remove(at atIndex: Int) {
		musicList.remove(at: atIndex);
	}
	
	@objc(insertObject:inMusicListAtIndex:) func insert(_ object: MusicListObject, at index: Int) {
		musicList.insert(object, at: index)
	}
	
	@objc(arrayOfObjectsInMusicListAtIndexes:) func objectsInMusicList(at theSet : NSIndexSet) -> [MusicListObject] {
		return musicList.filter({ (include) -> Bool in
			let idx = self.musicList.index(of: include)!
			return theSet.contains(idx)
		})
	}
	
	@objc(insertMusicLists:atIndexes:) func insertMusicLists(_ anObj: [MusicListObject], at indexes: NSIndexSet) {
		var idx = anObj.endIndex
		var i = indexes.lastIndex
		while i != NSNotFound {
			idx -= 1
			musicList.insert(anObj[idx], at: i)
			i = indexes.indexLessThanIndex(i)
		}
	}
	
	@objc(insertObjects:inMusicListAtIndexes:) func insert(_ objs: [MusicListObject], at indexes: NSIndexSet) {
		insertMusicLists(objs, at: indexes)
	}
	
	@objc(removeObjectsInMusicListAtIndexes:) func removeObjectsInMusicListAtIndexes(_ idxSet: NSIndexSet) {
		remove(at: idxSet)
	}
	
	@objc(insertObjects:inMusicListAtIndex:) func insert(_ anObj: [MusicListObject], at idx: Int) {
		let theIndexSet = NSIndexSet(indexesIn: NSRange(location: idx, length: anObj.count))
		self.willChange(.insertion, valuesAt: theIndexSet, forKey: kMusicListKVO)
		var currentIndex = theIndexSet.firstIndex
		let count = theIndexSet.count
		
		for i in 0 ..< count {
			let tempObj = anObj[i]
			musicList.insert(tempObj, at: currentIndex)
			currentIndex = theIndexSet.indexGreaterThanIndex(currentIndex)
		}

		self.didChange(.insertion, valuesAt: theIndexSet, forKey: kMusicListKVO)
	}
	
	#if os(OSX)
	func beginLoadingOfOldMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError?) -> Void) {
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(with: PPSTImporterHelper.self)
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcf(at: toOpen, withReply: {(bookmarkData:[String : AnyObject]?, error: NSError?) -> Void in
			NSOperationQueue.main().addOperation({
				defer {
					conn.invalidate()
				}
				if error != nil {
					theHandle(theErr: error)
				} else {
					guard let invalidAny = bookmarkData!["lostMusicCount"] as? UInt,
						selectedAny = bookmarkData!["SelectedMusic"] as? Int,
						pathsAny = bookmarkData!["MusicPaths"] as? NSArray as? [String] else {
							let lolwut = NSError(domain: NSCocoaErrorDomain, code: NSXPCConnectionReplyInvalid, userInfo: [NSLocalizedDescriptionKey: "Invalid data returned from helper"])
							theHandle(theErr: lolwut)
							return;
					}
					var pathsURL = [MusicListObject]()
					// Have all the new MusicListObjects use the same date
					let currentDate = NSDate()
					for aPath in pathsAny {
						let tmpURL = NSURL.fileURL(withPath: aPath)
						let tmpObj = MusicListObject(url: tmpURL, date: currentDate)
						pathsURL.append(tmpObj)
					}
					self.loadMusicList(pathsURL)
					self.lostMusicCount = invalidAny
					self.selectedMusic = selectedAny
					
					theHandle(theErr: nil)
				}
			})
		})
	}
	#endif
}
