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
	return aPcmd.getCommand(Int(row), track: Int(track))
}

public struct PPKPcmd {
	public var tracks: Int
	public var length: Int
	public var trackStart: Int
	public var posStart: Int
	public var structSize: Int { get {
		return sizeof(Pcmd.Type) + myCmd.count * sizeof(Cmd.Type)
		}}
	public var myCmd = [Cmd]()
	
	public init(_ aPcmd: UnsafePointer<Pcmd>) {
		let unwrapped = aPcmd.memory
		let myCmdPos: Int = Int(unwrapped.structSize) - sizeof(Pcmd.Type)
		tracks = Int(unwrapped.tracks)
		length = Int(unwrapped.length)
		trackStart = Int(unwrapped.trackStart)
		posStart = Int(unwrapped.posStart)
		let theirCmdPtr: UnsafePointer<Cmd> = UnsafePointer<Cmd>(aPcmd.advancedBy(sizeof(Pcmd.Type)))
		for i in 0 ..< myCmdPos / sizeof(Cmd.Type) {
			myCmd.append(theirCmdPtr[i])
		}
	}
	
	public mutating func replaceCmd(atRow row1: Int, track track1: Int, command: Cmd) {
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
		
		myCmd[(length * track) + row1] = command
	}
	
	public func getCommand(row: Int, track track1: Int) -> Cmd {
		var track = track1
		var row1 = row
		if (row < 0) {
			row1 = 0;
		} else if (row >= length) {
			row1 = length - 1;
		}
		
		if (track < 0) {
			track = 0;
		} else if (track >= tracks) {
			track = tracks - 1;
		}
		
		return myCmd[(length * track) + row1]
	}

	
	public var valid: Bool { get {
		return (tracks * length) == myCmd.count
		}}
}
