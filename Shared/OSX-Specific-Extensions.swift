//
//  OSX-Specific-Extensions.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/11/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit
import CoreGraphics

extension PPSampleObject {
class func drawCGSampleInt(start startI: Int, tSS: Int, tSE: Int, high: Int, larg: Int, trueV: Int, trueH: Int, channel: Int16, curData: PPSampleObject!, ctxRef: CGContextRef!) {
	CGContextSaveGState(ctxRef);
	
	var start = startI
	
	var i: Int = 0;
	var sampleSize = curData.data.length;
	var	temp: CGFloat = 0.0
	var theSample = UnsafePointer<UInt8>(curData.data.bytes)
	var theShortSample = UnsafePointer<UInt16>(curData.data.bytes)
	var BS: Int = 0
	var BE: Int = 0
	var x: Int = 0
	var isStereo = curData.stereo;
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
		CGContextMoveToPoint(ctxRef, CGFloat(trueH + tSS), trueV + temp);
		
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
			CGContextAddLineToPoint(ctxRef, CGFloat(trueH + i), CGFloat(temp + trueV));
			
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
				
				CGContextMoveToPoint(ctxRef, CGFloat(Double(trueH + i)), CGFloat(Double(minY + trueV)));
				CGContextAddLineToPoint(ctxRef, CGFloat(Double(trueH + i)), CGFloat(Double(maxY + trueV)));
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
		
		CGContextMoveToPoint(ctxRef, CGFloat(trueH + tSS), trueV + temp);
		
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
			CGContextAddLineToPoint(ctxRef, CGFloat(trueH + i), temp + trueV);
			
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
				
				CGContextMoveToPoint(ctxRef, CGFloat(trueH + i), minY + trueV);
				CGContextAddLineToPoint(ctxRef, CGFloat(trueH + i), maxY + trueV);
			}
		}
	}
	CGContextStrokePath(ctxRef);
	CGContextRestoreGState(ctxRef);
}
}
