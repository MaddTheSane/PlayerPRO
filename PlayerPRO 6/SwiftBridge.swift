//
//  SwiftBridge.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Foundation

public func ==(lhs: PPPlugInObject, rhs: PPPlugInObject) -> Bool {
	return lhs.isEqual(rhs)
}

extension PPPlugInObject: Hashable {
	override public var hashValue: Int {
		return self.hash
	}
}

extension PPInstrumentPlugHandler: SequenceType {
	public func generate() -> IndexingGenerator<[PPInstrumentImporterObject]> {
		return (plugInArray as [PPInstrumentImporterObject]).generate();
	}
}

extension PPFilterPlugHandler: SequenceType {
	public func generate() -> IndexingGenerator<[PPFilterPlugObject]> {
		return (plugInArray as [PPFilterPlugObject]).generate();
	}
}
