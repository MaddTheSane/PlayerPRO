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
	func getCommand(row: Int16, track: Int16) -> Cmd
	mutating func modifyCmdAtRow(arow: Int16, track: Int16, commandBlock: (inout Cmd) -> ())
	mutating func replaceCmd(row: Int16, track: Int16, command: Cmd)
	
	var commandLength: Int16 {get}
	var commandTracks: Int16 {get}
}

public func GetCommand<X where X: CommandIterator>(row: Int16, track: Int16, aIntPcmd: X) -> Cmd {
	return aIntPcmd.getCommand(row, track: track)
}

public func ReplaceCmd<X where X: CommandIterator>(row: Int16, track: Int16, command: Cmd, inout aPcmd: X) {
	aPcmd.replaceCmd(row, track: track, command: command)
}

public func ModifyCmdAtRow<X where X: CommandIterator>(row: Int16, track: Int16, inout aPcmd: X, commandBlock: (inout Cmd) -> ()) {
	aPcmd.modifyCmdAtRow(row, track: track, commandBlock: commandBlock)
}

