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
			let fileRef = try FileHandle(forUpdating: AlienFileURL)
			let data = fileRef.readData(ofLength: 128)
			return canOpenData(data)
		} catch _ {
		}
		
		return false
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject>?, driver: PPDriver) -> MADErr {
		let aSamp = PPSampleObject()
		var iErr = MADErr.noErr
		if let data = try? Data(contentsOf: sampleURL), tmpURL = assetForSND(data, error: &iErr) where iErr == .noErr {
			iErr = AIFFAtURL(tmpURL, toSample: aSamp)
			if iErr == .noErr {
				sample?.pointee = aSamp
				do {
				try FileManager.default().removeItem(at: tmpURL)
				} catch {
					
				}
			}
		}
		
		return iErr;
	}
}
