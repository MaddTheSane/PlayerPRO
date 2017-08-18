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
	private let PPPPath: URL = {
		do {
			var retURL = try FileManager.default.url(for: .applicationSupportDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
			retURL.appendPathComponent("PlayerPRO")
			retURL.appendPathComponent("Player", isDirectory: true)
			return retURL
		} catch {
			fatalError("Encountered error with locating Application Support directory: \(error)")
		}
	}()
#elseif os(iOS)
	private let listExtension = "pplist"
	private let PPPPath = (try! FileManager.default.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)).appendingPathComponent("Playlists", isDirectory: true)
#endif

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, Collection {
	@objc private(set)	dynamic var musicList = [MusicListObject]()
	private(set)	var lostMusicCount: UInt
	@objc dynamic var		selectedMusic: Int
	#if os(iOS)
	@objc dynamic var		name = "New Music List"
	private(set)	var fileUUID = UUID()
	#endif
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
	
	func makeIterator() -> IndexingIterator<[MusicListObject]> {
		return musicList.makeIterator();
	}
	
	subscript (index: Int) -> MusicListObject {
		return musicList[index]
	}

	@nonobjc
	func index(after: Int) -> Int {
		return after + 1
	}

	@nonobjc
	var startIndex: Int {
		return 0
	}

	@nonobjc
	var endIndex: Int {
		return musicList.count
	}
	
	subscript (subRange: Range<Int>) -> ArraySlice<MusicListObject> {
		return musicList[subRange]
	}
	
	/// Returns `NSNotFound` if the URL couldn't be found.
	///
	/// Use `indexOfObjectSimilar(URL:)` on Swift instead.
	func indexOfObjectSimilarToURL(_ theURL: URL) -> Int {
		return indexOfObjectSimilar(to: theURL) ?? NSNotFound
	}
	
	/// Returns `nil` if the URL couldn't be found.
	///
	/// This cannot be represented in Objective C.
	func indexOfObjectSimilar(to theURL: URL) -> Int? {
		for (i, obj) in musicList.enumerated() {
			if obj.pointsToFile(at: theURL) {
				return i
			}
		}
	
		return nil
	}
	
	/// Returns `nil` if the URL couldn't be found.
	@objc(indexesOfObjectsSimilarToURL:)
	func indexesOfObjectsSimilar(to theURL: URL) -> IndexSet? {
		var anIDXSet = IndexSet()
		
		for (i, obj) in musicList.enumerated() {
			if obj.pointsToFile(at: theURL) {
				anIDXSet.insert(i)
			}
		}
		
		if anIDXSet.count == 0 {
			return nil
		} else {
			return anIDXSet
		}
	}
	
	func clearMusicList() {
		let theIndex = NSIndexSet(indexesIn: NSRange(location: 0, length: musicList.count)) as IndexSet
		self.willChange(.removal, valuesAt: theIndex, forKey: kMusicListKVO)
		musicList.removeAll()
		self.didChange(.removal, valuesAt: theIndex, forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingBlock:)
	func sortMusicList(block: (_ lhs: MusicListObject, _ rhs: MusicListObject) -> Bool) {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(by: block)
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingComparator:)
	func sortMusicList(comparator: Comparator) {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(by: { (obj1, obj2) -> Bool in
			return comparator(obj1, obj2) == .orderedAscending
		})
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	func sortMusicListByName() {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(by: { (var1, var2) -> Bool in
			let result = var1.fileName.localizedStandardCompare(var2.fileName)
			return result == .orderedAscending
			})
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingDescriptors:)
	func sortMusicList(descriptors: [NSSortDescriptor]) {
		let anArray = musicList.sorted(using: descriptors)
		musicList = anArray
	}
	
	enum AddMusicStatus {
		case failure
		case success
		case similarURL
	}
	
	func addMusicURL(_ theURL: URL?, force: Bool = false) -> AddMusicStatus {
		guard let theURL = theURL else {
			return .failure
		}
		
		let obj = MusicListObject(url: theURL)
		
		if !force {
			let tmpArray = musicList.filter({ (obj2) -> Bool in
				return obj2.pointsToFile(at: obj.musicURL)
			})
			if tmpArray.count > 0 {
				return .similarURL
			}
		}
		
		let theIndex = IndexSet(integer: musicList.count);
		self.willChange(.insertion, valuesAt: theIndex, forKey: kMusicListKVO)
		musicList.append(obj)
		self.didChange(.insertion, valuesAt: theIndex, forKey: kMusicListKVO)
		return .success
	}
	
	override init() {
		lostMusicCount = 0
		selectedMusic = -1
		
		super.init()
	}
	
	#if os(iOS)
	class func availablePlaylistUUIDs() -> [UUID]! {
		let fm = FileManager.default
		let docDir = try! fm.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
		let playlistDir = docDir.appendingPathComponent("Playlists")
		do {
			try fm.createDirectory(at: playlistDir, withIntermediateDirectories: true, attributes: nil)
			let dirConts = try fm.contentsOfDirectory(at: playlistDir, includingPropertiesForKeys: nil, options: [])
			var toRetUUIDs = [UUID]()
			for url in dirConts {
				if let fileUUIDStr = (url.lastPathComponent as NSString?)?.deletingPathExtension, let fileUUID = UUID(uuidString: fileUUIDStr) {
					toRetUUIDs.append(fileUUID)
				}
			}
			return toRetUUIDs
			
		} catch {
			return nil
		}
	}
	
	convenience init?(uuid: UUID) {
		let path = PPPPath.appendingPathComponent(uuid.uuidString).appendingPathExtension(listExtension)
		self.init()
		fileUUID = uuid
		if !loadMusicList(from: path) {
			return nil
		}
	}
	
	@discardableResult
	func save() -> Bool {
		let saveURL = PPPPath.appendingPathComponent(fileUUID.uuidString).appendingPathExtension(listExtension)
		return saveMusicList(to: saveURL)
	}
	#endif
	
	// MARK: - NSCoding
	
	required init?(coder aDecoder: NSCoder) {
		lostMusicCount = 0;
		if let BookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey4) as? [MusicListObject] {
			selectedMusic = aDecoder.decodeInteger(forKey: kMusicListLocation4)
			#if os(iOS)
				if let aName = aDecoder.decodeObject(forKey: kMusicListName4) as? String {
					name = aName
				}
			#endif
			for book in BookmarkArray {
				if !book.checkIsReachableAndReturnError(error: nil) {
					if selectedMusic == -1 {
						//Do nothing
					} else if selectedMusic == musicList.count + 1 {
						selectedMusic = -1
					} else if selectedMusic > musicList.count + 1 {
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
			let currentDate = Date()
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
				let obj = MusicListObject(url: bookURL as URL, date: currentDate)
				musicList.append(obj)
			}
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey2) as? [Data] {
			if let curSel = aDecoder.decodeObject(forKey: kMusicListLocation2) as? Int {
				selectedMusic = curSel
			} else {
				selectedMusic = -1
			}
			// Have all the new MusicListObjects use the same date
			let currentDate = Date()
			for bookData in bookmarkArray {
				do {
					let fullURL = try MusicListObject(bookmarkData: bookData, resolutionOptions: .withoutUI, relativeURL: homeURL, date: currentDate)
					musicList.append(fullURL)
				} catch {
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
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey1) as? [Data] {
			// Have all the new MusicListObjects use the same date
			let currentDate = Date()
			for bookData in bookmarkArray {
				do {
					let fullURL = try MusicListObject(bookmarkData: bookData, resolutionOptions: .withoutUI, date: currentDate)
					musicList.append(fullURL)
				} catch {
					lostMusicCount += 1
				}
			}
			selectedMusic = -1;
		} else {
			return nil
		}
		
		super.init()
	}

	func encode(with aCoder: NSCoder) {
		aCoder.encode(selectedMusic, forKey: kMusicListLocation4)
		aCoder.encode(musicList, forKey: kMusicListKey4)
		#if os(iOS)
			aCoder.encode(name, forKey: kMusicListName4)
		#endif
	}
	
	static let supportsSecureCoding = true
	
	//MARK: -
	@objc(URLAtIndex:) func url(at index: Int) -> URL {
		assert(index < musicList.count)
		return musicList[index].musicURL
	}
	
	// MARK: - saving/loading
	private func loadMusicList(_ newArray: [MusicListObject]) {
		willChangeValue(forKey: kMusicListKVO)
		musicList = newArray
		didChangeValue(forKey: kMusicListKVO)
	}
	
	private func loadMusicList(from theData: Data) -> Bool {
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
	
	@discardableResult
	func loadMusicList(from fromURL: URL) -> Bool {
		if let unWrappedListData = try? Data(contentsOf: fromURL) {
			return loadMusicList(from: unWrappedListData)
		} else {
			return false
		}
	}
	
	@discardableResult
	func loadApplicationMusicList() -> Bool {
		let manager = FileManager.default
		#if os(OSX)
			let musListDefName = "PlayerPRO Music List"
			let defaults = UserDefaults.standard
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
		if !((try? PPPPath.checkResourceIsReachable()) ?? false) {
			do {
				try manager.createDirectory(at: PPPPath, withIntermediateDirectories: true, attributes: nil)
			} catch _ {
				return false
			}
			
		}
		return loadMusicList(from: PPPPath.appendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	@discardableResult
	@objc(saveMusicListToURL:)
	func saveMusicList(to URL: Foundation.URL) -> Bool {
		let theList = NSKeyedArchiver.archivedData(withRootObject: self)
		do {
			try theList.write(to: URL, options: [])
			return true
		} catch _ {
			return false
		}
	}
	
	@discardableResult
	func saveApplicationMusicList() -> Bool {
		let manager = FileManager.default
		
		if !((try? PPPPath.checkResourceIsReachable()) ?? false) {
			do {
				//Just making sure...
				try manager.createDirectory(at: PPPPath, withIntermediateDirectories: true, attributes:nil)
			} catch _ {
				return false
			}
		}
		
		return self.saveMusicList(to: PPPPath.appendingPathComponent(kPlayerList, isDirectory: false))
	}
	
	// MARK: - Key-valued Coding
	@objc(addMusicListObject:)
	func add(_ object: MusicListObject) {
		if !musicList.contains(object) {
			musicList.append(object)
		}
	}
	
	var countOfMusicList: Int {
		return musicList.count
	}
	
	@objc(replaceObjectInMusicListAtIndex:withObject:)
	func replaceObjectInMusicList(at index: Int, with object: MusicListObject) {
		musicList[index] = object;
	}
	
	@objc(objectInMusicListAtIndex:)
	func objectInMusicList(at index: Int) -> MusicListObject {
		return musicList[index];
	}
	
	@objc(removeMusicListAtIndexes:)
	func remove(at idxSet: IndexSet) {
		if idxSet.contains(selectedMusic) {
			selectedMusic = -1;
		}
		
		musicList.remove(indexes: idxSet)
	}
	
	@objc(removeObjectInMusicListAtIndex:)
	func remove(at atIndex: Int) {
		if atIndex == selectedMusic {
			selectedMusic = -1
		} else if selectedMusic > atIndex {
			selectedMusic -= 1
		}
		musicList.remove(at: atIndex)
	}
	
	@objc(insertObject:inMusicListAtIndex:)
	func insert(_ object: MusicListObject, at index: Int) {
		musicList.insert(object, at: index)
	}
	
	@objc(arrayOfObjectsInMusicListAtIndexes:)
	func objectsInMusicList(at theSet : IndexSet) -> [MusicListObject] {
		return musicList.filter({ (include) -> Bool in
			let idx = self.musicList.index(of: include)!
			return theSet.contains(idx)
		})
	}
	
	@objc(insertMusicLists:atIndexes:)
	func insertMusicLists(_ anObj: [MusicListObject], at indexes: IndexSet) {
		
		for (i, idx) in zip(indexes, anObj) {
			musicList.insert(idx, at: i)
		}
	}
	
	@objc(insertObjects:inMusicListAtIndexes:)
	func insert(_ objs: [MusicListObject], at indexes: IndexSet) {
		insertMusicLists(objs, at: indexes)
	}
	
	@objc(removeObjectsInMusicListAtIndexes:)
	func removeObjectsInMusicListAtIndexes(_ idxSet: IndexSet) {
		remove(at: idxSet)
	}
	
	@objc(insertObjects:inMusicListAtIndex:)
	func insert(_ anObj: [MusicListObject], at idx: Int) {
		let theIndexSet = IndexSet(integersIn: idx ..< anObj.count + idx)
		self.willChange(.insertion, valuesAt: theIndexSet, forKey: kMusicListKVO)
		
		insertMusicLists(anObj, at: theIndexSet)
		
		self.didChange(.insertion, valuesAt: theIndexSet, forKey: kMusicListKVO)
	}
	
	#if os(OSX)
	func beginLoadingOfOldMusicList(at toOpen: URL, completionHandle theHandle: @escaping (_ theErr: Error?) -> Void) {
		let modDate: Date? = {
			do {
				var values = try toOpen.resourceValues(forKeys: [.contentModificationDateKey])
				return values.contentModificationDate
			} catch _ {
				return nil
			}
		}()
		
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(with: PPSTImporterHelper.self)
		
		conn.resume()
		
		(conn.remoteObjectProxy as! PPSTImporterHelper).loadStcf(at: toOpen, withReply: {(bookmarkData: [String : Any]?, error: Error?) -> Void in
			OperationQueue.main.addOperation({
				defer {
					conn.invalidate()
				}
				if let error = error {
					theHandle(error)
					return
				} else {
					guard let invalidAny = bookmarkData?["lostMusicCount"] as? UInt,
						let selectedAny = bookmarkData?["SelectedMusic"] as? Int,
						let pathsAny = bookmarkData?["MusicPaths"] as? NSArray as? [String] else {
							let lolwut = NSError(domain: NSCocoaErrorDomain, code: NSXPCConnectionReplyInvalid, userInfo: [NSLocalizedDescriptionKey: NSLocalizedString("Invalid data returned from helper", comment: "Invalid data returned from helper")])
							theHandle(lolwut)
							return
					}
					// Have all the new MusicListObjects use the same date
					let currentDate = modDate ?? Date()
					let pathsURL = pathsAny.map({ (aPath) -> MusicListObject in
						let tmpURL = URL(fileURLWithPath: aPath)
						let tmpObj = MusicListObject(url: tmpURL, date: currentDate)
						return tmpObj
					})
					self.loadMusicList(pathsURL)
					self.lostMusicCount = invalidAny
					self.selectedMusic = selectedAny
					
					theHandle(nil)
				}
			})
		})
	}
	#endif
}
