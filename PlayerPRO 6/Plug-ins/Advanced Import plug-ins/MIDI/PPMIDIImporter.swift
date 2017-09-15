//
//  PPMIDIImporter.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/18/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit

private let headerData: Data = {
	let headerData2: [UInt8] = [0x4D, 0x54, 0x68, 0x64, 0, 0, 0, 6, 0]
	return Data(headerData2)
}()

@objc(PPMIDIImporter) final public class MIDIImporter: NSObject, PPComplexImportPlugInterface {
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	override public init() {
		super.init()
	}

	public func beginImport(of theURL: URL, withHandler handler: @escaping PPComplexImportHandler) {
		let aImp = MIDIImporterController.newWithLocation(theURL, handler: handler)
		aImp.beginImportModalSession()
	}
	
	public func canImport(_ theURL: URL) throws {
		let aFile = try FileHandle(forReadingFrom: theURL)
		let fileData = aFile.readData(ofLength: 9)
		aFile.closeFile()
		
		if fileData == headerData {
			return
		} else {
			throw MADErr.fileNotSupportedByThisPlug
		}
	}
}
