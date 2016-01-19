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

private var OpenPanelsInUse = [OpenPanelViewController]()

private func allOpenableTypesFromUTI(anUTI: String) -> [String] {
	var theOpenables = [anUTI]
	
	if #available(OSX 10.10, *) {
		let utiTagClasses = [kUTTagClassFilenameExtension as String, kUTTagClassMIMEType as String,
			/*kUTTagClassNSPboardType as String*/]
		
		for utiTag in utiTagClasses {
			guard let unPreArr = UTTypeCopyAllTagsWithClass(anUTI, utiTag) else {
				continue
			}
			
			let preArr = unPreArr.takeRetainedValue() as NSArray
			
			guard let anArr = preArr as? [String] else {
				continue
			}
			theOpenables += anArr
		}
		if let unPreArr = UTTypeCopyAllTagsWithClass(anUTI, kUTTagClassOSType),
			let anArr = unPreArr.takeRetainedValue() as NSArray as? [String] {
				let convertedArr = anArr.map({UTGetOSTypeFromString($0)}).map({NSFileTypeForHFSTypeCode($0)!})
				theOpenables += convertedArr
		}
	} else {
		// Fallback on earlier versions
		if let aType = UTTypeCopyPreferredTagWithClass(anUTI, kUTTagClassFilenameExtension)?.takeRetainedValue() {
			theOpenables.append(aType as NSString as String)
		}
	}
	
	return theOpenables
}

class OpenPanelViewController: NSViewController, NSOpenSavePanelDelegate {
	private enum utiType: Int {
		case allType = -1
		case trackerType = -2
		case playlistType = -3
		case instrumentType = -4
		case otherType = -5
	}
	
	private enum trackerType: Int {
		case tracker = 1
		case playlist
		case instrument
		case other
	}
	
	private struct OpenPanelViewItem: CustomDebugStringConvertible, CustomStringConvertible, Hashable {
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
				theUtiType = .other
			}
			
			utis = ut
			name = nam
		}
		
		var hashValue: Int {
			return name.hashValue ^ theUtiType.hashValue ^ utis.count
		}
		
		var debugDescription: String {
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
		}
		
		var description: String {
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
		}
	}
	
	private let openPanel: NSOpenPanel
	private let utiObjects: [OpenPanelViewItem]
	@IBOutlet weak var popUp: NSPopUpButton? = nil
	
	var allowsMultipleSelectionOfTrackers: Bool = false {
		didSet {
			self.openPanel.allowsMultipleSelection = allowsMultipleSelectionOfTrackers
		}
	}
	
	required init?(coder: NSCoder) {
		print("initWithCoder was called?")
		openPanel = NSOpenPanel()
		allowsMultipleSelectionOfTrackers = false
		utiObjects = [OpenPanelViewItem]()

		super.init(coder: coder)
	}
	
	@IBAction func selectUTI(sender: AnyObject?) {
		if let menuIte = sender as? NSMenuItem {
			let tag = menuIte.tag;
			var allowedUTIs = [String]();
			switch (tag) {
			case utiType.allType.rawValue:
				for obj in utiObjects {
					allowedUTIs += obj.utis
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypesFromUTI(uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if (allowsMultipleSelectionOfTrackers) {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.trackerType.rawValue:
				for obj in utiObjects {
					if (obj.theUtiType == trackerType.tracker) {
						allowedUTIs += obj.utis
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypesFromUTI(uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if (allowsMultipleSelectionOfTrackers) {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.playlistType.rawValue:
				for obj in utiObjects {
					if (obj.theUtiType == .playlist) {
						allowedUTIs += obj.utis
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypesFromUTI(uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if (allowsMultipleSelectionOfTrackers) {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.instrumentType.rawValue:
				for obj in utiObjects {
					if (obj.theUtiType == .instrument) {
						allowedUTIs += obj.utis
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypesFromUTI(uti)
					}
					
					return aTyp
				}()
				
				openPanel.allowedFileTypes = fileTypes
				if (allowsMultipleSelectionOfTrackers) {
					openPanel.allowsMultipleSelection = true
				}
				
			case utiType.otherType.rawValue:
				for obj in utiObjects {
					if (obj.theUtiType == .other) {
						allowedUTIs.appendContentsOf(obj.utis)
					}
				}
				
				let fileTypes: [String] = {
					var aTyp = [String]()
					
					for uti in allowedUTIs {
						aTyp += allOpenableTypesFromUTI(uti)
					}
					
					return aTyp
				}()

				openPanel.allowedFileTypes = fileTypes
				if (allowsMultipleSelectionOfTrackers) {
					openPanel.allowsMultipleSelection = true
				}
				break;
				
			default:
				if (tag < utiObjects.count && tag >= 0) {
					let selObj = utiObjects[tag];
					let fileTypes: [String] = {
						var aTyp = [String]()
						
						for uti in selObj.utis {
							aTyp += allOpenableTypesFromUTI(uti)
						}
						
						return aTyp
					}()
					
					openPanel.allowedFileTypes = fileTypes
					if (allowsMultipleSelectionOfTrackers == true) {
						if (selObj.theUtiType == .tracker) {
							openPanel.allowsMultipleSelection = true
						} else {
							openPanel.allowsMultipleSelection = false
						}
					}
				}
			}
		} else {
			NSBeep()
		}
	}
	
	override init?(nibName: String?, bundle: NSBundle?) {
		openPanel = NSOpenPanel()
		allowsMultipleSelectionOfTrackers = false
		utiObjects = [OpenPanelViewItem]()

		super.init(nibName: nibName, bundle: bundle)
	}
	
	init?(openPanel panel: NSOpenPanel, trackerDictionary td: [String: [String]]? = nil, playlistDictionary pd: [String: [String]]? = nil, instrumentDictionary insDict: [String: [String]]? = nil, additionalDictionary adddict: [String: [String]]? = nil) {
		openPanel = panel
		allowsMultipleSelectionOfTrackers = false
		var tmpUTIs = [OpenPanelViewItem]()
		if let tdHere = td {
			for (key, utis) in tdHere {
				let obj = OpenPanelViewItem(type: .trackerType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		if let pdHere = pd {
			for (key, utis) in pdHere {
				let obj = OpenPanelViewItem(type: .playlistType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		if let insHere = insDict {
			for (key, utis) in insHere {
				let obj = OpenPanelViewItem(type: .instrumentType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		if let addHere = adddict {
			for (key, utis) in addHere {
				let obj = OpenPanelViewItem(type: .otherType, utis: utis, name: key)
				tmpUTIs.append(obj)
			}
		}
		
		tmpUTIs.sortInPlace({(lhs, rhs) -> Bool in
			if (lhs.theUtiType.rawValue < rhs.theUtiType.rawValue) {
				return true
			} else if (lhs.theUtiType.rawValue > rhs.theUtiType.rawValue) {
				return false
			} else {
				
				let result = lhs.name.localizedStandardCompare(rhs.name);
				return result == NSComparisonResult.OrderedAscending;
			}
			})
		
		utiObjects = tmpUTIs
		super.init(nibName: "PPOpenPanel", bundle: nil)
		
		openPanel.delegate = self
	}
	
	func beginOpenPanel(completionHandler completionHandler: (result: Int) -> Void) {
		beginWithCompletionHandler(completionHandler)
	}
	
	func beginWithCompletionHandler(resultHandle: (result: Int) -> Void) {
		OpenPanelsInUse.append(self)
		openPanel.beginWithCompletionHandler({ (result) -> Void in
			if let anInt = OpenPanelsInUse.indexOf(self) {
				OpenPanelsInUse.removeAtIndex(anInt)
			}
			resultHandle(result: result)
		})
	}

	@objc func beginOpenPanel(parentWindow parentWindow: NSWindow, completionHandler resultHandle: (result: Int) -> Void) {
		OpenPanelsInUse.append(self)
		openPanel.beginSheetModalForWindow(parentWindow, completionHandler: { (result) -> Void in
			if let anInt = OpenPanelsInUse.indexOf(self) {
				OpenPanelsInUse.removeAtIndex(anInt)
			}
			resultHandle(result: result)
		})
	}
	
	override func awakeFromNib() {
		super.awakeFromNib()
		// Do view setup here.
		
		if let fileTypeSelectionMenu = popUp?.menu {
			let moreThanTwoTypes = hasMoreThanTwoTypes
			if (moreThanTwoTypes) {
				let mi0 = NSMenuItem(title: "All Openable Files", action: "selectUTI:", keyEquivalent: "")
				mi0.tag = utiType.allType.rawValue
				mi0.target = self
				fileTypeSelectionMenu.addItem(mi0)
				
				fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
			}
			
			for item in utiObjects {
				if (item.theUtiType == .tracker) {
					let mi0 = NSMenuItem(title: "All Trackers", action: "selectUTI:", keyEquivalent: "")
					mi0.tag = utiType.trackerType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			
			for item in utiObjects {
				if (item.theUtiType == .playlist) {
					let mi0 = NSMenuItem(title: "All Playlists", action: "selectUTI:", keyEquivalent: "")
					mi0.tag = utiType.playlistType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			
			for item in utiObjects {
				if (item.theUtiType == .instrument) {
					let mi0 = NSMenuItem(title: "All Instruments", action: "selectUTI:", keyEquivalent: "")
					mi0.tag = utiType.instrumentType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			
			for item in utiObjects {
				if (item.theUtiType == .other) {
					let mi0 = NSMenuItem(title: "All Other", action: "selectUTI:", keyEquivalent: "")
					mi0.tag = utiType.otherType.rawValue
					mi0.target = self
					fileTypeSelectionMenu.addItem(mi0)
					
					break;
				}
			}
			fileTypeSelectionMenu.addItem(NSMenuItem.separatorItem())
			
			for (i, curItem) in utiObjects.enumerate()  {
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
		}
		popUp?.selectItemAtIndex(0)
	}
	
	func setupDefaults() {
		var fileUTIs = [String]()
		
		for obj in utiObjects {
			fileUTIs += obj.utis
		}
		
		if (!allowsMultipleSelectionOfTrackers) {
			openPanel.allowsMultipleSelection = false
		}
		
		var allowedTypes = [String]()
		
		for uti in fileUTIs {
			allowedTypes += allOpenableTypesFromUTI(uti)
		}
		
		self.openPanel.allowedFileTypes = allowedTypes
		openPanel.canChooseFiles = true
		openPanel.accessoryView = self.view
	}
	
	private var hasMoreThanTwoTypes: Bool {
		let utiCount = utiObjects.count
		if (utiCount < 2) {
			return false
		}
		
		for i in 1 ..< utiCount {
			let obj1 = utiObjects[i - 1]
			let obj2 = utiObjects[i]
			if (obj1.theUtiType != obj2.theUtiType) {
				return true
			}
		}
		return false;
	}
	
	func panel(sender: AnyObject, shouldEnableURL url: NSURL) -> Bool {
		// We don't support non-file URLs yet.
		if !url.fileURL {
			return false
		}
		return true
	}
}
