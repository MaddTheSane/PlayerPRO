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
		if let rv = FVResourceFile(contentsOfURL: AlienFileURL, error: nil) {
			if !rv.isResourceFork {
				return false
			}
			for res in rv.types as! [FVResourceType] {
				if res.type == "snd " {
					return true
				}
			}
		}
		return false
	}
	
	public func importSampleAtURL(sampleURL: NSURL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		return .OrderNotImplemented
	}
}
