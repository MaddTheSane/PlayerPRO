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

let kPlayerList = "Player List"
let kUntitledMusicList = "Untitled player LiST"

#if os(OSX)
	let PPPPath: URL = {
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
	let listExtension = "pplist"
	let PPPPath = (try! FileManager.default.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)).appendingPathComponent("Playlists", isDirectory: true)
#endif

protocol MusicListDelegate: class {
	func musicList(_ list: MusicList, willAdd object: MusicListObject) -> MusicListObject
	func musicList(_ list: MusicList, didAdd object: MusicListObject)
	func musicList(_ list: MusicList, willRemove object: MusicListObject)
}

@objc(PPMusicList) class MusicList: NSObject, NSSecureCoding, NSFastEnumeration, Collection {
	@objc private(set)	dynamic var musicList = [MusicListObject]()
	private(set)	var lostMusicCount: UInt
	@objc dynamic var		selectedMusic: Int
	@objc dynamic var		name = "New Music List"
	#if os(iOS)
	private(set)	var fileUUID = UUID()
	#endif
	weak var delegate: MusicListDelegate?
	
	func countByEnumerating(with state: UnsafeMutablePointer<NSFastEnumerationState>, objects buffer: AutoreleasingUnsafeMutablePointer<AnyObject?>, count len: Int) -> Int {
		return (musicList as NSArray).countByEnumerating(with: state, objects: buffer, count: len)
	}
	
	func makeIterator() -> IndexingIterator<[MusicListObject]> {
		return musicList.makeIterator()
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
	/// Use `indexOfObjectSimilar(to:)` on Swift instead.
	@objc(indexOfObjectSimilarToURL:)
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
	
	@objc(sortMusicListUsingBlock:)
	func sortMusicList(using block: (_ lhs: MusicListObject, _ rhs: MusicListObject) -> Bool) {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(by: block)
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingComparator:)
	func sortMusicList(using comparator: Comparator) {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(by: { (obj1, obj2) -> Bool in
			return comparator(obj1, obj2) == .orderedAscending
		})
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc func sortMusicListByName() {
		self.willChangeValue(forKey: kMusicListKVO)
		musicList.sort(by: { (var1, var2) -> Bool in
			let result = var1.fileName.localizedStandardCompare(var2.fileName)
			return result == .orderedAscending
			})
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	@objc(sortMusicListUsingDescriptors:)
	func sortMusicList(using descriptors: [NSSortDescriptor]) {
		self.willChangeValue(forKey: kMusicListKVO)
		let anArray = musicList.sorted(using: descriptors)
		musicList = anArray
		self.didChangeValue(forKey: kMusicListKVO)
	}
	
	enum AddMusicStatus {
		case failure
		case success
		case similarURL
	}
	
	func add(music theURL: URL?, force: Bool = false) -> AddMusicStatus {
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
		
		let theIndex = IndexSet(integer: musicList.count)
		self.willChange(.insertion, valuesAt: theIndex, forKey: kMusicListKVO)
		let obj2 = delegate?.musicList(self, willAdd: obj) ?? obj
		musicList.append(obj2)
		delegate?.musicList(self, didAdd: obj2)
		self.didChange(.insertion, valuesAt: theIndex, forKey: kMusicListKVO)
		return .success
	}
	
	override init() {
		lostMusicCount = 0
		selectedMusic = -1
		
		super.init()
	}
	
	#if os(iOS)
	class func availablePlaylistUUIDs() throws -> [UUID]! {
		let fm = FileManager.default
		let docDir = try fm.url(for: .documentDirectory, in: .userDomainMask, appropriateFor: nil, create: true)
		let playlistDir = docDir.appendingPathComponent("Playlists")
		try fm.createDirectory(at: playlistDir, withIntermediateDirectories: true, attributes: nil)
		let dirConts = try fm.contentsOfDirectory(at: playlistDir, includingPropertiesForKeys: nil, options: [])
		var toRetUUIDs = [UUID]()
		for url in dirConts {
			if let fileUUIDStr = (url.lastPathComponent as NSString?)?.deletingPathExtension,
				let fileUUID = UUID(uuidString: fileUUIDStr) {
				toRetUUIDs.append(fileUUID)
			}
		}
		return toRetUUIDs
	}
	
	func save() throws {
		let saveURL = PPPPath.appendingPathComponent(fileUUID.uuidString).appendingPathExtension(listExtension)
		try saveMusicList(to: saveURL)
	}
	#endif
	
	func resolveObjects(against: MusicListLibrary) {
		let oldList = musicList
		for (i, obj) in oldList.enumerated() {
			let allLists = against.allMusicObjects
			var wasFound = false
			for obj2 in allLists {
				if obj.uuid == obj2.uuid {
					musicList[i] = obj2
					wasFound = true
				}
				if URLsPointingToTheSameFile(obj.musicURL, obj2.musicURL) {
					musicList[i] = obj2
					wasFound = true
				}
			}
			if !wasFound {
				against.add(music: obj)
			}
		}
	}
	
	// MARK: - NSCoding
	
	required init?(coder aDecoder: NSCoder) {
		lostMusicCount = 0
		if let BookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey4) as? [MusicListObject] {
			selectedMusic = aDecoder.decodeInteger(forKey: kMusicListLocation4)
			if let aName = aDecoder.decodeObject(forKey: kMusicListName4) as? String {
				name = aName
			} else {
				name = kUntitledMusicList
			}
			for book in BookmarkArray {
				if !((try? book.checkIsReachable()) ?? false) {
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
			name = kUntitledMusicList
			for bookURL in bookmarkArray {
				if !bookURL.checkResourceIsReachableAndReturnError(nil) {
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
				let obj = MusicListObject(url: bookURL as URL, date: currentDate)
				musicList.append(obj)
			}
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey2) as? [Data] {
			name = kUntitledMusicList
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
					if selectedMusic == -1 {
						//Do nothing
					} else if selectedMusic == musicList.count + 1 {
						selectedMusic = -1
					} else if selectedMusic > musicList.count + 1 {
						selectedMusic -= 1
					}
					lostMusicCount += 1
				}
			}
		} else if let bookmarkArray = aDecoder.decodeObject(forKey: kMusicListKey1) as? [Data] {
			name = kUntitledMusicList
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
			selectedMusic = -1
		} else {
			return nil
		}
		
		super.init()
	}

	func encode(with aCoder: NSCoder) {
		aCoder.encode(selectedMusic, forKey: kMusicListLocation4)
		aCoder.encode(musicList, forKey: kMusicListKey4)
		aCoder.encode(name, forKey: kMusicListName4)
	}
	
	static var supportsSecureCoding: Bool {
		return true
	}
	
	//MARK: -
	@objc(URLAtIndex:) func url(at index: Int) -> URL {
		assert(index < musicList.count)
		return musicList[index].musicURL
	}
	
	// MARK: - saving/loading
	static func from(contentsOf url: URL) throws -> MusicList {
		let wrappedData = try Data(contentsOf: url)
		let keyedUnarc = NSKeyedUnarchiver(forReadingWith: wrappedData)
		guard let newList = MusicList(coder: keyedUnarc) else {
			throw CocoaError(.fileReadCorruptFile, userInfo: [NSURLErrorKey: url])
		}
		if newList.name == kUntitledMusicList,
			let values = try? url.resourceValues(forKeys: [URLResourceKey.localizedNameKey]),
			let fileName = values.localizedName {
			newList.name = fileName
		}
		return newList
	}
	
	@discardableResult
	func loadApplicationMusicList() -> Bool {
		let musListDefName = "PlayerPRO Music List"
		UserDefaults.standard.removeObject(forKey: musListDefName)
		return false
	}
	
	func saveMusicList(to URL: Foundation.URL) throws {
		let theList = NSKeyedArchiver.archivedData(withRootObject: self)
		try theList.write(to: URL, options: [])
	}
	
	// MARK: - Key-valued Coding
	@objc(addMusicListObject:)
	func add(_ object: MusicListObject) {
		if !musicList.contains(object) {
			let newObj = delegate?.musicList(self, willAdd: object) ?? object
			musicList.append(newObj)
			delegate?.musicList(self, didAdd: newObj)
		}
	}
	
	@objc
	var countOfMusicList: Int {
		return musicList.count
	}
	
	@objc(replaceObjectInMusicListAtIndex:withObject:)
	func replaceObjectInMusicList(at index: Int, with object: MusicListObject) {
		let newObj = delegate?.musicList(self, willAdd: object) ?? object
		musicList[index] = newObj
		delegate?.musicList(self, didAdd: newObj)
	}
	
	@objc(objectInMusicListAtIndex:)
	func objectInMusicList(at index: Int) -> MusicListObject {
		return musicList[index]
	}
	
	@objc(removeMusicListAtIndexes:)
	func remove(at idxSet: IndexSet) {
		if idxSet.contains(selectedMusic) {
			selectedMusic = -1
		} else {
			selectedMusic -= idxSet.count(in: 0..<selectedMusic)
		}
		
		//delegate?.musicList(self, willRemove: object)
		musicList.remove(indexes: idxSet)
	}
	
	@objc(removeObjectInMusicListAtIndex:)
	func remove(at atIndex: Int) {
		if atIndex == selectedMusic {
			selectedMusic = -1
		} else if selectedMusic > atIndex {
			selectedMusic -= 1
		}
		delegate?.musicList(self, willRemove: musicList[atIndex])
		musicList.remove(at: atIndex)
	}
	
	@objc(insertObject:inMusicListAtIndex:)
	func insert(_ object: MusicListObject, at index: Int) {
		let newObj = delegate?.musicList(self, willAdd: object) ?? object
		musicList.insert(newObj, at: index)
		delegate?.musicList(self, didAdd: newObj)
	}
	
	@objc(arrayOfObjectsInMusicListAtIndexes:)
	func objectsInMusicList(at theSet : IndexSet) -> [MusicListObject] {
		var toRet = [MusicListObject]()
		for (i, obj) in musicList.enumerated() {
			if theSet.contains(i) {
				toRet.append(obj)
			}
		}
		return toRet
	}
	
	@objc(insertMusicLists:atIndexes:)
	func insertMusicLists(_ anObj: [MusicListObject], at indexes: IndexSet) {
		
		for (i, idx) in zip(indexes, anObj).reversed() {
			let newObj = delegate?.musicList(self, willAdd: idx) ?? idx
			musicList.insert(newObj, at: i)
			delegate?.musicList(self, didAdd: newObj)
		}
	}
	
	@objc(insertObjects:inMusicListAtIndexes:)
	func insert(_ objs: [MusicListObject], at indexes: IndexSet) {
		insertMusicLists(objs, at: indexes)
	}
	
	@objc(removeObjectsInMusicListAtIndexes:)
	func removeObjectsInMusicList(at idxSet: IndexSet) {
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
	static func beginLoadingOfOldMusicList(at toOpen: URL, completionHandle theHandle: @escaping (_ newList: MusicList?, _ theErr: Error?) -> Void) {
		let (modDate, fileName): (Date?, String?) = {
			do {
				var values = try toOpen.resourceValues(forKeys: [.contentModificationDateKey, .localizedNameKey])
				return (values.contentModificationDate, values.localizedName)
			} catch _ {
				return (nil, nil)
			}
		}()
		
		let conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(with: PPSTImporterHelper.self)
		
		conn.resume()
		
		(conn.remoteObjectProxyWithErrorHandler({ (err) in
			theHandle(nil, err)
		}) as! PPSTImporterHelper).loadStcf(at: toOpen, withReply: {(bookmarkData: [PPSTKeys : Any]?, error: Error?) -> Void in
			OperationQueue.main.addOperation({
				defer {
					conn.invalidate()
				}
				if let error = error {
					theHandle(nil, error)
					return
				} else {
					guard let invalidAny = bookmarkData?[.lostCount] as? UInt,
						let selectedAny = bookmarkData?[.selected] as? Int,
						let pathsAny = bookmarkData?[.urls] as? NSArray as? [URL] else {
							let lolwut = CocoaError(.xpcConnectionReplyInvalid, userInfo:
								[NSLocalizedDescriptionKey: NSLocalizedString("Invalid data returned from helper", comment: "Invalid data returned from helper"),
								 NSURLErrorKey: toOpen])
							theHandle(nil, lolwut)
							return
					}
					// Have all the new MusicListObjects use the same date
					let currentDate = modDate ?? Date()
					let pathsURL = pathsAny.map({ (aPath) -> MusicListObject in
						let tmpObj = MusicListObject(url: aPath, date: currentDate)
						return tmpObj
					})
					let newList = MusicList()
					newList.musicList = pathsURL
					newList.lostMusicCount = invalidAny
					newList.selectedMusic = selectedAny
					newList.name = fileName ?? toOpen.lastPathComponent
					
					theHandle(newList, nil)
				}
			})
		})
	}
	#endif
}
