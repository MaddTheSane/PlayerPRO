//
//  PPRunAlert.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/15/14.
//
//

import Foundation
import AppKit.NSAlert

@discardableResult private func PPRunAlertPanelBase(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String?, alternateButton: String?, otherButton: String?, alertStyle: NSAlert.Style) -> NSApplication.ModalResponse {
	let theAlert = NSAlert()
	theAlert.alertStyle = alertStyle
	theAlert.messageText = title
	theAlert.informativeText = msgFormat()
	if defaultButton == nil && alternateButton == nil && otherButton == nil {
		let defButt = theAlert.addButton(withTitle: "OK")
		defButt.tag = NSApplication.ModalResponse.alertFirstButtonReturn.rawValue
	} else {
		if let aDefaultButton = defaultButton {
			let defButt = theAlert.addButton(withTitle: aDefaultButton)
			defButt.tag = NSApplication.ModalResponse.alertFirstButtonReturn.rawValue
		}
		if let aAlternateButton = alternateButton {
			let altButt = theAlert.addButton(withTitle: aAlternateButton)
			altButt.tag = NSApplication.ModalResponse.alertSecondButtonReturn.rawValue
		}
		if let aOtherButton = otherButton {
			let othButt = theAlert.addButton(withTitle: aOtherButton)
			othButt.tag = NSApplication.ModalResponse.alertThirdButtonReturn.rawValue
		}
	}
	return theAlert.runModal()
}

@discardableResult internal func PPRunInformationalAlertPanel(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil) -> NSApplication.ModalResponse {
	return PPRunAlertPanelBase(title: title, message: msgFormat(), defaultButton: defaultButton, alternateButton: alternateButton, otherButton: otherButton, alertStyle: .informational)
}

@discardableResult internal func PPRunAlertPanel(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil) -> NSApplication.ModalResponse {
	return PPRunAlertPanelBase(title: title, message: msgFormat(), defaultButton: defaultButton, alternateButton: alternateButton, otherButton: otherButton, alertStyle: .warning)
}

@discardableResult internal func PPRunCriticalAlertPanel(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil) -> NSApplication.ModalResponse {
	return PPRunAlertPanelBase(title: title, message: msgFormat(), defaultButton: defaultButton, alternateButton: alternateButton, otherButton: otherButton, alertStyle: .critical)
}
