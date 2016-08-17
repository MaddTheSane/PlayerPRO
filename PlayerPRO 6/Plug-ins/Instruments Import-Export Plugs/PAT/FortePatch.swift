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

private func importPAT(_ insHeader: PPInstrumentObject, data: NSData) -> MADErr {
	var PATHeader: UnsafePointer<PatchHeader>
	var PATIns: UnsafePointer<PatInsHeader>
	var PATSamp: UnsafePointer<PatSampHeader>
	var PATData = data.bytes.assumingMemoryBound(to: UInt8.self)
	let scale_table: [UInt32] = {
		var toRet: [UInt32] = [
		16351, 17323, 18354, 19445, 20601, 21826, 23124, 24499, 25956, 27500, 29135, 30867,
		32703, 34647, 36708, 38890, 41203, 43653, 46249, 48999, 51913, 54999, 58270, 61735,
		65406, 69295, 73416, 77781, 82406, 87306, 92498, 97998, 103826, 109999, 116540, 123470,
		130812, 138591, 146832, 155563, 164813, 174614, 184997, 195997, 207652, 219999, 233081, 246941,
		261625, 277182, 293664, 311126, 329627, 349228, 369994, 391995, 415304, 440000, 466163, 493883,
		523251, 554365, 587329, 622254, 659255, 698456, 739989, 783991, 830609, 880000, 932328, 987767,
		1046503, 1108731, 1174660, 1244509, 1318511, 1396914, 1479979, 1567983, 1661220, 1760002, 1864657, 1975536,
		2093007, 2217464, 2349321, 2489019, 2637024, 2793830, 2959960, 3135968, 3322443, 3520006, 3729316, 3951073,
		4186073, 4434930, 4698645, 4978041, 5274051, 5587663, 5919922, 6271939, 6644889, 7040015, 7458636, 7902150]
		
		let toRemain = [UInt32](repeating: toRet.last!, count: 200 - toRet.count)
		toRet += toRemain
		
		return toRet
	}()
	
	// PATCH HEADER
	PATHeader = UnsafeRawPointer(PATData).assumingMemoryBound(to: PatchHeader.self)
	PATData += 129
	
	insHeader.resetInstrument()
	
	if (PATHeader.pointee.InsNo != 1) {
		return .fileNotSupportedByThisPlug;
	}
	
	let sampleCount = (Int(PATHeader.pointee.LoSamp) << 8) + Int(PATHeader.pointee.HiSamp)
	
	// INS HEADER -- Read only the first instrument
	PATIns = UnsafeRawPointer(PATData).assumingMemoryBound(to: PatInsHeader.self)

	//let patSize = PATIns.memory.size.littleEndian
	PATData += 63;
	
	insHeader.name = {
		let insNameBytes: [Int8] = try! arrayFromObject(reflecting: PATIns.pointee.name, appendLastObject: 0)
		let allowable = insNameBytes.map { (i) -> UInt8 in
			return UInt8(i)
		}
		return String(bytes: allowable, encoding: String.Encoding.macOSRoman) ?? ""
		}()
	
	// LAYERS
	for _ in 0..<PATIns.pointee.layer {
		PATData += 47;
	}
	
	// SAMPLES
	for x in 0..<sampleCount {
		let curData = PPSampleObject();
		var signedData: Bool
		
		PATSamp = UnsafeRawPointer(PATData).assumingMemoryBound(to: PatSampHeader.self)
		//curData = sample[x] = inMADCreateSample();
		
		curData.name = {
			let insNameBytes: [Int8] = try! arrayFromObject(reflecting: PATSamp.pointee.name, appendLastObject: 0)
			let allowable = insNameBytes.map { (i) -> UInt8 in
				return UInt8(i)
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
		
		if (PATSamp.pointee.Flag & 0x01) != 0 {
			curData.amplitude = 16;
		} else {
			curData.amplitude = 8;
		}
		
		if (PATSamp.pointee.Flag & 0x02) != 0 {
			signedData = true;
		} else {
			signedData = false;
		}
		
		if (PATSamp.pointee.Flag & 0x04) == 0 {
			curData.loopBegin = 0;
			curData.loopSize = 0;
		}
		
		if (PATSamp.pointee.Flag & 0x08) != 0 {
			curData.loopType = .pingPong;
		} else {
			curData.loopType = .classic;
		}
		
		///////////////
		var tmpHeader = PATSamp.pointee
		tmpHeader.minFreq = tmpHeader.minFreq.littleEndian
		tmpHeader.maxFreq = tmpHeader.maxFreq.littleEndian
		tmpHeader.originRate = tmpHeader.originRate.littleEndian
		
		for i in 0..<107 {
			if scale_table[i] >= UInt32(PATSamp.pointee.originRate) {
				tmpHeader.originRate = Int32(i);
				break
			}
		}
		
		curData.relativeNote = Int8(60 - (12 + PATSamp.pointee.originRate))
		
		for i in 0..<107 {
			if scale_table[i] >= UInt32(PATSamp.pointee.minFreq) {
				tmpHeader.minFreq = Int32(i)
				break
			}
		}
		
		for i in 0..<107{
			if scale_table[i] >= UInt32(PATSamp.pointee.maxFreq) {
				tmpHeader.maxFreq = Int32(i)
				break
			}
		}
		
		for i in PATSamp.pointee.minFreq ..< PATSamp.pointee.maxFreq {
			if (i < 96 && i >= 0) {
				insHeader.what[Int(i)] = UInt8(x)
			}
		}
		
		PATData += 96;
		
		// DATA
		let aDataObj = NSMutableData(bytes: UnsafeRawPointer(PATData), length: Int(sampSize))
		let aData = aDataObj.mutableBytes.assumingMemoryBound(to: UInt8.self)
		
		//if aData != nil {
			
			if (curData.amplitude == 16) {
				let tt = aDataObj.mutableBytes.assumingMemoryBound(to: UInt16.self)
				
				DispatchQueue.concurrentPerform(iterations: Int(sampSize / 2), execute: { (tL) -> Void in
					tt[tL] = tt[tL].littleEndian
					
					if signedData {
						tt[tL] = tt[tL] &+ 0x8000
					}
				})
			} else {
				if signedData {
					DispatchQueue.concurrentPerform(iterations: Int(sampSize), execute: { (ixi) -> Void in
						aData[ixi] = aData[ixi] &+ 0x80
					})
				}
			}
			curData.data = aDataObj as Data
		//}
		
		PATData += Int(PATSamp.pointee.size)
		
		insHeader.add(curData)
	}
	
	return .noErr
}


public final class FortePatch: NSObject, PPInstrumentImportPlugin {
	public let hasUIForImport = false
	//const char headerStr[20] = "GF1PATCH110\0\0";
	private let headerData: Data = {
		var headerChar: [ASCIICharacter] = [.LetterUppercaseG, .LetterUppercaseF, .NumberOne, .LetterUppercaseP, .LetterUppercaseA,
		.LetterUppercaseT, .LetterUppercaseC, .LetterUppercaseH, .NumberOne, .NumberOne, .NumberZero, .NullCharacter]
		var headerStr = headerChar.map({ (aChar) -> UInt8 in
			return UInt8(aChar.rawValue)
		})
		
		return Data(bytes: headerStr)
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
	
	public func importInstrument(at sampleURL: URL, instrument InsHeader: AutoreleasingUnsafeMutablePointer<PPInstrumentObject>?, driver: PPDriver) -> MADErr {
		if let inData = try? Data(contentsOf: sampleURL) {
			if let ourIns = PPInstrumentObject() {
				ourIns.resetInstrument()
				
				let iErr = importPAT(ourIns, data: inData as NSData)
				if iErr == .noErr {
					InsHeader?.pointee = ourIns
				}
				return iErr
			} else {
				return .needMemory
			}
		} else {
			return .readingErr
		}
	}
}
