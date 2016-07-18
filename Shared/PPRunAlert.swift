//
//  PPRunAlert.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/15/14.
//
//

import Foundation
import AppKit.NSAlert

private func PPRunAlertPanelBase(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String?, alternateButton: String?, otherButton: String?, alertStyle: NSAlertStyle) -> Int {
	let theAlert = NSAlert()
	theAlert.alertStyle = alertStyle
	theAlert.messageText = title
	theAlert.informativeText = msgFormat()
	if defaultButton == nil && alternateButton == nil && otherButton == nil {
		let defButt = theAlert.addButton(withTitle: "OK")
		defButt.tag = NSAlertDefaultReturn
	} else {
		if let aDefaultButton = defaultButton {
			let defButt = theAlert.addButton(withTitle: aDefaultButton)
			defButt.tag = NSAlertDefaultReturn
		}
		if let aAlternateButton = alternateButton {
			let altButt = theAlert.addButton(withTitle: aAlternateButton)
			altButt.tag = NSAlertAlternateReturn
		}
		if let aOtherButton = otherButton {
			let othButt = theAlert.addButton(withTitle: aOtherButton)
			othButt.tag = NSAlertOtherReturn
		}
	}
	return theAlert.runModal()
}

@discardableResult internal func PPRunInformationalAlertPanel(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil) -> Int {
	return PPRunAlertPanelBase(title: title, message: msgFormat(), defaultButton: defaultButton, alternateButton: alternateButton, otherButton: otherButton, alertStyle: .informational)
}

@discardableResult internal func PPRunAlertPanel(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil) -> Int {
	return PPRunAlertPanelBase(title: title, message: msgFormat(), defaultButton: defaultButton, alternateButton: alternateButton, otherButton: otherButton, alertStyle: .warning)
}

@discardableResult internal func PPRunCriticalAlertPanel(title: String, message msgFormat: @autoclosure() -> String, defaultButton: String? = nil, alternateButton: String? = nil, otherButton: String? = nil) -> Int {
	return PPRunAlertPanelBase(title: title, message: msgFormat(), defaultButton: defaultButton, alternateButton: alternateButton, otherButton: otherButton, alertStyle: .critical)
}
