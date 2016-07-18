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
	
	required convenience public init(forPlugIn: ()) {
		self.init()
	}

	public func canImportSample(at AlienFileURL: URL) -> Bool {
		do {
			let rv = try FVResourceFile.resourceFileWithContentsOfURL(AlienFileURL)
			if !rv.isResourceFork {
				return false
			}
			for res in rv.types {
				if res.type == "snd " {
					return true
				}
			}
		} catch _ {
		}
		return false
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject>?, driver: PPDriver) -> MADErr {
		do {
		let rv = try FVResourceFile.resourceFileWithContentsOfURL(sampleURL)
			for res in rv.types {
				if res.type == "snd " {
					for aRes in res.resources {
						var errStr = MADErr.noErr
						if let data = aRes.data, asset = assetForSND(data, error: &errStr) {
							let asample = PPSampleObject()
							errStr = AIFFAtURL(asset, toSample: asample)
							if errStr == .noErr {
								asample.name = (sampleURL.lastPathComponent! as NSString).deletingPathExtension
								sample?.pointee = asample
							}
							do {
								try FileManager.default.removeItem(at: asset)
							} catch {
								
							}
							
							return errStr
						} else {
							return errStr
						}
					}
				}
			}
			return .fileNotSupportedByThisPlug
		} catch {
			return .readingErr
		}
	}
}
