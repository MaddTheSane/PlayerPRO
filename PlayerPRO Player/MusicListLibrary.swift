//
//  MusicListLibrary.swift
//  PlayerPRO Player
//
//  Created by C.W. Betts on 12/1/17.
//

import Foundation

class MusicListLibrary: NSObject {
	var allMusicObjects: [MusicListObject]
	var allLists: [MusicList]

	override init() {
		allMusicObjects = [MusicListObject]()
		allLists = [MusicList]()
		super.init()
	}
	
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
