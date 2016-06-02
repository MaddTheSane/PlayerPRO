//
//  PPKPcmd.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/27/14.
//
//

import Foundation
import PlayerPROCore

/// A Swift-friendly way of messing with Pcmds.
public struct PPKPcmd: MutableCollection, CommandIterator {
	public typealias Generator = IndexingIterator<[Cmd]>
	public typealias Index = Int
	public var tracks: Int16
	public var length: Int16
	public var trackStart: Int16
	public var positionStart: Int16
	public var myCmd = [Cmd]()
	
	/// returns what the size of a Pcmd structure would be, or `nil`
	/// if the Pcmd structure isn't valid.
	public var structSize: Int32? {
		if self.valid {
			return Int32(sizeof(Pcmd.Type) + myCmd.count * sizeof(Cmd.Type))
		} else {
			return nil
		}
	}
	
	public var startIndex: Int {
		return 0
	}
	
	public var endIndex: Int {
		return myCmd.count
	}
	
	public func index(after: Int) -> Int {
		return after + 1
	}
	
	public subscript (position: Int) -> Cmd {
		get {
			return myCmd[position]
		}
		set {
			myCmd[position] = newValue
		}
	}

	public func makeIterator() -> IndexingIterator<[Cmd]> {
		return myCmd.makeIterator()
	}
	
	/// Must be freed after use, otherwise memory *will* leak.
	public func newIntPcmd() -> UnsafeMutablePointer<IntPcmd>? {
		if let _ = structSize {
			let ourIntPcmd = intPcmd!
			let toRet = UnsafeMutablePointer<IntPcmd>(allocatingCapacity: 1)
			toRet.initialize(with: ourIntPcmd)
			
			return toRet
		} else {
			return nil
		}
	}
	
	public var commandLength: Int16 {
		return length
	}
	
	public var commandTracks: Int16 {
		return tracks
	}
	
	public var intPcmd: IntPcmd? {
		if let _ = structSize {
			var toRet = IntPcmd()
			toRet.tracks = tracks
			toRet.length = length
			toRet.trackStart = trackStart
			toRet.posStart = positionStart
			toRet.cmdCount = Int32(myCmd.count)
			toRet.myCmd = UnsafeMutablePointer<Cmd>(allocatingCapacity: myCmd.count)
			
			for (i, aCmd) in myCmd.enumerated() {
				toRet.myCmd[i] = aCmd
			}
			
			return toRet
		} else {
			return nil
		}
	}
	
	/// Must be freed after use, otherwise memory *will* leak.
	public func newPcmd() -> UnsafeMutablePointer<Pcmd>? {
		if let _ = structSize {
			let ourIntPcmd = newIntPcmd()!
			let toRet = MADIntPcmdToPcmd(ourIntPcmd, true)
			return toRet
		} else {
			return nil;
		}
	}
	
	/// returns `false` if the tracks and length don't match the count
	/// of `myCmd` objects.
	public var valid: Bool {
		return Int(tracks * length) == myCmd.count
	}
	
	public init() {
		tracks = 1
		trackStart = 0
		length = 1
		positionStart = 0
		myCmd.append(Cmd.blankCmd())
	}
	
	/// Initializes a `PPKPcmd` structure with the specified tracks and rows.
	///
	/// - parameter tracks: The amount of tracks.
	/// - parameter startTrack: The starting location of the tracks. Default is `0`.
	/// - parameter rows: The amount of rows.
	/// - parameter startPosition: The starting location of the rows. Default is `0`.
	public init(tracks: Int16, startTrack: Int16 = 0, rows: Int16, startPosition: Int16 = 0) {
		self.tracks = tracks
		trackStart = startTrack
		length = rows
		positionStart = startPosition
		for _ in 0 ..< tracks * rows {
			myCmd.append(Cmd.blankCmd())
		}
	}
	
	/// The data in `aPcmd` is copied, so you don't have to worry about
	/// keeping track of it afterwards.
	public init(_ aPcmd: UnsafeMutablePointer<Pcmd>) {
		let ourIntPcmd = MADPcmdToInt(aPcmd)
		self.init(intPcmd: ourIntPcmd!)
		MADFreeIntPcmd(ourIntPcmd)
	}
	
	/// The data in `aTmp` is copied, so you don't have to worry about
	/// keeping track of it afterwards.
	public init(intPcmd aTmp: UnsafeMutablePointer<IntPcmd>) {
		let intPcmd = aTmp.pointee
		tracks = intPcmd.tracks
		length = intPcmd.length
		trackStart = intPcmd.trackStart
		positionStart = intPcmd.posStart
		for i in 0 ..< Int(intPcmd.cmdCount) {
			myCmd.append(intPcmd.myCmd[i])
		}
	}
	
	/// Adds a row to the struct.
	public mutating func addRow() {
		#if false
			length += 1
			for i in 0 ..< tracks {
			myCmd.append(Cmd())
			}
			#else
			// Brute forcing our way, because I can't get it to work the other way yet.
			var anewPcmd = PPKPcmd(tracks: self.tracks, startTrack: self.trackStart, rows: length + 1, startPosition: positionStart)
			
			for i in 0 ..< (tracks * length) {
				let oldCmd = getCommand(row: i % length, track: i / length)
				anewPcmd.replaceCommand(row: i % length, track: i / length, command: oldCmd)
			}
			
			self = anewPcmd
		#endif
	}

	/// Adds a track to the struct.
	public mutating func addTrack() {
		#if false
			for var i = length; i > 0; i-- {
			let acmd = Cmd()
			myCmd.insert(Cmd(), atIndex: Int(i * tracks))
			}
			tracks += 1
			#else
			// Brute forcing our way, because I can't get it to work the other way yet.
			var anewPcmd = PPKPcmd(tracks: self.tracks + 1, startTrack: self.trackStart, rows: length, startPosition: positionStart)
			
			for i in 0 ..< (tracks * length) {
				let oldCmd = getCommand(row: i % length, track: i / length)
				anewPcmd.replaceCommand(row: i % length, track: i / length, command: oldCmd)
			}
			
			self = anewPcmd
		#endif
	}
	
	public mutating func modifyCommand(row row1: Int16, track track1: Int16, commandBlock: (inout Cmd)-> ()) {
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
	
	public mutating func addCmd(command: Cmd = Cmd.blankCmd()) {
		myCmd.append(command)
	}
	
	public mutating func replaceCommand(row row1: Int16, track track1: Int16, command: Cmd) {
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
	
	public func getCommand(row row1: Int16, track track1: Int16) -> Cmd {
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
