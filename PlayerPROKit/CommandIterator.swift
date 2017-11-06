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
	func getCommand(row: Int16, track: Int16) -> Cmd
	mutating func modifyCommand(row: Int16, track: Int16, commandBlock: (inout Cmd) -> ())
	mutating func replaceCommand(row: Int16, track: Int16, with command: Cmd)
	
	var commandLength: Int16 {get}
	var commandTracks: Int16 {get}
}

extension CommandIterator {
	public mutating func iterateCommands(commandBlock block: (_ command: inout Cmd, _ row: Int16, _ track: Int16) -> Bool) {
		for i in 0 ..< commandLength {
			for x in 0 ..< commandTracks {
				var aCmd = getCommand(row: i, track: x)
				if block(&aCmd, i, x) {
					replaceCommand(row: i, track: x, with: aCmd)
				}
			}
		}
	}
}

public func getCommand<X>(row: Int16, track: Int16, aIntPcmd: X) -> Cmd where X: CommandIterator {
	return aIntPcmd.getCommand(row: row, track: track)
}

public func replaceCommand<X>(row: Int16, track: Int16, command: Cmd, aPcmd: inout X) where X: CommandIterator {
	aPcmd.replaceCommand(row: row, track: track, with: command)
}

public func modifyCommand<X>(row: Int16, track: Int16, aPcmd: inout X, commandBlock: (inout Cmd) -> ()) where X: CommandIterator {
	aPcmd.modifyCommand(row: row, track: track, commandBlock: commandBlock)
}

public func iterateCommands<X>( aPcmd: inout X, commandBlock block: (_ command: inout Cmd, _ row: Int16, _ track: Int16) -> Bool) where X: CommandIterator {
	aPcmd.iterateCommands(commandBlock: block)
}
