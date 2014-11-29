//
//  CommandIterator.swift
//  PPMacho
//
//  Created by C.W. Betts on 11/5/14.
//
//

import Foundation
import PlayerPROCore

public protocol CommandIterator {
	func getCommand(#row: Int16, track: Int16) -> Cmd
	mutating func modifyCommand(#row: Int16, track: Int16, commandBlock: (inout Cmd) -> ())
	mutating func replaceCommand(#row: Int16, track: Int16, command: Cmd)
	
	var commandLength: Int16 {get}
	var commandTracks: Int16 {get}
}

public func GetCommand<X where X: CommandIterator>(#row: Int16, #track: Int16, aIntPcmd: X) -> Cmd {
	return aIntPcmd.getCommand(row: row, track: track)
}

public func ReplaceCommand<X where X: CommandIterator>(#row: Int16, #track: Int16, #command: Cmd, inout #aPcmd: X) {
	aPcmd.replaceCommand(row: row, track: track, command: command)
}

public func ModifyCommand<X where X: CommandIterator>(#row: Int16, #track: Int16, inout #aPcmd: X, #commandBlock: (inout Cmd) -> ()) {
	aPcmd.modifyCommand(row: row, track: track, commandBlock: commandBlock)
}

public func IterateCommands<X where X: CommandIterator>(inout aPcmd: X, commandBlock block: (inout command: Cmd, row: Int16, track: Int16) -> Bool) {
	for i in 0 ..< aPcmd.commandLength {
		for x in 0 ..< aPcmd.commandTracks {
			var aCmd = aPcmd.getCommand(row: i, track: x)
			if block(command: &aCmd, row: i, track: x) {
				aPcmd.replaceCommand(row: i, track: x, command: aCmd)
			}
		}
	}
}
