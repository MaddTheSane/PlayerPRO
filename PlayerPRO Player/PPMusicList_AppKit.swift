//
//  PPMusicListObject_AppKit.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/16/14.
//
//

import Cocoa

extension PPMusicListObject {
	var fileIcon: NSImage {get {
		var image = NSWorkspace.sharedWorkspace().iconForFile(musicURL.path);
		image.size = NSMakeSize(16, 16);
		return image;
	}}
}
