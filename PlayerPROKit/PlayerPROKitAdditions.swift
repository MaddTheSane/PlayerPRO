//
//  OSX-Specific-Extensions.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import CoreFoundation
import Foundation
import PlayerPROCore
import CoreGraphics
#if os(OSX)
	import Cocoa
#elseif os(iOS)
	import UIKit
#endif

public func ==(lhs: PPMadCommandObject, rhs: Cmd) -> Bool {
	return lhs.theCommand == rhs
}

public func ==(lhs: PPEnvelopeObject, rhs: EnvRec) -> Bool {
	return lhs.envelopeRec == rhs
}

public func ==(lhs: PPFXBusObject, rhs: FXBus) -> Bool {
	return lhs.theBus == rhs
}

public func ==(lhs: PPFXSetObject, rhs: FXSets) -> Bool {
	return lhs.theSet == rhs
}

public func GetCommand(position: Int16, channel: Int16, aPat: PPPatternObject) -> Cmd {
	var aCmd = aPat.getCommandFromPosition(position, channel: channel)
	return aCmd.theCommand
}

public func ReplaceCmd(position: Int16, channel: Int16, command: Cmd, aPat: PPPatternObject) {
	aPat.replaceCommandAtPosition(position, channel: channel, cmd: command)
}

public func ModifyCmdAtRow(position: Int16, channel: Int16, aPat: PPPatternObject, commandBlock: (inout Cmd)-> ()) {
	aPat.modifyCommandAtPosition(position, channel: channel, commandBlock: { (aCmd) -> Void in
		var tmpCmd = aCmd.memory
		commandBlock(&tmpCmd)
		aCmd.memory = tmpCmd
	})
}

public func CreateErrorFromMADErrorType(theErr: MADErr) -> NSError? {
	return PPCreateErrorFromMADErrorType(theErr)
}

public func NoteFromString(myTT: String) -> Int16?
{
	if ( myTT == "" || myTT == "---" || countElements(myTT) < 2) {
		return nil
	}
	
	func findNote(inStr: String) -> (outString: String, sharp: Bool) {
		var idx = inStr.endIndex
		--idx
		var maybeSign = inStr[idx]
		var maybeStr = inStr[inStr.startIndex ..< idx]
		switch maybeSign {
		case "#", "♯"/*Unicode sharp sign, just in case*/:
			return (maybeStr, true)

		case " ", "-":
			return (maybeStr, false)
			
		default:
			return (inStr, false)
		}
	}
	
	var idx = myTT.endIndex
	let lastChar = myTT[--idx]
	let octMaybe = String(lastChar).toInt()
	if octMaybe == nil {
		return nil
	}
	
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-

	var Oct = Int16(octMaybe!)
	Oct *= 12
	
	let theRest = myTT[myTT.startIndex ..< idx]
	let theRet = findNote(theRest)
	let val1 = theRet.outString.lowercaseString
	switch val1 {
	case "c", "do":
		Oct += 0
		
	case "d", "ré", "re":
		Oct += 2
		
	case "e", "mi":
		Oct += 4
		
	case "f", "fa":
		Oct += 5
		
	case "g", "sol", "so":
		Oct += 7
		
	case "a", "la":
		Oct += 9
		
	case "b", "si", "ti":
		Oct += 11
		
	default:
		Oct = 0xFF
	}
	
	if Oct != 0xFF {
		if theRet.sharp {
			Oct++
		}
		
		if Oct > 95 {
			Oct = 0xFF
		}
		if Oct < 0 {
			Oct = 0xFF
		}
	}
	
	if Oct == 0xFF {
		return nil
	} else {
		return Oct
	}
}

public func OctaveNameFromNote(octNote: UInt8, letters isUseLetters: Bool = true) -> String? {
	return OctaveNameFromNote(Int16(octNote), letters: isUseLetters)
}

public func OctaveNameFromNote(octNote: Int16, letters isUseLetters: Bool = true) -> String? {
	if (octNote > 95 || octNote < 0) {
		return nil
	}
	
	if isUseLetters {
		let NNames = ["C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "]
		
		return "\(NNames[Int(octNote % 12)])\(octNote / 12)"
	} else {
		let NNames_nonEnglish = ["Do", "Do#", "Ré", "Ré#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"]
	
		return "\(NNames_nonEnglish[Int(octNote % 12)])\(octNote / 12)"
	}
}

extension PPSampleObject {

#if os(OSX)
	@objc(waveformImageUsingView:) final public func waveformImage(#view: NSView) -> NSImage? {
		return PPSampleObject.waveformImage(sample: self, view: view)
	}
	
	@objc(waveformImageFromSample:usingView:) final public class func waveformImage(sample theDat: PPSampleObject, view: NSView) -> NSImage? {
		func viewSize() -> NSSize {
			var imageSize = view.convertSizeToBacking(view.frame.size)
			imageSize.height *= 2
			imageSize.width *= 2
			
			return imageSize
		}
		let imageSize = viewSize()
		let datIsStereo = theDat.stereo
		let aRect = CGRect(origin: CGPoint.zeroPoint, size: imageSize)
		let rowBytes = 4 * UInt(imageSize.width)
		let bitMapFormat = CGBitmapInfo(alphaInfo: .PremultipliedLast, additionalInfo: .ByteOrderDefault)
		let bitmapContext = CGBitmapContextCreate(nil, UInt(imageSize.width), UInt(imageSize.height), 8, rowBytes, CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB)!, bitMapFormat)!
		
		CGContextClearRect(bitmapContext, aRect)
		let lineSize = view.convertSizeToBacking(NSSize(width: 1, height: 1)).height
		CGContextSetLineWidth(bitmapContext, lineSize)
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		if (datIsStereo) {
			CGContextSetStrokeColorWithColor(bitmapContext, CGColorCreateGenericRGB(0, 0, 1, stereoTrans))
			drawSample(rectangle: aRect, channel: 1, currentData: theDat, context: bitmapContext)
		}
		
		CGContextSetStrokeColorWithColor(bitmapContext, CGColorCreateGenericRGB(1, 0, 0, stereoTrans));
		drawSample(rectangle: aRect, channel: 0, currentData: theDat, context: bitmapContext)
		
		if (theDat.loopSize != 0) {
			CGContextSetStrokeColorWithColor(bitmapContext, CGColorCreateGenericRGB(1, 0.1, 0.5, 0.8))
			var loopRect = aRect
			let lineSize = view.convertSizeToBacking(NSSize(width: 2, height: 2)).width
			let padSize = view.convertSizeToBacking(NSSize(width: 1, height: 1)).width
			CGContextSetLineWidth(bitmapContext, lineSize)
			loopRect.origin.x =  CGFloat(theDat.loopBegin) * imageSize.width / CGFloat(theDat.data.length)
			loopRect.origin.y += padSize
			loopRect.size.width = CGFloat(theDat.loopSize) * imageSize.width / CGFloat(theDat.data.length)
			loopRect.size.height -= padSize * 2
			CGContextStrokeRect(bitmapContext, loopRect)
		}
		
		if let theCGimg = CGBitmapContextCreateImage(bitmapContext) {
			return NSImage(CGImage: theCGimg, size: view.frame.size)
		} else {
			return nil
		}
	}
#elseif os(iOS)
	@objc(waveformImageUsingView:) final public func waveformImage(#view: UIView) -> UIImage? {
		return PPSampleObject.waveformImage(sample: self, view: view)
	}
	
	@objc(waveformImageFromSample:usingView:) final public class func waveformImage(sample theDat: PPSampleObject, view: UIView) -> UIImage? {
		func viewSize() -> CGSize {
			var imageSize = view.bounds.size
			let scale = view.contentScaleFactor
			imageSize.width *= scale
			imageSize.height *= scale
			return imageSize
		}
		let imageSize = viewSize()
		let scale = view.contentScaleFactor
		let datIsStereo = theDat.stereo;
		let aRect = CGRect(origin: CGPoint.zeroPoint, size: imageSize)
		let rowBytes = 4 * UInt(imageSize.width)
		let bitMapFormat = CGBitmapInfo(alphaInfo: .PremultipliedLast, additionalInfo: .ByteOrderDefault)
		let bitmapContext = CGBitmapContextCreate(nil, UInt(imageSize.width), UInt(imageSize.height), 8, rowBytes, CGColorSpaceCreateDeviceRGB(), bitMapFormat)
		CGContextClearRect(bitmapContext, aRect)
		let lineSize = 1 * scale
		CGContextSetLineWidth(bitmapContext, lineSize)
		var colorRef: UIColor
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		if (datIsStereo) {
			colorRef = UIColor(red: 0, green: 0, blue: 1, alpha: stereoTrans)
			CGContextSetStrokeColorWithColor(bitmapContext, colorRef.CGColor)
			drawSample(rectangle: aRect, channel: 1, currentData: theDat, context: bitmapContext)
		}
		
		colorRef = UIColor(red: 1, green: 0, blue: 0, alpha: stereoTrans)
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef.CGColor)
		drawSample(rectangle: aRect, channel: 0, currentData: theDat, context: bitmapContext)
		
		if (theDat.loopSize != 0) {
			colorRef = UIColor(red: 1, green: 0.1, blue: 0.5, alpha: 0.8)
			CGContextSetStrokeColorWithColor(bitmapContext, colorRef.CGColor)
			var loopRect = aRect
			let lineSize = 2 * scale
			let padSize = 1 * scale
			CGContextSetLineWidth(bitmapContext, lineSize)
			loopRect.origin.x =  CGFloat(theDat.loopBegin) * imageSize.width / CGFloat(theDat.data.length)
			loopRect.origin.y += padSize
			loopRect.size.width = CGFloat(theDat.loopSize) * imageSize.width / CGFloat(theDat.data.length)
			loopRect.size.height -= padSize * 2
			CGContextStrokeRect(bitmapContext, loopRect)
		}
		
		if let theCGimg = CGBitmapContextCreateImage(bitmapContext) {
			return UIImage(CGImage: theCGimg, scale: scale, orientation: .Up)
		} else {
			return nil
		}
	}
#endif
	
	final public func drawSample(start: Int = 0, tSS: Int = 0, rectangle rect: CGRect, channel: Int16 = 0, context ctxRef: CGContext) {
		PPSampleObject.drawSample(start: start, tSS: tSS, tSE: Int(rect.size.width), high: Int(rect.size.height), larg: Int(rect.size.width), trueV: Int(rect.origin.x), trueH: Int(rect.origin.y), channel: channel, currentData: self, context:ctxRef)
	}
	
	final public func drawSample(start startI: Int = 0, tSS: Int = 0, tSE: Int, high: Int, larg: Int, trueV: Int = 0, trueH: Int = 0, channel: Int16 = 0, context ctxRef: CGContext) {
		PPSampleObject.drawSample(start: startI, tSS: tSS, tSE: tSE, high: high, larg: larg, trueV: trueV, trueH: trueH, channel: channel, currentData: self, context: ctxRef)
	}
	
	final public class func drawSample(start: Int = 0, tSS: Int = 0, rectangle rect: CGRect, channel: Int16 = 0, currentData curData: PPSampleObject, context ctxRef: CGContext) {
		drawSample(start: start, tSS: tSS, tSE: Int(rect.size.width), high: Int(rect.size.height), larg: Int(rect.size.width), trueV: Int(rect.origin.x), trueH: Int(rect.origin.y), channel: channel, currentData: curData, context:ctxRef)
	}

	final public class func drawSample(start startI: Int = 0, tSS: Int = 0, tSE: Int, high: Int, larg: Int, trueV: Int = 0, trueH: Int = 0, channel: Int16 = 0, currentData curData: PPSampleObject, context ctxRef: CGContext) {
		CGContextSaveGState(ctxRef);
		
		var temp: CGFloat = 0.0
		let isStereo = curData.stereo
		var minY: CGFloat = 0.0
		var maxY: CGFloat = 0.0
		let oneShiftedBy16 = CGFloat(1 << 16)
		let oneShiftedBy8 = CGFloat(1 << 8)

		if (curData.amplitude == 16) {
			let theShortSample = UnsafePointer<UInt16>(curData.data.bytes)
			let sampleSize = curData.data.length / 2
			let start = startI / 2
			
			var BS = start + (tSS * sampleSize) / larg
			if (isStereo) {
				BS /= 2; BS *= 2;
				BS += Int(channel)
			}
			temp = CGFloat(theShortSample[BS] &+ 0x8000)
			temp *= CGFloat(high);
			temp /= oneShiftedBy16;
			CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(tSS), CGFloat(trueV) + temp)
			
			for i in tSS ..< tSE {
				BS = start + (i * sampleSize) / larg
				var BE = start + ((i + 1) * sampleSize) / larg
				
				if (isStereo) {
					BS /= 2; BS *= 2;
					BE /= 2; BE *= 2;
					
					BS += Int(channel)
					BE += Int(channel)
				}
				
				temp = CGFloat(theShortSample[BS] &+ 0x8000)
				minY = temp; maxY = temp;
				temp *= CGFloat(high)
				temp /= oneShiftedBy16
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), temp + CGFloat(trueV))
				
				if (BS != BE) {
					for (var x = BS; x < BE; x++) {
						temp = CGFloat(theShortSample[x] &+ 0x8000)
						
						if (temp > maxY) {
							maxY = temp
						}
						
						if (temp < minY) {
							minY = temp
						}
						
						if (isStereo) {
							x++
						}
					}
					
					maxY *= CGFloat(high)
					maxY /= oneShiftedBy16
					minY *= CGFloat(high)
					minY /= oneShiftedBy16
					
					CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), minY + CGFloat(trueV))
					CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), maxY + CGFloat(trueV))
				}
			}
		} else {
			let start = startI
			let sampleSize = curData.data.length
			let theSample = UnsafePointer<UInt8>(curData.data.bytes)

			var BS = start + (tSS * sampleSize) / larg;
			if (isStereo) {
				BS /= 2; BS *= 2;
				BS += Int(channel)
			}
			
			temp = CGFloat(theSample[BS] &- 0x80)
			temp *= CGFloat(high)
			temp /= oneShiftedBy8
			
			CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(tSS), CGFloat(trueV) + temp)
			
			for i in tSS ..< tSE {
				BS = start + (i * sampleSize) / larg
				var BE = start + ((i + 1) * sampleSize) / larg
				
				if (isStereo) {
					BS /= 2; BS *= 2;
					BE /= 2; BE *= 2;
					
					BS += Int(channel);
					BE += Int(channel);
				}
				
				temp = CGFloat(theSample[BS] &- 0x80);
				minY = temp; maxY = temp;
				temp *= CGFloat(high)
				temp /= oneShiftedBy8
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), temp + CGFloat(trueV))
				
				if (BS != BE) {
					for (var x = BS; x < BE; x++) {
						temp = CGFloat(theSample[x] &- 0x80)
						
						if (temp > maxY) {
							maxY = temp
						}
						if (temp < minY) {
							minY = temp
						}
						
						if (isStereo) {
							x++;
						}
					}
					maxY *= CGFloat(high);
					maxY /= oneShiftedBy8;
					minY *= CGFloat(high);
					minY /= oneShiftedBy8;
					
					CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), minY + CGFloat(trueV))
					CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), maxY + CGFloat(trueV))
				}
			}
		}
		CGContextStrokePath(ctxRef);
		CGContextRestoreGState(ctxRef);
	}
	
	@objc final public class func octaveNameFromNote(octNote: Int16) -> String {
		return OctaveNameFromNote(octNote) ?? "---"
	}

	@objc final public class func octaveNameFromNote(octNote: Int16, usingSingularLetter: Bool) -> String {
		return OctaveNameFromNote(octNote, letters: usingSingularLetter) ?? "---"
	}

	@objc final public class func noteFromString(myTT: String) -> Int16 {
		return NoteFromString(myTT) ?? 0xFF
	}
}

extension PPPatternObject: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self)
	}
}

public func InfoRecToMusicInfo(infoRec: MADInfoRec) -> PPLibrary.MusicFileInfo {
	let tmpDict = PPInfoRecToDictionary(infoRec)
	
	return PPLibrary.MusicFileInfo(infoDict: tmpDict)
}

extension PPLibrary: SequenceType {
	public func generate() -> NSFastGenerator {
		return NSFastGenerator(self)
	}
	
	public struct MusicFileInfo {
		public var totalPatterns: Int
		public var partitionLength: Int
		public var fileSize: Int
		public var totalTracks: Int
		public var totalInstruments: Int
		public var internalFileName: String
		public var formatDescription: String
		public var signature: String

		private init(infoDict: NSDictionary) {
			totalPatterns = infoDict[kPPTotalPatterns] as NSNumber as Int
			partitionLength = infoDict[kPPPartitionLength] as NSNumber as Int
			fileSize = infoDict[kPPFileSize] as NSNumber as Int
			totalTracks = infoDict[kPPTotalTracks] as NSNumber as Int
			totalInstruments = infoDict[kPPTotalInstruments] as NSNumber as Int
			internalFileName = infoDict[kPPInternalFileName] as NSString
			formatDescription = infoDict[kPPFormatDescription] as NSString
			signature = infoDict[kPPSignature] as NSString
		}
	}
	
	public func identifyFile(#URL: NSURL) -> (error: MADErr, format: String?) {
		var ourStrType: NSString? = nil
		let toRet = identifyFileAtURL(URL, stringType: &ourStrType)
		
		return (toRet, ourStrType)
	}
	
	public func identifyFile(#path: String) -> (error: MADErr, format: String?) {
		var ourStrType: NSString? = nil
		let toRet = identifyFileAtPath(path, stringType: &ourStrType)
		
		return (toRet, ourStrType)
	}
	
	public func getInformationFromFile(#URL: NSURL, type stringType: String) -> (error: MADErr, musicInfo: MusicFileInfo?) {
		var ourDict: NSDictionary? = nil
		let toRet = getInformationFromFileAtURL(URL, stringType: stringType, infoDictionary: &ourDict)
		if let unwrapped = ourDict {
			let inf = MusicFileInfo(infoDict: unwrapped)
			return (toRet, inf)
		} else {
			return (toRet, nil)
		}
	}
	
	public func getInformationFromFile(#path: String, type stringType: String) -> (error: MADErr, musicInfo: MusicFileInfo?) {
		var ourDict: NSDictionary? = nil
		let toRet = getInformationFromFileAtPath(path, stringType: stringType, infoDictionary: &ourDict)
		if let unwrapped = ourDict {
			let inf = MusicFileInfo(infoDict: unwrapped)
			return (toRet, inf)
		} else {
			return (toRet, nil)
		}
	}
}

extension PPInstrumentObject: SequenceType {
	public func generate() -> IndexingGenerator<[PPSampleObject]> {
		return (samples as [PPSampleObject]).generate()
	}
}

extension PPDriver {
	public func playSoundData(pointer theSnd: UnsafePointer<()>, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: Int = 0, loopLength iloopLen: Int = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundDataFromPointer(theSnd, withSize: UInt(sndSize), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopStart), andLoopLength: UInt(loopLen))
	}

	public func playSoundData(pointer theSnd: UnsafePointer<()>, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundDataFromPointer(theSnd, withSize: UInt(sndSize), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopInRange: loopRange)
	}
	
	public func playSoundData(pointer theSnd: UnsafePointer<()>, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: Range<Int>) -> MADErr {
		return playSoundData(pointer: theSnd, size: sndSize, channel: theChan, amplitude: amp, bitRate: rate, stereo: stereo, note: theNote, loopInRange: NSRange(loopRange))
	}
	
	public func playSoundData(data theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: Int = 0, loopLength iloopLen: Int = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundDataFromPointer(theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopStart), andLoopLength: UInt(loopLen))
	}
	
	public func playSoundData(data theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundDataFromPointer(theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopInRange: loopRange)
	}
	
	public func playSoundData(data theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: Range<Int>) -> MADErr {
		return playSoundData(data: theSnd, channel: theChan, amplitude: amp, bitRate: rate, stereo: stereo, note: theNote, loopInRange: NSRange(loopRange))
	}
	
	public var oscilloscope: (size: size_t, pointer: UnsafePointer<Void>) {
		return (oscilloscopeSize, UnsafePointer<Void>(oscilloscopePointer))
	}
}
