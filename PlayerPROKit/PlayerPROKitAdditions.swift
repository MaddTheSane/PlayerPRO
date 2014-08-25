//
//  OSX-Specific-Extensions.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Foundation
import PlayerPROCore
import CoreGraphics

public func CreateErrorFromMADErrorType(theErr: MADErr) -> NSError? {
	return PPCreateErrorFromMADErrorType(theErr)
}

extension PPSampleObject {

#if os(OSX)
	public class func waveformImage(fromSample theDat: PPSampleObject, view: NSView) -> NSImage {
		var imageSize = view.convertSizeToBacking(view.frame.size)
		let datIsStereo = theDat.stereo;
		imageSize.height *= 2;
		imageSize.width *= 2;
		let aRect = CGRect(origin: CGPointZero, size: imageSize)
		let rowBytes: UInt = 4 * UInt(imageSize.width)
		let defaultSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB)!
		let bitMapFormat = CGBitmapInfo.fromRaw(CGImageAlphaInfo.PremultipliedLast.toRaw())!
		let bitmapContext = CGBitmapContextCreate(nil, UInt(imageSize.width), UInt(imageSize.height), 8, rowBytes, defaultSpace, bitMapFormat);
		CGContextClearRect(bitmapContext, CGRectMake(0, 0, imageSize.width, imageSize.height));
		let lineSize = view.convertSizeToBacking(NSSize(width: 1, height: 1));
		CGContextSetLineWidth(bitmapContext, lineSize.height);
		var colorRef = CGColorCreateGenericGray(0, 0)
		if (datIsStereo) {
			colorRef = CGColorCreateGenericRGB(0, 0, 1, 0.75);
			CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
			drawSample(rectangle: aRect, channel: 1, currentData: theDat, context: bitmapContext)
		}
		let stereoTrans: CGFloat = datIsStereo ? 0.75 : 1
		
		colorRef = CGColorCreateGenericRGB(1, 0, 0, stereoTrans);
		CGContextSetStrokeColorWithColor(bitmapContext, colorRef);
		drawSample(rectangle: aRect, channel: 0, currentData: theDat, context: bitmapContext)
		
		if (theDat.loopSize != 0) {
			colorRef = CGColorCreateGenericRGB(1, 0.1, 0.5, 0.8)
			CGContextSetStrokeColorWithColor(bitmapContext, colorRef)
			var loopRect = aRect
			var lineSize = view.convertSizeToBacking(NSSize(width: 2, height: 2))
			var padSize = view.convertSizeToBacking(NSSize(width: 1, height: 1))
			CGContextSetLineWidth(bitmapContext, lineSize.height)
			loopRect.origin.x =  CGFloat(theDat.loopBegin) * imageSize.width / CGFloat(theDat.data.length)
			loopRect.origin.y += padSize.width
			loopRect.size.width = CGFloat(theDat.loopSize) * imageSize.width / CGFloat(theDat.data.length)
			loopRect.size.height -= padSize.width * 2
			CGContextStrokeRect(bitmapContext, loopRect);
		}
		
		var theCGimg = CGBitmapContextCreateImage(bitmapContext);
		
		return NSImage(CGImage: theCGimg, size: view.frame.size)
	}
#endif
	
	public class func drawSample(start: Int = 0, tSS: Int = 0, rectangle rect:CGRect, channel: Int16 = 0, currentData curData: PPSampleObject, context ctxRef: CGContext) {
		return drawSample(start: start, tSS: tSS, tSE: Int(rect.size.width), high: Int(rect.size.height), larg: Int(rect.size.width), trueV: Int(rect.origin.x), trueH: Int(rect.origin.y), channel: channel, currentData: curData, context:ctxRef)
	}

	public class func drawSample(start startI: Int = 0, tSS: Int = 0, tSE: Int, high: Int, larg: Int, trueV: Int = 0, trueH: Int = 0, channel: Int16 = 0, currentData curData: PPSampleObject, context ctxRef: CGContext) {
		CGContextSaveGState(ctxRef);
		
		var start = startI
		var i = 0;
		var sampleSize = curData.data.length;
		var temp: CGFloat = 0.0
		let theSample = UnsafePointer<UInt8>(curData.data.bytes)
		let theShortSample = UnsafePointer<UInt16>(curData.data.bytes)
		var BS = 0
		var BE = 0
		var x = 0
		let isStereo = curData.stereo;
		var minY: CGFloat = 0.0
		var maxY: CGFloat = 0.0
		let oneShiftedBy16 = CGFloat(1 << 16)
		let oneShiftedBy8 = CGFloat(1 << 8)

		if (curData.amplitude == 16) {
			sampleSize /= 2
			start /= 2
			
			BS = start + (tSS * sampleSize) / larg;
			if (isStereo) {
				BS /= 2;
				BS *= 2;
				BS += Int(channel);
			}
			temp = CGFloat(Int(theShortSample[BS]) + 0x8000);
			temp *= CGFloat(high);
			temp /= oneShiftedBy16;
			CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(tSS), CGFloat(trueV) + temp);
			
			for (i = tSS; i < tSE; i++) {
				BS = start + (i * sampleSize) / larg;
				BE = start + ((i + 1) * sampleSize) / larg;
				
				if (isStereo) {
					BS /= 2;
					BS *= 2;
					BE /= 2;
					BE *= 2;
					
					BS += Int(channel);
					BE += Int(channel);
				}
				
				temp = CGFloat(Int(theShortSample[BS]) + 0x8000);
				minY = CGFloat(temp);
				maxY = CGFloat(temp);
				temp *= CGFloat(high);
				temp /= oneShiftedBy16;
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), temp + CGFloat(trueV));
				
				if (BS != BE) {
					for (x = BS; x < BE; x++) {
						temp = CGFloat(Int(theShortSample[x]) + 0x8000);
						
						if (temp > maxY) {
							maxY = temp;
						}
						
						if (temp < minY) {
							minY = temp;
						}
						
						if (isStereo) {
							x++;
						}
					}
					
					maxY *= CGFloat(high);
					maxY /= oneShiftedBy16
					minY *= CGFloat(high);
					minY /= oneShiftedBy16
					
					CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), minY + CGFloat(trueV));
					CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), maxY + CGFloat(trueV));
				}
			}
		} else {
			BS = start + (tSS * sampleSize) / larg;
			if (isStereo) {
				BS /= 2;
				BS *= 2;
				BS += Int(channel);
			}
			
			temp = CGFloat((theSample[BS] - 0x80));
			temp *= CGFloat(high);
			temp /= oneShiftedBy8;
			
			CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(tSS), CGFloat(trueV) + temp);
			
			for (i = tSS; i < tSE; i++) {
				BS = start + (i * sampleSize) / larg;
				BE = start + ((i + 1) * sampleSize) / larg;
				
				if (isStereo) {
					BS /= 2;
					BS *= 2;
					
					BE /= 2;
					BE *= 2;
					
					BS += Int(channel);
					BE += Int(channel);
				}
				
				temp = CGFloat(theSample[BS] - 0x80);
				minY = temp; maxY = temp;
				temp *= CGFloat(high);
				temp /= oneShiftedBy8;
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), temp + CGFloat(trueV));
				
				if (BS != BE) {
					for (x = BS; x < BE; x++) {
						temp = CGFloat(theSample[x] - 0x80);
						
						if (temp > maxY) {
							maxY = temp;
						}
						if (temp < minY) {
							minY = temp;
						}
						
						if (isStereo) {
							x++;
						}
					}
					maxY *= CGFloat(high);
					maxY /= oneShiftedBy8;
					minY *= CGFloat(high);
					minY /= oneShiftedBy8;
					
					CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), minY + CGFloat(trueV));
					CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), maxY + CGFloat(trueV));
				}
			}
		}
		CGContextStrokePath(ctxRef);
		CGContextRestoreGState(ctxRef);
	}
	
	//public class func octaveNameFromNote(octNote: Int8 /*, usingLetters isUseLetters: Bool = true*/) -> String {
	//	return self.octaveNameFromNote(Int16(octNote) /*, usingLetters: isUseLetters*/)
	//}
}

extension PPDriver {
	public func playSoundData(fromPointer theSnd: UnsafePointer<()>, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: Int = 0, loopLength iloopLen: Int = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundDataFromPointer(theSnd, withSize: UInt(sndSize), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopStart), andLoopLength: UInt(loopLen))
	}

	public func playSoundData(fromPointer theSnd: UnsafePointer<()>, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundDataFromPointer(theSnd, withSize: UInt(sndSize), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopRange.location), andLoopLength: UInt(loopRange.length))
	}
	
	public func playSoundData(fromPointer theSnd: UnsafePointer<()>, size sndSize: Int, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: Range<Int>) -> MADErr {
		return playSoundData(fromPointer: theSnd, size: sndSize, channel: theChan, amplitude: amp, bitRate: rate, stereo: stereo, note: theNote, loopInRange: NSRange(loopRange))
	}
	
	public func playSoundData(fromData theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: Int = 0, loopLength iloopLen: Int = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundDataFromPointer(theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopStart), andLoopLength: UInt(loopLen))
	}
	
	public func playSoundData(fromData theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundDataFromPointer(theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopRange.location), andLoopLength: UInt(loopRange.length))
	}
	
	public func playSoundData(fromData theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: Range<Int>) -> MADErr {
		return playSoundData(fromData: theSnd, channel: theChan, amplitude: amp, bitRate: rate, stereo: stereo, note: theNote, loopInRange: NSRange(loopRange))
	}
}
