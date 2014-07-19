//
//  PPByteSwap.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/18/14.
//
//

import Swift

extension MADSpec {
	var bigEndian: MADSpec { get {
		var toRet = self
		toRet.MAD = self.MAD.bigEndian;
		toRet.speed = self.speed.bigEndian;
		toRet.tempo = self.tempo.bigEndian;
		toRet.EPitch = self.EPitch.bigEndian;
		toRet.ESpeed = self.ESpeed.bigEndian;
		
		return toRet
	}}
}

/*
extension FXSets {
	var bigEndian: FXSets {get {
		var toRet = self
		toRet.FXID = self.FXID.bigEndian
		toRet.id = self.id.bigEndian
		toRet.noArg = self.noArg.bigEndian
		toRet.track = self.track.bigEndian
		let array: Array<Float> = [0,0,0]
		
		return toRet
	}}
}
*/

extension FXBus {
	var bigEndian: FXBus {get {
		var toRet = FXBus(ByPass: self.ByPass, copyId: self.copyId.bigEndian, Active: self.Active)
		
		return toRet
	}}
}

extension PatHeader {
	var bigEndian: PatHeader {get {
		var toRet = self
		toRet.size = self.size.bigEndian
		toRet.compMode = self.compMode.bigEndian
		toRet.patBytes = self.patBytes.bigEndian
		
		return toRet
	}}
}

extension EnvRec {
	var bigEndian: EnvRec {get {
		var toRet = EnvRec(pos: self.pos.bigEndian, val: self.val.bigEndian)
		
		return toRet
	}}
}

extension InstrData {
	var bigEndian: InstrData {get {
		var toRet = self;
		toRet.numSamples = self.numSamples.bigEndian
		toRet.firstSample = self.firstSample.bigEndian
		toRet.volFade = self.volFade.bigEndian
		
		toRet.MIDI = self.MIDI.bigEndian
		toRet.MIDIType = self.MIDIType.bigEndian
		
		toRet.volEnv = (self.volEnv.0.bigEndian, self.volEnv.1.bigEndian,
			self.volEnv.2.bigEndian, self.volEnv.3.bigEndian, self.volEnv.4.bigEndian,
			self.volEnv.5.bigEndian, self.volEnv.6.bigEndian, self.volEnv.7.bigEndian,
			self.volEnv.8.bigEndian, self.volEnv.9.bigEndian, self.volEnv.10.bigEndian,
			self.volEnv.11.bigEndian)
		
		toRet.pannEnv = (self.pannEnv.0.bigEndian, self.pannEnv.1.bigEndian,
			self.pannEnv.2.bigEndian, self.pannEnv.3.bigEndian, self.pannEnv.4.bigEndian,
			self.pannEnv.5.bigEndian, self.pannEnv.6.bigEndian, self.pannEnv.7.bigEndian,
			self.pannEnv.8.bigEndian, self.pannEnv.9.bigEndian, self.pannEnv.10.bigEndian,
			self.pannEnv.11.bigEndian)

		toRet.pitchEnv = (self.pitchEnv.0.bigEndian, self.pitchEnv.1.bigEndian,
			self.pitchEnv.2.bigEndian, self.pitchEnv.3.bigEndian, self.pitchEnv.4.bigEndian,
			self.pitchEnv.5.bigEndian, self.pitchEnv.6.bigEndian, self.pitchEnv.7.bigEndian,
			self.pitchEnv.8.bigEndian, self.pitchEnv.9.bigEndian, self.pitchEnv.10.bigEndian,
			self.pitchEnv.11.bigEndian)
		
		return toRet
	}}
}

extension sData {
	var bigEndian: sData { get {
		var toRet = self
		toRet.size = self.size.bigEndian
		toRet.loopBeg = self.loopBeg.bigEndian
		toRet.loopSize = self.loopBeg.bigEndian
		toRet.c2spd = self.c2spd.bigEndian
		
		return toRet
	}}

}

extension sData32 {
	var bigEndian: sData32 { get {
		var toRet = self
		toRet.size = self.size.bigEndian
		toRet.loopBeg = self.loopBeg.bigEndian
		toRet.loopSize = self.loopBeg.bigEndian
		toRet.c2spd = self.c2spd.bigEndian
		
		return toRet
	}}
	
}



