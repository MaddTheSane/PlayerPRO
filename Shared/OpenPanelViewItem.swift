//
//  OpenPanelViewItem.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

import Cocoa

enum utiType: Int {
	case allType = -1
	case trackerType = -2
	case playlistType = -3
	case instrumentType = -4
	case otherType = -5
};

enum trackerType : UInt32 {
	case tracker = 1
	case playlist
	case instrument
	case other
}

class OpenPanelViewItem: DebugPrintable, Printable {
	private(set) var name: String! = nil
	private(set) var theUtiType: trackerType = .tracker
	private(set) var utis: [String]! = nil

	init(type typ: utiType, utis ut: [String], name nam: String) {
		switch (typ) {
		case .trackerType:
			theUtiType = .tracker;
			
		case .playlistType:
			theUtiType = .playlist
			
		case .instrumentType:
			theUtiType = .instrument
			
		case .otherType:
			theUtiType = .other
			
		default:
			// Because we can't fail on inits yet...
			theUtiType = .other
		}
		
		utis = ut
		name = nam
	}

	var debugDescription: String { get {
		var des: String
		if (theUtiType == .playlist) {
			des = "Playlist";
		} else if (theUtiType == .instrument) {
			des = "Instrument";
		} else if (theUtiType == .tracker) {
			des = "Tracker";
		} else if (theUtiType == .other) {
			des = "Other";
		} else {
			des = "Unknown";
		}
		
		return "\(name): \(des) - \(utis.description)"
	}}

	var description: String { get {
		var des: String
		if (theUtiType == .playlist) {
			des = "Playlist";
		} else if (theUtiType == .instrument) {
			des = "Instrument";
		} else if (theUtiType == .tracker) {
			des = "Tracker";
		} else if (theUtiType == .other) {
			des = "Other";
		} else {
			des = "Unknown";
		}
		return "\(name): \(des)"
	}}

}
