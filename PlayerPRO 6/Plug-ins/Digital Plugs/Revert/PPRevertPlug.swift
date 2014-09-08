//
//  File.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

import Foundation
import PlayerPROKit

@objc(PPRevertPlug) class RevertPlug : NSObject, PPDigitalPlugin {
	var hasUIConfiguration: Bool {
		get {
			return false
		}
	}
	
	func runWithPcmd(myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver!) -> MADErr {
		let srcCmd = PPKPcmd(myPcmd)
		var track: Int16 = 0
		var row: Int16 = 0
		let maxLenth = myPcmd.memory.length - 1
		
		for track in 0 ..< myPcmd.memory.tracks {
			for row in 0 ... maxLenth {
				var myCmdsrc	= srcCmd.getCommand(maxLenth - row, track: track) 
				ReplaceCmd(row, track, myCmdsrc, myPcmd)
			}
		}
		
		return .NoErr
	}
}
