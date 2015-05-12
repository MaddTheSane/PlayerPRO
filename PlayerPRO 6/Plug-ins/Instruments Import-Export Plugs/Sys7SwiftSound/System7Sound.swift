//
//  System7Sound.swift
//  PPMacho
//
//  Created by C.W. Betts on 5/11/15.
//
//

import Cocoa
import AudioToolbox
import PlayerPROCore
import PlayerPROKit
import SwiftAdditions
import SwiftAudioAdditions


public final class System7Sound: NSObject, PPSampleImportPlugin {
	public let hasUIForImport = false

	override init() {
		super.init()
	}
	
	required convenience public init!(forPlugIn: ()) {
		self.init()
	}

	public func canImportSampleAtURL(AlienFileURL: NSURL) -> Bool {
		if let rv = FVResourceFile.resourceFileWithContentsOfURL(AlienFileURL, error: nil) {
			if !rv.isResourceFork {
				return false
			}
			for res in rv.types {
				if res.type == "snd " {
					return true
				}
			}
		}
		return false
	}
	
	public func importSampleAtURL(sampleURL: NSURL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		if let rv = FVResourceFile.resourceFileWithContentsOfURL(sampleURL, error: nil) {
			for res in rv.types {
				if res.type == "snd " {
					for aRes in res.resources {
						var errStr = ""
						if let data = aRes.data, asset = assetForSND(data, &errStr) {
							var asample = PPSampleObject()
							if AIFFAtURL(asset, toSample: asample) {
								asample.name = sampleURL.lastPathComponent!.stringByDeletingPathExtension
								sample.memory = asample
								
								return .NoErr
							}
							
							return .IncompatibleFile
						} else {
							return .IncompatibleFile
						}
					}
				}
			}
			return .IncompatibleFile
		} else {
			return .ReadingErr
		}
	}
}
