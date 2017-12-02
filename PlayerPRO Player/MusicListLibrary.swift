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
	return PPMusicBase.appendingPathComponent("PlayerPRO Library.pplib")
}()

class MusicListLibrary: NSObject {
	/// Basic wrapper around Music List Objects, so we don't encode the same MusicList twice.
	private struct ListWrapper: Codable {
		var name: String
		var selectedIndex: Int
		var musicUUIDs: [UUID]
		
		init(musicList: MusicList) {
			name = musicList.name
			selectedIndex = musicList.selectedMusic
			musicUUIDs = musicList.musicList.map({$0.uuid})
		}
	}
	
	var allMusicObjects: [MusicListObject]
	var allLists: [MusicList]

	override init() {
		allMusicObjects = [MusicListObject]()
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
	func loadOldLibrary(at url: URL) throws -> MusicList {
		let data = try Data(contentsOf: url)
		guard let list = NSKeyedUnarchiver.unarchiveObject(with: data) as? MusicList else {
			throw NSError(domain: NSCocoaErrorDomain, code: NSFileReadCorruptFileError, userInfo: nil)
		}
		list.resolveObjects(against: self)
		if let props = try? url.resourceValues(forKeys: [URLResourceKey.localizedNameKey]),
			let locName = props.localizedName {
			list.name = locName
		} else {
			list.name = NSLocalizedString("Untitled Imported List", comment: "An imported list that the name couldn't be found")
		}
		allLists.append(list)
		return list
	}
	
	#if os(OSX)
	func migrateOldLibrary() throws {
		let oldPath = PPPPath.appendingPathComponent(kPlayerList, isDirectory: false)
		let oldList = try loadOldLibrary(at: oldPath)
		//try FileManager.default.removeItem(at: oldPath)
		oldList.name = "Migrated Music List"
	}
	#endif
	
	// MARK: - codable
	required init(from decoder: Decoder) throws {
		let values = try decoder.container(keyedBy: CodingKeys.self)
		
		let allMusicObjects = try values.decode(Array<MusicListObject>.self, forKey: .allMusicObjects)
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
			
			//let badIdxs
			
			for (i, mus) in musics.enumerated() {
				if let mus = mus {
					newList.add(mus)
				} else {
					if selectedMusic == -1 {
						//Do nothing
					} else if selectedMusic == i + 1 {
						selectedMusic = -1
					} else if selectedMusic > i + 1 {
						selectedMusic -= 1
					}
				}
			}
			
			newList.selectedMusic = selectedMusic
			
			return newList
		}
		super.init()
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
