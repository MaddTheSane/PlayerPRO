//
//  ClassicSound.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/13/15.
//
//

import Cocoa
import AudioToolbox
import PlayerPROCore
import PlayerPROKit
import SwiftAdditions
import SwiftAudioAdditions

public final class ClassicSound: NSObject, PPSampleImportPlugin {
	public let hasUIForImport = false
	
	override init() {
		super.init()
	}
	
	required convenience public init(forPlugIn: ()) {
		self.init()
	}
	
	public func canImportSample(at AlienFileURL: URL) -> Bool {
		do {
			let fileRef = try FileHandle(forReadingFrom: AlienFileURL)
			let data = fileRef.readData(ofLength: 128)
			return canOpenData(data)
		} catch _ {
			return false
		}
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) throws {
		let data = try Data(contentsOf: sampleURL)
		let tmpURL = try assetForSND(data)
		defer {
			do {
				try FileManager.default.removeItem(at: tmpURL)
			} catch _ {
			}
		}
		
		let aSamp = try readAIFF(at: tmpURL)
		aSamp.name = sampleURL.deletingPathExtension().lastPathComponent
		sample.pointee = aSamp
	}
}
