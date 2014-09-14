//
//  OpenPanelViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

import Cocoa

private func ==(lhs: OpenPanelViewController.OpenPanelViewItem, rhs: OpenPanelViewController.OpenPanelViewItem) -> Bool {
	if lhs.theUtiType != rhs.theUtiType {
		return false
	} else if lhs.name != rhs.name {
		return false
	} else if lhs.utis != rhs.utis {
		return false
	} else {
		return true
	}
}

class OpenPanelViewController: NSViewController {
	enum utiType: Int {
		case allType = -1
		case trackerType = -2
		case playlistType = -3
		case instrumentType = -4
		case otherType = -5
	};
	
	enum trackerType : UInt {
		case tracker = 1
		case playlist
		case instrument
		case other
	}
	
	private class OpenPanelViewItem: DebugPrintable, Printable, Hashable {
		let name: String
		let theUtiType: trackerType
		let utis: [String]
		
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
				// TODO: fail
				theUtiType = .other
			}
			
			utis = ut
			name = nam
		}
		
		var hashValue: Int { get {
			return name.hashValue ^ theUtiType.hashValue ^ (utis as NSArray).hash
			}}
		
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
	
	let openPanel: NSOpenPanel
	private var utiObjects = [OpenPanelViewItem]()
	private var multipleSelection: Bool
	@IBOutlet weak var popUp: NSPopUpButton! = nil
	
	var allowsMultipleSelectionOfTrackers:Bool { get {
		return multipleSelection
	}
	set {
		multipleSelection = newValue
		openPanel.allowsMultipleSelection = multipleSelection
	}}
	
	required init(coder: NSCoder!) {
		println("initWithCoder was called?")
		multipleSelection = false
		openPanel = NSOpenPanel()
		
		super.init(coder: coder)
	}
	
	@IBAction func selectUTI(sender: AnyObject!) {
		var tag = (sender as NSMenuItem).tag;
		var allowedUTIs = [String]();
		switch (tag) {
		case utiType.allType.toRaw():
			for obj in utiObjects {
				allowedUTIs += obj.utis
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.trackerType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == trackerType.tracker) {
					allowedUTIs += obj.utis
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.playlistType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == .playlist) {
					allowedUTIs += obj.utis
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.instrumentType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == .instrument) {
					allowedUTIs += obj.utis
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			
		case utiType.otherType.toRaw():
			for obj in utiObjects {
				if (obj.theUtiType == .other) {
					for uti in obj.utis {
						allowedUTIs.append(uti)
					}
				}
			}
			openPanel.allowedFileTypes = allowedUTIs
			if (allowsMultipleSelectionOfTrackers) {
				openPanel.allowsMultipleSelection = true
			}
			break;
			
		default:
			if (tag < utiObjects.count && tag >= 0) {
				var selObj = utiObjects[tag];
				openPanel.allowedFileTypes = selObj.utis
				if (allowsMultipleSelectionOfTrackers == true) {
					if (selObj.theUtiType == .tracker) {
						openPanel.allowsMultipleSelection = true
					} else {
						openPanel.allowsMultipleSelection = false
					}
				}
				
			}
			break;
		}
	}
	
	init(openPanel panel: NSOpenPanel, trackerDictionary td: [String: [String]]? = nil, playlistDictionary pd: [String: [String]]? = nil, instrumentDictionary insDict: [String: [String]]? = nil, additionalDictionary adddict: [String: [String]]? = nil) {
		openPanel = panel
		multipleSelection = false
		super.init(nibName: "OpenPanelViewController", bundle: nil)
		if let tdHere = td {
			for (key, utis) in tdHere {
				var obj = OpenPanelViewItem(type: .trackerType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		if let pdHere = pd {
			for (key, utis) in pdHere {
				var obj = OpenPanelViewItem(type: .playlistType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		if let insHere = insDict {
			for (key, utis) in insHere {
				var obj = OpenPanelViewItem(type: .instrumentType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		if let addHere = adddict {
			for (key, utis) in addHere {
				var obj = OpenPanelViewItem(type: .otherType, utis: utis, name: key)
				utiObjects.append(obj)
			}
		}
		
		utiObjects.sort({(lhs, rhs) -> Bool in
			if (lhs.theUtiType.toRaw() < rhs.theUtiType.toRaw()) {
				return true
			} else if (lhs.theUtiType.toRaw() > rhs.theUtiType.toRaw()) {
				return false
			} else {
				
				let result = lhs.name.localizedStandardCompare(rhs.name);
				return result == NSComparisonResult.OrderedAscending;
			}
			})
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		// Do view setup here.
		
		let fileTypeSelectionMenu = popUp.menu;
		let moreThanTwoTypes = hasMoreThanTwoTypes();
		if (moreThanTwoTypes) {
			let mi0 = NSMenuItem(title: "All Openable Files", action: "selectUTI:", keyEquivalent: "")
			mi0.tag = utiType.allType.toRaw()
			mi0.target = self
			fileTypeSelectionMenu.addItem(mi0)
			
			fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
		}
		
		for item in utiObjects {
			if (item.theUtiType == .tracker) {
				let mi0 = NSMenuItem(title: "All Trackers", action: "selectUTI:", keyEquivalent: "")
				mi0.tag = utiType.trackerType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				break;
			}
		}
		
		for item in utiObjects {
			if (item.theUtiType == .playlist) {
				let mi0 = NSMenuItem(title: "All Playlists", action: "selectUTI:", keyEquivalent: "")
				mi0.tag = utiType.playlistType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				break;
			}
		}
		
		for item in utiObjects {
			if (item.theUtiType == .instrument) {
				let mi0 = NSMenuItem(title: "All Instruments", action: "selectUTI:", keyEquivalent: "")
				mi0.tag = utiType.instrumentType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				break;
			}
		}
		
		for item in utiObjects {
			if (item.theUtiType == .other) {
				let mi0 = NSMenuItem(title: "All Other", action: "selectUTI:", keyEquivalent: "")
				mi0.tag = utiType.otherType.toRaw()
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				break;
			}
		}
		fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
		
		for (i, curItem) in enumerate(utiObjects)  {
			if (moreThanTwoTypes) {
				if (i - 1 >= 0) {
					let prevItem = utiObjects[i - 1];
					if (curItem.theUtiType != prevItem.theUtiType) {
						fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
					}
				}
			}
			let mi = NSMenuItem(title: curItem.name, action: "selectUTI:", keyEquivalent: "")
			mi.tag = i
			mi.target = self
			fileTypeSelectionMenu.addItem(mi)
		}
		popUp.selectItemAtIndex(0)
	}
	
	func setupDefaults() {
		var fileUTIs = [String]()
		
		for obj in utiObjects {
			fileUTIs += obj.utis
		}
		
		if (!allowsMultipleSelectionOfTrackers) {
			openPanel.allowsMultipleSelection = false
		}
		
		openPanel.allowedFileTypes = fileUTIs
		openPanel.accessoryView = self.view
	}
	
	private func hasMoreThanTwoTypes() -> Bool {
		let utiCount = utiObjects.count;
		if (utiCount < 2) {
			return false;
		}
		
		for (var i = 1; i < utiCount; i++) {
			let obj1 = utiObjects[i - 1];
			let obj2 = utiObjects[i];
			if (obj1.theUtiType != obj2.theUtiType) {
				return true;
			}
		}
		return false;
	}
}
