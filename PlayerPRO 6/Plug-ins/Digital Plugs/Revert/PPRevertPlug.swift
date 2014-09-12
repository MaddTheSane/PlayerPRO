//
//  File.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

import Foundation
import PlayerPROKit

@objc(PPRevertPlug) public class RevertPlug : NSObject, PPDigitalPlugin {
	public let hasUIConfiguration = false
	
	public func runWithPcmd(myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver!) -> MADErr {
		let srcCmd = PPKPcmd(myPcmd)
		let maxLenth = srcCmd.length - 1
		
		for track in 0 ..< srcCmd.tracks {
			for row in 0 ... maxLenth {
				var myCmdsrc	= srcCmd.getCommand(maxLenth - row, track: track) 
				ReplaceCmd(row, track, myCmdsrc, myPcmd)
			}
		}
		
		return .NoErr
	}
}
