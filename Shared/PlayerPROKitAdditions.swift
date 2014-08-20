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

extension PPSampleObject {
public class func drawSample(start startI: Int = 0, tSS: Int = 0, tSE: Int, high: Int, larg: Int, trueV: Int = 0, trueH: Int = 0, channel: Int16 = 0, currentData curData: PPSampleObject!, context ctxRef: CGContextRef!) {
	
	CGContextSaveGState(ctxRef);
	
	var start = startI
	var i = 0;
	var sampleSize = curData.data.length;
	var	temp: CGFloat = 0.0
	var theSample = UnsafePointer<UInt8>(curData.data.bytes)
	var theShortSample = UnsafePointer<UInt16>(curData.data.bytes)
	var BS = 0
	var BE = 0
	var x = 0
	let isStereo = curData.stereo;
	var minY: CGFloat = 0.0
	var maxY: CGFloat = 0.0
	
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
		temp  /= CGFloat((1 << 16));
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
			temp /= CGFloat((1 << 16));
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
				maxY /= CGFloat((1 << 16));
				minY *= CGFloat(high);
				minY /= CGFloat((1 << 16));
				
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
		temp /= CGFloat((1 << 8));
		
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
			temp /= CGFloat(1 << 8);
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
				maxY /= CGFloat(1 << 8);
				minY *= CGFloat(high);
				minY /= CGFloat(1 << 8);
				
				CGContextMoveToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), minY + CGFloat(trueV));
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH) + CGFloat(i), maxY + CGFloat(trueV));
			}
		}
	}
	CGContextStrokePath(ctxRef);
	CGContextRestoreGState(ctxRef);
}
}

extension PPDriver {
	public func playSoundData(FromPointer theSnd: UnsafePointer<()>, size sndSize: UInt, fromChannel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: UInt = 0, loopLength iloopLen: UInt = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundDataFromPointer(theSnd, withSize: sndSize, fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: loopStart, andLoopLength: loopLen)
	}

	public func playSoundData(FromPointer theSnd: UnsafePointer<()>, size sndSize: UInt, fromChannel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundDataFromPointer(theSnd, withSize: sndSize, fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopRange.location), andLoopLength: UInt(loopRange.length))
	}
	
	public func playSoundData(fromData theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopStartingAt iloopStart: UInt = 0, loopLength iloopLen: UInt = 0) -> MADErr {
		var loopStart = iloopStart
		var loopLen = iloopLen
		if (loopLen == 0) {
			loopStart = 0
			loopLen = 0
		}
		return self.playSoundDataFromPointer(theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: loopStart, andLoopLength: loopLen)
	}
	
	public func playSoundData(fromData theSnd: NSData, channel theChan: Int32, amplitude amp: Int16, bitRate rate: UInt32, stereo: Bool, note theNote: MADByte = 0xFF, loopInRange loopRange: NSRange) -> MADErr {
		return self.playSoundDataFromPointer(theSnd.bytes, withSize: UInt(theSnd.length), fromChannel: theChan, amplitude: amp, bitRate: rate, isStereo: stereo, withNote: theNote, withLoopStartingAt: UInt(loopRange.location), andLoopLength: UInt(loopRange.length))
	}
}
