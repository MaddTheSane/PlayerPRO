//
//  SwiftBridge.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Foundation

// files are here because these extensions cause Xcode's IntelliSense to crash.

extension PPInstrumentPlugHandler: SequenceType {
	public func generate() -> IndexingGenerator<[PPInstrumentImporterObject]> {
		return (plugInArray as NSArray as [PPInstrumentImporterObject]).generate();
	}
}

extension PPDigitalPlugHandler: SequenceType {
	public func generate() -> IndexingGenerator<[PPDigitalPlugInObject]> {
		return (plugInArray as NSArray as [PPDigitalPlugInObject]).generate();
	}
}

extension PPFilterPlugHandler: SequenceType {
	public func generate() -> IndexingGenerator<[PPFilterPlugObject]> {
		return (plugInArray as NSArray as [PPFilterPlugObject]).generate();
	}
}

