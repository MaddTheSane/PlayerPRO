//
//  MusicListLibrary.swift
//  PlayerPRO Player
//
//  Created by C.W. Betts on 12/1/17.
//

import Foundation

private let PPMusicBase: URL = {
	var toRet = try! FileManager.default.url(for: .musicDirectory, in: .userDomainMask, appropriateFor: nil, create: false)
	toRet.appendPathComponent("PlayerPRO", isDirectory: true)
	return toRet
}()

private let PPMusicLib: URL = {
	return PPMusicBase.appendingPathComponent("PlayerPRO Library.pplib", isDirectory: false)
}()

class MusicListLibrary: NSObject {
	/// Basic wrapper around Music List Objects, so we don't encode the same MusicList twice.
	private struct ListWrapper: Codable {
		enum CodingKeys: String, CodingKey {
			case name
			case selectedIndex = "selected_index"
			case musicUUIDs = "music_uuids"
		}

		var name: String
		var selectedIndex: Int
		var musicUUIDs: [UUID]
		
		init(musicList: MusicList) {
			name = musicList.name
			selectedIndex = musicList.selectedMusic
			musicUUIDs = musicList.musicList.map({$0.uuid})
		}
	}
	
	private(set) var allMusicObjects: Set<MusicListObject>
	private(set) var allLists: [MusicList]

	override init() {
		allMusicObjects = Set<MusicListObject>()
		allLists = [MusicList]()
		super.init()
	}
	
	class func load() throws -> MusicListLibrary {
		let data = try Data(contentsOf: PPMusicLib)
		
		return try JSONDecoder().decode(MusicListLibrary.self, from: data)
	}
	
	func save() throws {
		if !((try? PPMusicBase.checkResourceIsReachable()) ?? false) {
			try FileManager.default.createDirectory(at: PPMusicBase, withIntermediateDirectories: true, attributes: nil)
		}
		let dat = try JSONEncoder().encode(self)
		try dat.write(to: PPMusicLib)
	}
	
	@discardableResult
	func loadList(at url: URL) throws -> MusicList {
		let list = try MusicList.from(contentsOf: url)
		add(list: list)
		return list
	}
	
	#if os(OSX)
	func migrateOldLibrary() throws {
		let oldPath = PPPPath.appendingPathComponent(kPlayerList, isDirectory: false)
		let oldList = try loadList(at: oldPath)
		//try FileManager.default.removeItem(at: oldPath)
		oldList.name = "Migrated Music List"
	}
	#endif
	
	func add(list: MusicList) {
		list.resolveObjects(against: self)
		list.delegate = self
		allLists.append(list)
	}
	
	func add(music object: MusicListObject) {
		if !allMusicObjects.contains(object) {
			allMusicObjects.insert(object)
		}
	}
	
	// MARK: - codable
	required init(from decoder: Decoder) throws {
		let values = try decoder.container(keyedBy: CodingKeys.self)
		
		let allMusicObjects = try values.decode(Set<MusicListObject>.self, forKey: .allMusicObjects)
		let convLists = try values.decode(Array<ListWrapper>.self, forKey: .allLists)
		self.allMusicObjects = allMusicObjects
		allLists = convLists.map { (wrapped) -> MusicList in
			let newList = MusicList()
			newList.name = wrapped.name
			var selectedMusic = wrapped.selectedIndex
			
			let musics = wrapped.musicUUIDs.map({ (uuid) -> MusicListObject? in
				allMusicObjects.first(where: { (mlo) -> Bool in
					mlo.uuid == uuid
				})
			})
			
			var badIdxs = IndexSet()
			
			for (i, mus) in musics.enumerated() {
				if let mus = mus {
					newList.add(mus)
				} else {
					badIdxs.insert(i)
				}
			}
			
			if badIdxs.count != 0 {
				if badIdxs.contains(selectedMusic) {
					selectedMusic = -1
				} else if selectedMusic != -1 {
					selectedMusic -= badIdxs.count(in: 0 ..< selectedMusic)
				}
			}
			
			newList.selectedMusic = selectedMusic
			
			return newList
		}
		super.init()
		
		allLists.forEach { (ml) in
			ml.delegate = self
		}
	}
}

extension MusicListLibrary: Codable {
	enum CodingKeys: String, CodingKey {
		case allMusicObjects = "all_music_objects"
		case allLists = "all_lists"
	}
	
	func encode(to encoder: Encoder) throws {
		var container = encoder.container(keyedBy: CodingKeys.self)
		
		let convList = allLists.map({ListWrapper(musicList: $0)})
		
		try container.encode(allMusicObjects, forKey: .allMusicObjects)
		try container.encode(convList, forKey: .allLists)
	}
}

extension MusicListLibrary: MusicListDelegate {
	func musicList(_ list: MusicList, willAdd object: MusicListObject) -> MusicListObject {
		let newUUID = object.uuid
		let newURL = object.musicURL
		if let aObj = allMusicObjects.first(where: { (mlo) -> Bool in
			mlo.uuid == newUUID
		}) {
			return aObj
		} else if let aObj = allMusicObjects.first(where: { (mlo) -> Bool in
			return URLsPointingToTheSameFile(mlo.musicURL, newURL)
		}) {
			return aObj
		}

		return object
	}
	
	func musicList(_ list: MusicList, didAdd object: MusicListObject) {
		if !allMusicObjects.contains(object) {
			allMusicObjects.insert(object)
		}
	}
	
	func musicList(_ list: MusicList, willRemove object: MusicListObject) {
		// Nothing for now
	}
}

// KVO/KVC for allLists
extension MusicListLibrary {
	
	@objc var countOfAllLists: Int {
		return allLists.count
	}

	@objc(objectInAllListsAtIndex:)
	func objectInAllLists(at index: Int) -> MusicList {
		return allLists[index]
	}

	//valueforall
}
