//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

class PPCurrentlyPlayingIndex: NSObject, DebugPrintable {
	public var index:Int = -1
	public var playbackURL: NSURL? = nil
	func movePlayingIndexToOtherIndex(othIdx: PPCurrentlyPlayingIndex) {
		othIdx.index = index
		othIdx.playbackURL = playbackURL
	}
	
	override var description: String {
	get {
		return self.debugDescription
	}
	}
	
	override var debugDescription: String { get {
		return "Index: \(index) URL: \(playbackURL) URL Path: \(playbackURL!.path)"
	}}
}
