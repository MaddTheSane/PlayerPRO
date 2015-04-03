//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

final class CurrentlyPlayingIndex: NSObject, DebugPrintable, Printable {
	var index = -1
	var playbackURL: NSURL? = nil
	func movePlayingIndexToOtherIndex(othIdx: CurrentlyPlayingIndex) {
		othIdx.index = index
		othIdx.playbackURL = playbackURL
	}
	
	override var description: String {
		return "Index: \(index) URL: \(playbackURL!.relativePath!)"
	}
	
	override var debugDescription: String {
		return "Index: \(index) URL: \(playbackURL) URL Path: \(playbackURL!.path)"
	}
}
