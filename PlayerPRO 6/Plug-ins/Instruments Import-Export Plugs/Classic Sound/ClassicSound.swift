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
			return false
		}
		
		return false
	}
	
	public func importSample(at sampleURL: URL, sample: AutoreleasingUnsafeMutablePointer<PPSampleObject?>, driver: PPDriver) -> MADErr {
		var iErr = MADErr.readingErr
		if let data = try? Data(contentsOf: sampleURL), let tmpURL = assetForSND(data, error: &iErr), iErr == .noErr {
			defer {
				do {
					try FileManager.default.removeItem(at: tmpURL)
				} catch _ {
				}
			}
			do {
				let aSamp = try readAIFF(at: tmpURL)
				aSamp.name = (sampleURL.lastPathComponent as NSString).deletingPathExtension
				sample.pointee = aSamp
				return .noErr
			} catch let error as MADErr {
				return error
			} catch {
				return .unknownErr
			}
		}
		
		return iErr;
	}
}
