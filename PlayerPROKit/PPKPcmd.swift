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

public func ReplaceCmd(row: Int16, track: Int16, command: Cmd, inout aPat: PPKPcmd) {
	aPat.replaceCmdAtRow(row, track: track, command: command)
}

public func ModifyCmdAtRow(row: Int16, track: Int16, inout aPcmd: PPKPcmd, commandBlock: (inout Cmd)-> ()) {
	aPcmd.modifyCmdAtRow(row, track: track, commandBlock: commandBlock)
}

public struct PPKPcmd: SequenceType {
	public var tracks: Int16
	public var length: Int16
	public var trackStart: Int16
	public var positionStart: Int16
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
	
	public func newPcmd() -> UnsafeMutablePointer<Pcmd>? {
		if let newSize = structSize {
			var aPcmd = UnsafeMutablePointer<Pcmd>(malloc(UInt(newSize)))
			aPcmd.memory.tracks = tracks
			aPcmd.memory.length = length
			aPcmd.memory.trackStart = trackStart
			aPcmd.memory.posStart = positionStart
			aPcmd.memory.structSize = newSize
			var theirCmdPtr = UnsafeMutablePointer<Cmd>(aPcmd.advancedBy(sizeof(Pcmd.Type)))
			for i in 0 ..< myCmd.count {
				theirCmdPtr[i] = myCmd[i]
			}
			return aPcmd
		} else {
			return nil
		}
	}
	
	public var valid: Bool {
		get {
			return Int(tracks * length) == myCmd.count
		}
	}
	
	public init() {
		tracks = 1
		trackStart = 0
		length = 1
		positionStart = 0
		myCmd.append(Cmd())
	}
	
	public init(tracks atr: Int16, startTrack: Int16 = 0, rows: Int16, startPosition: Int16 = 0) {
		tracks = atr
		trackStart = startTrack
		length = rows
		positionStart = startPosition
		for i in 0 ..< atr + rows {
			myCmd.append(Cmd())
		}
	}
	
	public init(_ aPcmd: UnsafePointer<Pcmd>) {
		let unwrapped = aPcmd.memory
		let myCmdPos: Int = Int(unwrapped.structSize) - sizeof(Pcmd.Type)
		tracks = unwrapped.tracks
		length = unwrapped.length
		trackStart = unwrapped.trackStart
		positionStart = unwrapped.posStart
		let theirCmdPtr = UnsafePointer<Cmd>(aPcmd.advancedBy(sizeof(Pcmd.Type)))
		for i in 0 ..< myCmdPos / sizeof(Cmd.Type) {
			myCmd.append(theirCmdPtr[i])
		}
	}
	
	public mutating func addRow() {
		length += 1
		for i in 0 ..< tracks {
			myCmd.append(Cmd())
		}
	}
	
	public mutating func addTrack() {
		for i in length - 1 ... 0 {
			let acmd = Cmd()
			myCmd.insert(Cmd(), atIndex: Int(i * tracks))
		}
		tracks += 1
	}
	
	public mutating func modifyCmdAtRow(row1: Int16, track track1: Int16, commandBlock: (inout Cmd)-> ()) {
		var track = track1
		var row = row1
		if (row1 < 0) {
			row = 0;
		} else if (row1 >= length) {
			row = length - 1;
		}
		
		if (track1 < 0) {
			track = 0;
		} else if (track1 >= tracks) {
			track = tracks - 1;
		}
		
		commandBlock(&myCmd[Int((length * track) + row)])
	}
	
	public mutating func addCmd(command: Cmd = Cmd()) {
		myCmd.append(command)
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
	
	public func getCommand(row1: Int16, track track1: Int16) -> Cmd {
		var track = track1
		var row = row1
		if (row1 < 0) {
			row = 0;
		} else if (row1 >= length) {
			row = length - 1;
		}
		
		if (track1 < 0) {
			track = 0;
		} else if (track1 >= tracks) {
			track = tracks - 1;
		}
		
		return myCmd[Int((length * track) + row)]
	}
}
