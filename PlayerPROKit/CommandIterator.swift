//
//  CommandIterator.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/5/14.
//
//

import Foundation
//import PlayerPROCore

public protocol CommandIterator {
	func getCommand(atRow: Int16, track: Int16) -> Cmd
	mutating func modifyCommand(atRow: Int16, track: Int16, commandBlock: (inout Cmd) -> ())
	mutating func replaceCommand(atRow: Int16, track: Int16, with command: Cmd)
	
	var commandLength: Int16 {get}
	var tracks: Int16 {get}
}

extension CommandIterator {
	public mutating func iterateCommands(commandBlock block: (_ command: inout Cmd, _ row: Int16, _ track: Int16) -> Bool) {
		for i in 0 ..< commandLength {
			for x in 0 ..< tracks {
				var aCmd = getCommand(atRow: i, track: x)
				if block(&aCmd, i, x) {
					replaceCommand(atRow: i, track: x, with: aCmd)
				}
			}
		}
	}
}

public func getCommand<X>(atRow row: Int16, track: Int16, aIntPcmd: X) -> Cmd where X: CommandIterator {
	return aIntPcmd.getCommand(atRow: row, track: track)
}

public func replaceCommand<X>(atRow row: Int16, track: Int16, command: Cmd, aPcmd: inout X) where X: CommandIterator {
	aPcmd.replaceCommand(atRow: row, track: track, with: command)
}

public func modifyCommand<X>(atRow row: Int16, track: Int16, aPcmd: inout X, commandBlock: (inout Cmd) -> ()) where X: CommandIterator {
	aPcmd.modifyCommand(atRow: row, track: track, commandBlock: commandBlock)
}

public func iterateCommands<X>( aPcmd: inout X, commandBlock block: (_ command: inout Cmd, _ row: Int16, _ track: Int16) -> Bool) where X: CommandIterator {
	aPcmd.iterateCommands(commandBlock: block)
}
