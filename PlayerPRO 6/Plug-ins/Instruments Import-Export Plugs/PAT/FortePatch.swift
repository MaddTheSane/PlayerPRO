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

private func importPAT(insHeader: PPInstrumentObject, data: NSData) -> MADErr {
	var PATHeader: UnsafePointer<PatchHeader>
	var PATIns: UnsafePointer<PatInsHeader>
	var PATSamp: UnsafePointer<PatSampHeader>
	var PATData = UnsafePointer<UInt8>(data.bytes)
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
		
		//let toRemain =
		
		return toRet
	}()
	
	// PATCH HEADER
	PATHeader = UnsafePointer<PatchHeader>(PATData)
	PATData += 129
	
	insHeader.resetInstrument()
	
	if (PATHeader.memory.InsNo != 1) {
		return .FileNotSupportedByThisPlug;
	}
	
	let sampleCount = (Int(PATHeader.memory.LoSamp) << 8) + Int(PATHeader.memory.HiSamp)
	
	// INS HEADER -- Read only the first instrument
	PATIns = UnsafePointer<PatInsHeader>(PATData)

	let patSize = PATIns.memory.size.littleEndian
	PATData += 63;
	
	insHeader.name = {
		let insNameBytes: [Int8] = getArrayFromMirror(reflect(PATIns.memory.name), appendLastObject: 0)
		let allowable = insNameBytes.map { (i) -> UInt8 in
			return UInt8(i)
		}
		return String(bytes: allowable, encoding: NSMacOSRomanStringEncoding) ?? ""
		}()
	
	// LAYERS
	for i in 0..<PATIns.memory.layer {
		PATData += 47;
	}
	
	// SAMPLES
	for x in 0..<sampleCount {
		let curData = PPSampleObject();
		var signedData: Bool
		
		PATSamp = UnsafePointer<PatSampHeader>(PATData)
		//curData = sample[x] = inMADCreateSample();
		
		curData.name = {
			let insNameBytes: [Int8] = getArrayFromMirror(reflect(PATSamp.memory.name), appendLastObject: 0)
			let allowable = insNameBytes.map { (i) -> UInt8 in
				return UInt8(i)
			}
			return String(bytes: allowable, encoding: NSMacOSRomanStringEncoding) ?? ""
			}()
		
		//int sampSize, sampStartLoop, sampEndLoop;
		let sampRate = PATSamp.memory.rate.littleEndian
		let sampSize = PATSamp.memory.size.littleEndian
		let sampStartLoop = PATSamp.memory.startLoop.littleEndian
		let sampEndLoop = PATSamp.memory.endLoop.littleEndian
		
		//curData->size = PATSamp->size;
		curData.loopBegin = sampStartLoop;
		curData.loopSize = sampEndLoop - sampStartLoop;
		curData.c2spd = sampRate;
		
		curData.volume = 64;
		curData.loopType = MADLoopType.Classic
		
		if (PATSamp.memory.Flag & 0x01) {
			curData.amplitude = 16;
		} else {
			curData.amplitude = 8;
		}
		
		if (PATSamp.memory.Flag & 0x02) {
			signedData = true;
		} else {
			signedData = false;
		}
		
		if (!(PATSamp.memory.Flag & 0x04)) {
			curData.loopBegin = 0;
			curData.loopSize = 0;
		}
		
		if (PATSamp.memory.Flag & 0x08) {
			curData.loopType = .PingPong;
		} else {
			curData.loopType = .Classic;
		}
		
		///////////////
		var tmpHeader = PATSamp.memory
		tmpHeader.minFreq = tmpHeader.minFreq.littleEndian
		tmpHeader.maxFreq = tmpHeader.maxFreq.littleEndian
		tmpHeader.originRate = tmpHeader.originRate.littleEndian
		
		for var i = 0; i < 107; i++ {
			if scale_table[i] >= UInt32(PATSamp.memory.originRate) {
				tmpHeader.originRate = Int32(i);
				i = 108;
			}
		}
		
		curData.relativeNote = Int8(60 - (12 + PATSamp.memory.originRate))
		
		for var i = 0; i < 107; i++ {
			if scale_table[i] >= UInt32(PATSamp.memory.minFreq) {
				tmpHeader.minFreq = Int32(i)
				i = 108;
			}
		}
		
		for var i = 0; i < 107; i++ {
			if scale_table[i] >= UInt32(PATSamp.memory.maxFreq) {
				tmpHeader.maxFreq = Int32(i)
				i = 108;
			}
		}
		
		for var i = PATSamp.memory.minFreq; i < PATSamp.memory.maxFreq; i++ {
			if (i < 96 && i >= 0) {
				insHeader.what[Int(i)] = UInt8(x)
			}
		}
		
		PATData += 96;
		
		// DATA
		let aDataObj = NSMutableData(bytes: UnsafePointer<Void>(PATData), length: Int(sampSize))
		var aData = UnsafeMutablePointer<UInt8>(aDataObj.mutableBytes)
		
		if aData != nil {
			
			if (curData.amplitude == 16) {
				var tt = UnsafeMutablePointer<UInt16>(aData)
				
				dispatch_apply(Int(sampSize / 2), dispatch_get_global_queue(0, 0), { (tL) -> Void in
					tt[tL] = tt[tL].littleEndian
					
					if signedData {
						tt[tL] = tt[tL] &+ 0x8000
					}
				})
			} else {
				if signedData {
					dispatch_apply(Int(sampSize), dispatch_get_global_queue(0, 0), { (ixi) -> Void in
						aData[ixi] = aData[ixi] &+ 0x80
					})
				}
			}
			curData.data = aDataObj;
		}
		
		PATData += Int(PATSamp.memory.size)
		
		insHeader.addSampleObject(curData)
	}
	
	return .NoErr
}


public final class FortePatch: NSObject, PPInstrumentImportPlugin {
	public let hasUIForImport = false
	//const char headerStr[20] = "GF1PATCH110\0\0";
	private let headerData = NSData(byteArray:[0x47, 0x46, 0x31, 0x50, 0x41,0x54, 0x43, 0x48, 0x31, 0x31, 0x30, 0])

	override public init() {
		super.init()
	}
	
	public convenience init(forPlugIn: ()) {
		self.init()
	}
	
	public func canImportInstrumentAtURL(url: NSURL) -> Bool {
		if let aHandle = NSFileHandle(forReadingFromURL:url, error: nil) {
			let fileData = aHandle.readDataOfLength(headerData.length)
			
			return fileData.isEqualToData(headerData)
		} else {
			return false
		}
	}
	
	public func importInstrumentAtURL(sampleURL: NSURL, instrument InsHeader: AutoreleasingUnsafeMutablePointer<PPInstrumentObject?>, driver: PPDriver) -> MADErr {
		if let inData = NSData(contentsOfURL: sampleURL) {
			if let ourIns = PPInstrumentObject() {
				ourIns.resetInstrument()
				
				let iErr = importPAT(ourIns, inData)
				if iErr == .NoErr {
					InsHeader.memory = ourIns
				}
				return iErr
			} else {
				return .NeedMemory
			}
		} else {
			return .ReadingErr
		}
	}
}
