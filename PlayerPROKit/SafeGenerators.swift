//
//  SafeGenerators.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/19/14.
//
//

import Foundation

#if false
//Disabled because I'm not yet comfortable using this yet.
final public class PPLibraryGenerator: GeneratorType {
	typealias Element = PPLibraryObject
	let currentLibrary: PPLibrary
	var currentNumber = 0
	let totalNumber:Int
	public init(library:PPLibrary) {
		currentLibrary = library
		totalNumber = Int(library.pluginCount)
	}
	public mutating func next() -> PPLibraryObject? {
		if currentNumber > totalNumber {
			var tmpPlug = currentLibrary.pluginAtIndex(UInt(currentNumber))
			currentNumber++
			return tmpPlug
		}else {
			return nil
		}
	}
}

extension PPLibrary: SequenceType {
	public func generate() -> PPLibraryGenerator {
		return PPLibraryGenerator(library: self);
	}
}

#else

extension PPLibrary: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self);
	}
}
#endif

extension PPInstrumentObject: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self);
	}
}

extension PPPatternObject: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self);
	}
}

