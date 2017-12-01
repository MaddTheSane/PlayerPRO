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
		if (try? PPMusicBase.checkResourceIsReachable()) ?? false {
			try FileManager.default.createDirectory(at: PPMusicBase, withIntermediateDirectories: true, attributes: nil)
		}
		let dat = try JSONEncoder().encode(self)
		try dat.write(to: PPMusicLib)
	}
	
	
	// MARK: - codable
	required init(from decoder: Decoder) throws {
		let values = try decoder.container(keyedBy: CodingKeys.self)
		allMusicObjects = try values.decode(Array<MusicListObject>.self, forKey: .allMusicObjects)
		allLists = try values.decode(Array<MusicList>.self, forKey: .allLists)
		super.init()
		for list in allLists {
			list.resolveObjects(against: self)
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
		
		try container.encode(allMusicObjects, forKey: .allMusicObjects)
		try container.encode(allLists, forKey: .allLists)
	}
}
