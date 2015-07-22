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
	
	public func canImportSampleAtURL(AlienFileURL: NSURL) -> Bool {
		do {
			let fileRef = try NSFileHandle(forUpdatingURL: AlienFileURL)
			let data = fileRef.readDataOfLength(128)
			return canOpenData(data)
		} catch _ {
		}
		
		return false
	}
	
	public func importSampleAtURL(sampleURL: NSURL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		let aSamp = PPSampleObject()
		var iErr = MADErr.NoErr
		if let data = NSData(contentsOfURL: sampleURL), tmpURL = assetForSND(data, error: &iErr) where iErr == .NoErr {
			iErr = AIFFAtURL(tmpURL, toSample: aSamp)
			if iErr == .NoErr {
				sample.memory = aSamp
				do {
				try NSFileManager.defaultManager().removeItemAtURL(tmpURL)
				} catch {
					
				}
			}
		}
		
		return iErr;
	}
}
