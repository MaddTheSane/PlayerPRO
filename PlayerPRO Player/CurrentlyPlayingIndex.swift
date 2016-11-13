//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

struct CurrentlyPlayingIndex: CustomDebugStringConvertible, CustomStringConvertible {
	var index = -1
	var playbackURL: URL? = nil
	
	var description: String {
		return "Index: \(index) URL: \(playbackURL?.relativePath ?? "none")"
	}
	
	var debugDescription: String {
		let urlDes: String
		if let playURL = playbackURL {
			urlDes = String(describing: playURL)
		} else {
			urlDes = "none"
		}
		
		return "Index: \(index) URL: '\(urlDes)' URL Path: \(playbackURL?.path ?? "none")"
	}
}
