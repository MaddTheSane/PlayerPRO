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
import SwiftAdditions
#if os(OSX)
	import Cocoa
#else
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

public func getCommand(position: Int16, channel: Int16, aPat: PPPatternObject) -> Cmd {
	let aCmd = aPat.getCommand(position: position, channel: channel)
	return aCmd.theCommand
}

public func ReplaceCmd(position: Int16, channel: Int16, command: Cmd, aPat: PPPatternObject) {
	aPat.replaceCommand(atPosition: position, channel: channel, cmd: command)
}

public func ModifyCmdAtRow(position: Int16, channel: Int16, aPat: PPPatternObject, commandBlock: @escaping (inout Cmd)-> ()) {
	aPat.modifyCommand(atPosition: position, channel: channel, command: { (aCmd) -> Void in
		var tmpCmd = aCmd.pointee
		commandBlock(&tmpCmd)
		aCmd.pointee = tmpCmd
	})
}

public func note(from myTT: String) -> Int16? {
	if myTT == "" || myTT == "---" || myTT.characters.count < 2 {
		return nil
	}
	
	let idx = myTT.index(before: myTT.endIndex)
	let lastChar = myTT[idx]
	guard let octMaybe = Int(String(lastChar)) else {
		return nil
	}
	//	0	1	 2	 3	 4	 5	 6	 7 	 8	 9	 10	 11
	//	C-  C#   D-  D#  E-  F-  F#  G-  G#  A-  A#  B-
	
	var Oct = Int16(octMaybe)
	Oct *= 12
	
	let theRest = myTT[myTT.startIndex ..< idx]
	
	let theRet: (outString: String, sharp: Bool) = {
		let idx2 = myTT.index(before: idx)
		let maybeSign = myTT[idx2]
		let maybeStr = myTT[myTT.startIndex ..< idx2]
		switch maybeSign {
		case "#", "♯"/*Unicode sharp sign, just in case*/:
			return (maybeStr, true)
			
		case " ", "-":
			return (maybeStr, false)
			
		default:
			return (theRest, false)
		}
	}()
	
	let val1 = theRet.outString.lowercased()
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
		return nil
	}
	
	if theRet.sharp {
		Oct += 1
	}
	
	if Oct > 95 {
		return nil
	} else if Oct < 0 {
		return nil
	}
	
	return Oct
}

public func octaveName(from octNote: UInt8, letters isUseLetters: Bool = true) -> String? {
	return octaveName(from: Int16(octNote), letters: isUseLetters)
}

public func octaveName(from octNote: Int16, letters isUseLetters: Bool = true) -> String? {
	if octNote > 95 || octNote < 0 {
		return nil
	}
	
	let NNames: [String]
	
	if isUseLetters {
		NNames = ["C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "]
	} else {
		NNames = ["Do", "Do#", "Ré", "Ré#", "Mi", "Fa", "Fa#", "Sol", "Sol#", "La", "La#", "Si"]
	}
	return "\(NNames[Int(octNote % 12)])\(octNote / 12)"
}

extension PPSampleObject {

#if os(OSX)
	@objc(waveformImageUsingView:)
	final public func waveformImage(view: NSView) -> NSImage? {
		return PPSampleObject.waveFormImage(sample: self, view: view)
	}
	
	@objc(waveformImageFromSample:usingView:)
	final public class func waveFormImage(sample theDat: PPSampleObject, view: NSView) -> NSImage? {
		let imageSize: NSSize = {
			var aimageSize = view.convertToBacking(view.frame.size)
			aimageSize.height *= 2
			aimageSize.width *= 2
			
			return aimageSize
			}()
		let colorSpace = CGColorSpace(name: CGColorSpace.sRGB)!
		let datIsStereo = theDat.isStereo
		let aRect = CGRect(origin: CGPoint.zero, size: imageSize)
		let rowBytes = 4 * Int(imageSize.width)
		let bitMapFormat: CGBitmapInfo = [CGBitmapInfo.byteOrder32Host, CGBitmapInfo(alphaInfo: .premultipliedLast)]
		let bitmapContext = CGContext(data: nil, width: Int(imageSize.width), height: Int(imageSize.height), bitsPerComponent: 8, bytesPerRow: rowBytes, space: colorSpace, bitmapInfo: bitMapFormat.rawValue)!
		
		bitmapContext.clear(aRect)
		bitmapContext.setLineWidth(1)
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		if datIsStereo {
			let colorComps: [CGFloat] = [0,0,1,stereoTrans]
			let colorRef = CGColor(colorSpace: colorSpace, components: colorComps)!
			bitmapContext.setStrokeColor(colorRef)
			drawSample(rectangle: aRect, channel: 1, currentData: theDat, context: bitmapContext)
		}
		
		do {
			let colorComps: [CGFloat] = [1,0,0,stereoTrans]
			let colorRef = CGColor(colorSpace: colorSpace, components: colorComps)!
			bitmapContext.setStrokeColor(colorRef)
		}
		drawSample(rectangle: aRect, channel: 0, currentData: theDat, context: bitmapContext)
		
		if theDat.loopSize != 0 {
			let colorComps: [CGFloat] = [0.2,0.1,0.5,0.8]
			let colorRef = CGColor(colorSpace: colorSpace, components: colorComps)!
			bitmapContext.setStrokeColor(colorRef)
			let lineSize = view.convertToBacking(NSSize(width: 2, height: 2)).width * 2
			let padSize = view.convertToBacking(NSSize(width: 1, height: 1)).width * 2
			bitmapContext.setLineWidth(lineSize)
			var loopRect = aRect.insetBy(dx: padSize, dy: padSize)
			loopRect.origin.x =  CGFloat(theDat.loopBegin) * imageSize.width / CGFloat(theDat.data.count)
			loopRect.size.width = CGFloat(theDat.loopSize) * imageSize.width / CGFloat(theDat.data.count)
			bitmapContext.stroke(loopRect)
		}
		
		if let theCGimg = bitmapContext.makeImage() {
			return NSImage(cgImage: theCGimg, size: view.frame.size)
		} else {
			return nil
		}
	}
#elseif os(iOS) || os(tvOS)
	@objc(waveformImageUsingView:)
	final public func waveformImage(view: UIView) -> UIImage? {
		return PPSampleObject.waveFormImage(sample: self, view: view)
	}
	
	@objc(waveformImageFromSample:usingView:)
	final public class func waveFormImage(sample theDat: PPSampleObject, view: UIView) -> UIImage? {
		let scale = view.contentScaleFactor
		let imageSize: CGSize = {
			var viewSize = view.bounds.size
			viewSize.width *= scale
			viewSize.height *= scale
			return viewSize
			}()
		let datIsStereo = theDat.isStereo;
		let aRect = CGRect(origin: CGPoint.zero, size: imageSize)
		let rowBytes = 4 * Int(imageSize.width)
		//let bitMapFormat = CGBitmapInfo(alphaInfo: .PremultipliedLast, additionalInfo: .ByteOrder32Host)
		let bitMapFormat: CGBitmapInfo = [CGBitmapInfo.byteOrder32Host, CGBitmapInfo(alphaInfo: .premultipliedLast)]
		let colorSpace: CGColorSpace
		if #available(iOS 9.0, *) {
			colorSpace = CGColorSpace(name: CGColorSpace.sRGB)!
		} else {
			colorSpace = CGColorSpaceCreateDeviceRGB()
		}
		guard let bitmapContext = CGContext(data: nil, width: Int(imageSize.width), height: Int(imageSize.height), bitsPerComponent: 8, bytesPerRow: rowBytes, space: colorSpace, bitmapInfo: bitMapFormat.rawValue) else {
			return nil
		}
		bitmapContext.clear(aRect)
		bitmapContext.setLineWidth(1)
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		if datIsStereo {
			let colorComps: [CGFloat] = [0,0,1,stereoTrans]
			let colorRef = CGColor(colorSpace: colorSpace, components: colorComps)!
			bitmapContext.setStrokeColor(colorRef)
			drawSample(rectangle: aRect, channel: 1, currentData: theDat, context: bitmapContext)
		}
		
		do {
			let colorComps: [CGFloat] = [1,0,0,stereoTrans]
			let colorRef = CGColor(colorSpace: colorSpace, components: colorComps)!
			bitmapContext.setStrokeColor(colorRef)
		}
		drawSample(rectangle: aRect, channel: 0, currentData: theDat, context: bitmapContext)
		
		if theDat.loopSize != 0 {
			let colorComps: [CGFloat] = [0.2,0.1,0.5,0.8]
			let colorRef = CGColor(colorSpace: colorSpace, components: colorComps)!
			bitmapContext.setStrokeColor(colorRef)
			let lineSize = 2 * scale
			let padSize = 1 * scale
			bitmapContext.setLineWidth(lineSize)
			var loopRect = aRect.insetBy(dx: padSize, dy: padSize)
			loopRect.origin.x =  CGFloat(theDat.loopBegin) * imageSize.width / CGFloat(theDat.data.count)
			//loopRect.origin.y += padSize
			loopRect.size.width = CGFloat(theDat.loopSize) * imageSize.width / CGFloat(theDat.data.count)
			//loopRect.size.height -= padSize * 2
			bitmapContext.stroke(loopRect)
		}
		
		if let theCGimg = bitmapContext.makeImage() {
			return UIImage(cgImage: theCGimg, scale: scale, orientation: .up)
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

	final public class func drawSample(start start1: Int = 0, tSS: Int = 0, tSE: Int, high: Int, larg: Int, trueV: Int = 0, trueH: Int = 0, channel: Int16 = 0, currentData curData: PPSampleObject, context ctxRef: CGContext) {
		var start = start1
		ctxRef.saveGState();
		defer {
			ctxRef.restoreGState()
		}
		
		var temp: CGFloat = 0.0
		let isStereo = curData.isStereo
		var minY: CGFloat = 0.0
		var maxY: CGFloat = 0.0
		let oneShiftedBy16 = 1 / CGFloat(1 << 16)
		let oneShiftedBy8 = 1 / CGFloat(1 << 8)

		if curData.amplitude == 16 {
			let theShortSample = (curData.data as NSData).bytes.assumingMemoryBound(to: UInt16.self)
			let sampleSize = curData.data.count / 2
			start /= 2
			
			var BS = start + (tSS * sampleSize) / larg
			if isStereo {
				BS /= 2; BS *= 2;
				BS += Int(channel)
			}
			temp = CGFloat(theShortSample[BS] &+ 0x8000)
			temp *= CGFloat(high) * oneShiftedBy16
			ctxRef.move(to: CGPoint(x: CGFloat(trueH) + CGFloat(tSS), y: CGFloat(trueV) + temp))
			
			for i in tSS ..< tSE {
				BS = start + (i * sampleSize) / larg
				var BE = start + ((i + 1) * sampleSize) / larg
				
				if isStereo {
					BS /= 2; BS *= 2;
					BE /= 2; BE *= 2;
					
					BS += Int(channel)
					BE += Int(channel)
				}
				
				temp = CGFloat(theShortSample[BS] &+ 0x8000)
				minY = temp; maxY = temp;
				temp *= CGFloat(high) * oneShiftedBy16
				ctxRef.addLine(to: CGPoint(x: CGFloat(trueH + i), y: temp + CGFloat(trueV)))
				
				if BS != BE {
					var x = BS
					while x < BE {
						temp = CGFloat(theShortSample[x] &+ 0x8000)
						
						maxY = max(temp, maxY)
						minY = min(temp, minY)
						
						if isStereo {
							x += 1
						}
						x += 1
					}
					
					maxY *= CGFloat(high) * oneShiftedBy16
					minY *= CGFloat(high) * oneShiftedBy16
					
					ctxRef.move(to: CGPoint(x: CGFloat(trueH + i), y: minY + CGFloat(trueV)))
					ctxRef.addLine(to: CGPoint(x: CGFloat(trueH + i), y: maxY + CGFloat(trueV)))
				}
			}
		} else {
			let sampleSize = curData.data.count
			let theSample = (curData.data as NSData).bytes.assumingMemoryBound(to: UInt8.self)

			var BS = start + (tSS * sampleSize) / larg;
			if isStereo {
				BS /= 2; BS *= 2;
				BS += Int(channel)
			}
			
			temp = CGFloat(theSample[BS] &- 0x80)
			temp *= CGFloat(high) * oneShiftedBy8
			
			ctxRef.move(to: CGPoint(x: CGFloat(trueH + tSS), y: CGFloat(trueV) + temp))
			
			for i in tSS ..< tSE {
				BS = start + (i * sampleSize) / larg
				var BE = start + ((i + 1) * sampleSize) / larg
				
				if isStereo {
					BS /= 2; BS *= 2;
					BE /= 2; BE *= 2;
					
					BS += Int(channel);
					BE += Int(channel);
				}
				
				temp = CGFloat(theSample[BS] &- 0x80);
				minY = temp; maxY = temp;
				temp *= CGFloat(high) * oneShiftedBy8
				ctxRef.addLine(to: CGPoint(x: CGFloat(trueH + i), y: temp + CGFloat(trueV)))
				
				if BS != BE {
					var x = BS
					while x < BE {
						temp = CGFloat(theSample[x] &- 0x80)
						
						maxY = max(temp, maxY)
						minY = min(temp, minY)
						
						if isStereo {
							x += 1;
						}
						x += 1
					}
					maxY *= CGFloat(high) * oneShiftedBy8
					minY *= CGFloat(high) * oneShiftedBy8
					
					ctxRef.move(to: CGPoint(x: CGFloat(trueH) + CGFloat(i), y: minY + CGFloat(trueV)))
					ctxRef.addLine(to: CGPoint(x: CGFloat(trueH) + CGFloat(i), y: maxY + CGFloat(trueV)))
				}
			}
		}
		ctxRef.closePath();
		ctxRef.strokePath();
	}
	
	@objc(octaveNameFromNote:) final public class func octaveName(from octNote: Int16) -> String {
		return PlayerPROKit.octaveName(from: octNote) ?? "---"
	}

	@objc(octaveNameFromNote:usingSingularLetter:) final public class func octaveName(from octNote: Int16, usingSingularLetter: Bool) -> String {
		return PlayerPROKit.octaveName(from: octNote, letters: usingSingularLetter) ?? "---"
	}

	@objc(noteFromString:) final public class func noteFromString(myTT: String) -> Int16 {
		return note(from: myTT) ?? 0xFF
	}
}

extension PPPatternObject: Sequence {
	public func makeIterator() -> AnyIterator<PPMadCommandObject> {
		var index = 0
		return AnyIterator {
			if index < self.lengthOfCommands {
				let idx = self[index]
				index += 1
				return idx
			}
			
			return nil
		}
	}
}

extension PPInstrumentObject: Sequence {
	public func makeIterator() -> IndexingIterator<[PPSampleObject]> {
		return samples.makeIterator()
	}
}

extension PPDriver {
	public func playSoundData(pointer theSnd: UnsafeRawPointer, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: Int = 0, loopLength iloopLen: Int = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundData(fromPointer: theSnd, withSize: UInt(sndSize), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopStart), andLoopLength: UInt(loopLen))
	}

	public func playSoundData(pointer theSnd: UnsafeRawPointer, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundData(fromPointer: theSnd, withSize: UInt(sndSize), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopIn: loopRange)
	}
	
	public func playSoundData(pointer theSnd: UnsafeRawPointer, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: Range<Int>) -> MADErr {
		return playSoundData(pointer: theSnd, size: sndSize, channel: theChan, amplitude: amp, bitRate: rate, stereo: stereo, note: theNote, loopInRange: NSRange(loopRange))
	}
	
	public func playSoundData(data theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: Int = 0, loopLength iloopLen: Int = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if loopLen == 0 {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundData(fromPointer: theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopStart), andLoopLength: UInt(loopLen))
	}
	
	public func playSoundData(data theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundData(fromPointer: theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopIn: loopRange)
	}
	
	public func playSoundData(data theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: Range<Int>) -> MADErr {
		return playSoundData(data: theSnd, channel: theChan, amplitude: amp, bitRate: rate, stereo: stereo, note: theNote, loopInRange: NSRange(loopRange))
	}
	
	///Returns nil on error.
	///The time values are in 1/60th of a second.
	public var musicStatusTime: (current: Int, total: Int)? {
		var cT = 0
		var tT = 0
		let anErr = getMusicStatus(withCurrentTime: &cT, totalTime: &tT)
		if anErr == .noErr {
			return (cT, tT)
		} else {
			return nil
		}
	}
	
	public var oscilloscope: (size: size_t, pointer: UnsafeRawPointer?) {
		return (oscilloscopeSize, UnsafeRawPointer(oscilloscopePointer))
	}
}
