//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

struct CurrentlyPlayingIndex: DebugPrintable, Printable {
	var index = -1
	var playbackURL: NSURL? = nil
	
	var description: String {
		return "Index: \(index) URL: \(playbackURL!.relativePath!)"
	}
	
	var debugDescription: String {
		return "Index: \(index) URL: \(playbackURL) URL Path: \(playbackURL!.path)"
	}
}
