//
//  File.swift
//  PPMacho
//
//  Created by C.W. Betts on 9/6/14.
//
//

import Foundation
import PlayerPROKit

public final class Revert: NSObject, PPDigitalPlugin {
	public let hasUIConfiguration = false
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}

	public func runWithPcmd(myPcmd: UnsafeMutablePointer<Pcmd>, driver: PPDriver) -> MADErr {
		let srcCmd = PPKPcmd(myPcmd)
		let maxLenth = srcCmd.length - 1
		
		for track in 0 ..< srcCmd.tracks {
			for row in 0 ... maxLenth {
				let myCmdsrc = getCommand(row: maxLenth - row, track: track, aIntPcmd: srcCmd)
				replaceCommand(row: row, track: track, command: myCmdsrc, aPcmd: myPcmd)
			}
		}
		
		return .NoErr
	}
}
