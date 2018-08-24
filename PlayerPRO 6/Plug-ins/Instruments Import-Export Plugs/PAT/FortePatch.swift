//
//  FortePatch.swift
//  PPMacho
//
//  Created by C.W. Betts on 4/24/15.
//
//

import Cocoa
import PlayerPROKit
import SwiftAdditions

private func importPAT(_ insHeader: PPInstrumentObject, data: Data) throws {
	try data.withUnsafeBytes { (PATData1: UnsafePointer<UInt8>) -> Void in
		var PATHeader: UnsafePointer<PatchHeader>
		var PATIns: UnsafePointer<PatInsHeader>
		var PATSamp: UnsafePointer<PatSampHeader>
		var PATData = PATData1
		
		let scale_table: [Int32] = [
			16351, 17323, 18354, 19445, 20601, 21826, 23124, 24499, 25956, 27500, 29135, 30867,
			32703, 34647, 36708, 38890, 41203, 43653, 46249, 48999, 51913, 54999, 58270, 61735,
			65406, 69295, 73416, 77781, 82406, 87306, 92498, 97998, 103826, 109999, 116540, 123470,
			130812, 138591, 146832, 155563, 164813, 174614, 184997, 195997, 207652, 219999, 233081, 246941,
			261625, 277182, 293664, 311126, 329627, 349228, 369994, 391995, 415304, 440000, 466163, 493883,
			523251, 554365, 587329, 622254, 659255, 698456, 739989, 783991, 830609, 880000, 932328, 987767,
			1046503, 1108731, 1174660, 1244509, 1318511, 1396914, 1479979, 1567983, 1661220, 1760002, 1864657, 1975536,
			2093007, 2217464, 2349321, 2489019, 2637024, 2793830, 2959960, 3135968, 3322443, 3520006, 3729316, 3951073,
			4186073, 4434930, 4698645, 4978041, 5274051, 5587663, 5919922, 6271939, 6644889, 7040015, 7458636, 7902150]
		
		// PATCH HEADER
		PATHeader = UnsafeRawPointer(PATData).assumingMemoryBound(to: PatchHeader.self)
		PATData += 129
		
		insHeader.resetInstrument()
		
		if PATHeader.pointee.InsNo != 1 {
			throw PPMADError(.fileNotSupportedByThisPlug)
		}
		
		let sampleCount = Int(PATHeader.pointee.Samp.littleEndian)
		
		// INS HEADER -- Read only the first instrument
		PATIns = UnsafeRawPointer(PATData).assumingMemoryBound(to: PatInsHeader.self)
		
		//let patSize = PATIns.memory.size.littleEndian
		PATData = PATData.advanced(by: 63)
		
		insHeader.name = {
			let insNameBytes: [Int8] = try! arrayFromObject(reflecting: PATIns.pointee.name, appendLastObject: 0)
			let allowable = insNameBytes.map { (i) -> UInt8 in
				return UInt8(bitPattern: i)
			}
			return String(bytes: allowable, encoding: String.Encoding.macOSRoman) ?? ""
		}()
		
		// LAYERS
		for _ in 0 ..< PATIns.pointee.layer {
			PATData = PATData.advanced(by: 47)
		}
		
		// SAMPLES
		for x in 0..<sampleCount {
			let curData = PPSampleObject()
			let signedData: Bool
			
			PATSamp = UnsafeRawPointer(PATData).assumingMemoryBound(to: PatSampHeader.self)
			//curData = sample[x] = inMADCreateSample();
			
			curData.name = {
				let insNameBytes: [Int8] = try! arrayFromObject(reflecting: PATSamp.pointee.name, appendLastObject: 0)
				let allowable = insNameBytes.map { (i) -> UInt8 in
					return UInt8(bitPattern: i)
				}
				return String(bytes: allowable, encoding: String.Encoding.macOSRoman) ?? ""
			}()
			
			//int sampSize, sampStartLoop, sampEndLoop;
			let sampRate = PATSamp.pointee.rate.littleEndian
			let sampSize = PATSamp.pointee.size.littleEndian
			let sampStartLoop = PATSamp.pointee.startLoop.littleEndian
			let sampEndLoop = PATSamp.pointee.endLoop.littleEndian
			
			//curData->size = PATSamp->size;
			curData.loopBegin = sampStartLoop;
			curData.loopSize = sampEndLoop - sampStartLoop;
			curData.c2spd = sampRate;
			
			curData.volume = 64;
			curData.loopType = MADLoopType.classic
			
			if PATSamp.pointee.Flag.contains(.is16Bit) {
				curData.amplitude = 16;
			} else {
				curData.amplitude = 8;
			}
			
			if PATSamp.pointee.Flag.contains(.isSigned) {
				signedData = true;
			} else {
				signedData = false;
			}
			
			if !PATSamp.pointee.Flag.contains(.loops) {
				curData.loopBegin = 0;
				curData.loopSize = 0;
			}
			
			if PATSamp.pointee.Flag.contains(.isPingPong) {
				curData.loopType = .pingPong;
			} else {
				curData.loopType = .classic;
			}
			
			///////////////
			var tmpHeader = PATSamp.pointee
			tmpHeader.minFreq = tmpHeader.minFreq.littleEndian
			tmpHeader.maxFreq = tmpHeader.maxFreq.littleEndian
			tmpHeader.originRate = tmpHeader.originRate.littleEndian
			
			for i in 0 ..< 107 {
				if scale_table[i] >= PATSamp.pointee.originRate {
					tmpHeader.originRate = Int32(i);
					break
				}
			}
			
			curData.realNote = Int8(60 - (12 + PATSamp.pointee.originRate))
			
			for i in 0 ..< 107 {
				if scale_table[i] >= PATSamp.pointee.minFreq {
					tmpHeader.minFreq = Int32(i)
					break
				}
			}
			
			for i in 0 ..< 107 {
				if scale_table[i] >= PATSamp.pointee.maxFreq {
					tmpHeader.maxFreq = Int32(i)
					break
				}
			}
			
			for i in PATSamp.pointee.minFreq ..< PATSamp.pointee.maxFreq {
				if i < 96 && i >= 0 {
					insHeader.what[Int(i)] = UInt8(x)
				}
			}
			
			PATData += 96;
			
			// DATA
			var dat2 = Data(bytes: UnsafeRawPointer(PATData), count: Int(sampSize))
			
			//if aData != nil {
			let BYTESWAP_STRIDE = 8
			if (curData.amplitude == 16) {
				dat2.withUnsafeMutableBytes({ (tt: UnsafeMutablePointer<UInt16>) -> Void in
					DispatchQueue.concurrentPerform(iterations: Int(sampSize / 2) / BYTESWAP_STRIDE, execute: { (tL) -> Void in
						for j in 0 ..< BYTESWAP_STRIDE {
							tt[tL*BYTESWAP_STRIDE+j] = tt[tL*BYTESWAP_STRIDE+j].littleEndian
						}

						if signedData {
							for j in 0 ..< BYTESWAP_STRIDE {
								tt[tL*BYTESWAP_STRIDE+j] = tt[tL*BYTESWAP_STRIDE+j] &+ 0x8000
							}
						}
					})
					if Int(sampSize / 2) % BYTESWAP_STRIDE != 0 {
						for i in Int(sampSize / 2) ..< (Int(sampSize / 2) - (Int(sampSize / 2)) % BYTESWAP_STRIDE) {
							tt[i] = tt[i].littleEndian

							if signedData {
								tt[i] = tt[i] &+ 0x8000
							}
						}
					}
				})
			} else {
				if signedData {
					dat2.withUnsafeMutableBytes({ (aData: UnsafeMutablePointer<UInt8>) -> Void in
						DispatchQueue.concurrentPerform(iterations: Int(sampSize) / BYTESWAP_STRIDE, execute: { (ixi) -> Void in
							for j in 0 ..< BYTESWAP_STRIDE {
								aData[ixi*BYTESWAP_STRIDE+j] = aData[ixi*BYTESWAP_STRIDE+j] &+ 0x80
							}
						})
						if Int(sampSize) % BYTESWAP_STRIDE != 0 {
							for i in Int(sampSize) ..< (Int(sampSize) - (Int(sampSize)) % BYTESWAP_STRIDE) {
								aData[i] = aData[i] &+ 0x80
							}
						}
					})
				}
			}
			curData.data = dat2
			//}
			
			PATData += Int(PATSamp.pointee.size)
			
			insHeader.add(curData)
		}
	}
}


public final class FortePatch: NSObject, PPInstrumentImportPlugin {
	public let hasUIForImport = false
	//const char headerStr[20] = "GF1PATCH110\0\0";
	private let headerData: Data = {
		return Data(bytes: Array("GF1PATCH110\0".utf8))
	}()

	override public init() {
		super.init()
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	public func canImportInstrument(at url: URL) -> Bool {
		do {
			let aHandle = try FileHandle(forReadingFrom:url)
			let fileData = aHandle.readData(ofLength: headerData.count)
			
			return (fileData == headerData)
		} catch _ {
			return false
		}
	}
	
	public func importInstrument(at sampleURL: URL, instrument InsHeader: AutoreleasingUnsafeMutablePointer<PPInstrumentObject?>, driver: PPDriver) throws {
		let inData: Data
		do {
			inData = try Data(contentsOf: sampleURL)
		} catch {
			throw PPMADError(.reading, userInfo: [NSUnderlyingErrorKey : error])
		}
		if let ourIns = PPInstrumentObject() as PPInstrumentObject? {
			ourIns.resetInstrument()
			
			try importPAT(ourIns, data: inData)
			InsHeader.pointee = ourIns
		} else {
			throw PPMADError(.needsMemory)
		}
	}
}
