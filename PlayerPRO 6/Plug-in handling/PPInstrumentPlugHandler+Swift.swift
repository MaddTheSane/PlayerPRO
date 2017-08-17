//
//  PPInstrumentPlugHandler+Swift.swift
//  PlayerPRO 6
//
//  Created by C.W. Betts on 8/17/17.
//

import Foundation

extension PPInstrumentPlugHandler {
	open func identifyInstrumentFile(_ ref: URL) throws -> OSType {
		var type1: OSType = 0
		try __identifyInstrumentFile(ref, type: &type1)
		return type1
	}
}
