//
//  PPMusicListObject_AppKit.swift
//  PPMacho
//
//  Created by C.W. Betts on 7/16/14.
//
//

import Cocoa
import PlayerPROCore
import PlayerPROKit

extension PPMusicListObject {
	var fileIcon: NSImage {get {
		var image = NSWorkspace.sharedWorkspace().iconForFile(musicURL.path);
		image.size = NSMakeSize(16, 16);
		return image;
	}}
}

extension PPMusicList {
	func beginLoadingOfMusicListAtURL(toOpen: NSURL, completionHandle theHandle: (theErr: NSError!) ->Void) {
		var conn = NSXPCConnection(serviceName: "net.sourceforge.playerpro.StcfImporter")
		conn.remoteObjectInterface = NSXPCInterface(`protocol`: PPSTImporterHelper.self);
		
		conn.resume()
		
		conn.remoteObjectProxy.loadStcfAtURL(toOpen, withReply: {(bookmarkData:[NSObject : AnyObject]!, error: NSError!) -> Void in
			NSOperationQueue.mainQueue().addOperationWithBlock({
				if (error) {
					theHandle(theErr: error)
				} else {
					var invalidAny: AnyObject? = bookmarkData["lostMusicCount"];
					var selectedAny: AnyObject? = bookmarkData["SelectedMusic"]
					var pathsAny: AnyObject? = bookmarkData["MusicPaths"]
					if (!invalidAny || !selectedAny || !pathsAny) {
						var lolwut = CreateErrorFromMADErrorType(.UnknownErr)
						theHandle(theErr: lolwut)
					} else {
						var pathsURL: [PPMusicListObject] = []
						self.lostMusicCount = invalidAny as UInt;
						self.selectedMusic = selectedAny as Int;
						for aPath in pathsAny as NSArray {
							var tmpURL = NSURL.fileURLWithPath(aPath as String)
							if (!tmpURL) {
								continue;
							}
							var tmpObj = PPMusicListObject(URL: tmpURL)
							pathsURL.append(tmpObj)
						}
						self.loadMusicList(pathsURL)
						
						theHandle(theErr: nil)
					}
				}
				
				conn.invalidate();
				})
			})
	}
}
