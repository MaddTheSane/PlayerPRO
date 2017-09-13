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
		func getHeaderData() -> Data {
			let headerData: [UInt8] = [0x4D, 0x54, 0x68, 0x64, 0, 0, 0, 6, 0]
			return Data(headerData)
		}
		//var myErr = MADErr.NoErr;
		do {
			let aFile = try FileHandle(forReadingFrom: theURL)
			let fileData = aFile.readData(ofLength: 9)
			aFile.closeFile()
			let headerData = getHeaderData()
			
			if fileData == headerData {
				return
			} else {
				throw MADErr.fileNotSupportedByThisPlug
			}
		}
	}
}
