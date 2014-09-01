//
//  PPKPcmd.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/27/14.
//
//

import Foundation
import PlayerPROCore

public func GetCommand(row: Int16, track: Int16, aPcmd: PPKPcmd) -> Cmd {
	return aPcmd.getCommand(row, track: track)
}

public func ReplaceCmd(row1: Int16, track1: Int16, command: Cmd, inout aPat: PPKPcmd) {
	aPat.replaceCmd(atRow: row1, track: track1, command: command)
}

public struct PPKPcmd: SequenceType {
	public var tracks: Int16
	public var length: Int16
	public var trackStart: Int16
	public var posStart: Int16
	public var myCmd = [Cmd]()
	public var structSize: Int32? { get {
		if self.valid {
			return Int32(sizeof(Pcmd.Type) + myCmd.count * sizeof(Cmd.Type))
		} else {
			return nil
		}
		}}
	
	public func generate() -> IndexingGenerator<[Cmd]> {
		return myCmd.generate()
	}
	
	public var valid: Bool { get {
		return Int(tracks * length) == myCmd.count
		}}
	
	public init() {
		tracks = 1
		trackStart = 0
		length = 1
		posStart = 0
		myCmd.append(Cmd())
	}
	
	public init(_ aPcmd: UnsafePointer<Pcmd>) {
		let unwrapped = aPcmd.memory
		let myCmdPos: Int = Int(unwrapped.structSize) - sizeof(Pcmd.Type)
		tracks = unwrapped.tracks
		length = unwrapped.length
		trackStart = unwrapped.trackStart
		posStart = unwrapped.posStart
		let theirCmdPtr: UnsafePointer<Cmd> = UnsafePointer<Cmd>(aPcmd.advancedBy(sizeof(Pcmd.Type)))
		for i in 0 ..< myCmdPos / sizeof(Cmd.Type) {
			myCmd.append(theirCmdPtr[i])
		}
	}
	public mutating func modifyCmdAtRow(row: Int, track: Int, commandBlock: (inout Cmd)-> ()) {
		modifyCmdAtRow(Int16(row), track: Int16(track), commandBlock)
	}
	
	public mutating func modifyCmdAtRow(row1: Int16, track track1: Int16, commandBlock: (inout Cmd)-> ()) {
		var track = track1
		var row = row1
		if (row < 0) {
			row = 0;
		} else if (row >= length) {
			row = length - 1;
		}
		
		if (track < 0) {
			track = 0;
		} else if (track >= tracks) {
			track = tracks - 1;
		}
		
		commandBlock(&myCmd[Int((length * track) + row)])

	}
	
	public mutating func addCmd(command: Cmd) {
		myCmd.append(command)
	}
	
	public mutating func replaceCmd(atRow row1: Int16, track: Int16, command: Cmd) {
		replaceCmdAtRow(row1, track: track, command: command)
	}
	
	public mutating func replaceCmdAtRow(row1: Int, track track1: Int, command: Cmd) {
		replaceCmdAtRow(Int16(row1), track: Int16(track1), command: command)
	}
	
	public mutating func replaceCmdAtRow(row1: Int16, track track1: Int16, command: Cmd) {
		var track = track1
		var row = row1
		if (row < 0) {
			row = 0;
		} else if (row >= length) {
			row = length - 1;
		}
		
		if (track < 0) {
			track = 0;
		} else if (track >= tracks) {
			track = tracks - 1;
		}
		
		myCmd[Int((length * track) + row)] = command
	}
	
	public func getCommand(row1: Int, track track1: Int) -> Cmd {
		return getCommand(Int16(row1), track: Int16(track1))
	}
	
	public func getCommand(row1: Int16, track track1: Int16) -> Cmd {
		var track = track1
		var row = row1
		if (row < 0) {
			row = 0;
		} else if (row >= length) {
			row = length - 1;
		}
		
		if (track < 0) {
			track = 0;
		} else if (track >= tracks) {
			track = tracks - 1;
		}
		
		return myCmd[Int((length * track) + row)]
	}
}
