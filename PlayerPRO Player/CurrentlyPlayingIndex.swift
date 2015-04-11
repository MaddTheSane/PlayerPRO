//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

final class CurrentlyPlayingIndex: DebugPrintable, Printable {
	var index = -1
	var playbackURL: NSURL? = nil
	func movePlayingIndexToOtherIndex(othIdx: CurrentlyPlayingIndex) {
		othIdx.index = index
		othIdx.playbackURL = playbackURL
	}
	
	var description: String {
		return "Index: \(index) URL: \(playbackURL!.relativePath!)"
	}
	
	var debugDescription: String {
		return "Index: \(index) URL: \(playbackURL) URL Path: \(playbackURL!.path)"
	}
}
