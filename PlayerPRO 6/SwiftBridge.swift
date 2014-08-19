//
//  SwiftBridge.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Foundation

// files are here because these extensions cause Xcode's intellisense to crash.

extension PPInstrumentPlugHandler: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self);
	}
}

extension PPDigitalPlugHandler: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self);
	}
}

extension PPFilterPlugHandler: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self);
	}
}

