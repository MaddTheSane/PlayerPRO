//
//  PPCurrentlyPlayingIndex.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/25/14.
//
//

import Foundation

class PPCurrentlyPlayingIndex: NSObject, DebugPrintable, Printable {
	internal var index:Int = -1
	internal var playbackURL: NSURL? = nil
	func movePlayingIndexToOtherIndex(othIdx: PPCurrentlyPlayingIndex) {
		othIdx.index = index
		othIdx.playbackURL = playbackURL
	}
	
	override init() {
		super.init()
	}
	
	override var description: String { get {
		return "Index: \(index) URL: \(playbackURL)"
	}}
	
	override var debugDescription: String { get {
		return "Index: \(index) URL: \(playbackURL) URL Path: \(playbackURL!.path)"
	}}
}
