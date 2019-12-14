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
import ResourceFork


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
			let rv = try FVResourceFile.resourceFileWithContents(of: AlienFileURL)
			if !rv.isResourceFork {
				return false
			}
			for res in rv.types {
				if res.type == toOSType("snd ") {
					return true
				}
			}
		} catch _ {
		}
		return false
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) throws {
		let rv = try FVResourceFile.resourceFileWithContents(of: sampleURL)
		for res in rv.types {
			if res.type == toOSType("snd ") {
				for aRes in res.resources {
					if let data = aRes.data {
						let asset = try assetForSND(data)
						let asample = try readAIFF(at: asset)
						
						asample.name = sampleURL.deletingPathExtension().lastPathComponent
						sample.pointee = asample
						do {
							try FileManager.default.removeItem(at: asset)
						} catch {
							
						}
						
						return
					}
				}
			}
		}
		throw PPMADError(.fileNotSupportedByThisPlug)
	}
}
