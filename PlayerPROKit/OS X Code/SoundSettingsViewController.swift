//
//  SoundSettingsViewController.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/20/14.
//
//

import PlayerPROCore
import Cocoa

private enum SoundRate {
	case rate11Khz
	case rate22Khz
	case rate44Khz
	case rate48Khz
}

private enum BitRate {
	case bits8
	case bits16
	case bits20
	case bits24
}

internal struct TagCoupling {
	var amount:		Int32
	var tag:		Int
	
	init(_ theAmount: Int32, _ theTag: Int) {
		amount = theAmount
		tag = theTag
	}
}

private func stereoDelayFromTag(_ theTag: Int) -> Int32 {
	var toSet: Int32 = 0

	for i in StereoDelayCoupling {
		if i.tag == theTag {
			toSet = i.amount
			break
		}
	}
	
	if toSet == 0 {
		toSet = 30
	}
	
	return toSet
}

private func oversamplingFromTag(_ theTag: Int) -> Int32 {
	var toSet: Int32 = 0
	
	for i in OversamplingCoupling {
		if i.tag == theTag {
			toSet = i.amount
			break
		}
	}

	if toSet == 0 {
		toSet = 1
	}
	
	return toSet;
}

private let StereoDelayCoupling = [TagCoupling(1, 1), TagCoupling(10, 2), TagCoupling(25, 3), TagCoupling(30, 4) ,TagCoupling(35, 5),
	TagCoupling(40, 6), TagCoupling(45, 7), TagCoupling(50, 8), TagCoupling(60, 9), TagCoupling(70, 10), TagCoupling(80, 11), TagCoupling(90, 12), TagCoupling(100, 13), TagCoupling(200, 14)];

private let ReverbAmountCoupling = [TagCoupling(25, 1), TagCoupling(30, 2), TagCoupling(35, 3), TagCoupling(40, 4), TagCoupling(45, 5),
	TagCoupling(50, 6), TagCoupling(60, 7), TagCoupling(70, 8), TagCoupling(80, 9), TagCoupling(90, 10), TagCoupling(100, 11), TagCoupling(200, 12), TagCoupling(300, 13), TagCoupling(400, 14)];

private let ReverbPercentCoupling = [TagCoupling(10, 1), TagCoupling(20, 2), TagCoupling(30, 3), TagCoupling(40, 4),
	TagCoupling(50, 5), TagCoupling(60, 6), TagCoupling(70, 7)]

private let OversamplingCoupling = [TagCoupling(1, 1), TagCoupling(2, 2), TagCoupling(3, 3), TagCoupling(4, 4),
	TagCoupling(6, 5), TagCoupling(8, 6), TagCoupling(10, 7), TagCoupling(16, 8), TagCoupling(20, 9), TagCoupling(25, 10), TagCoupling(30, 11)];

@objc public protocol SoundSettingsViewControllerDelegate : NSObjectProtocol {
	func soundView(_ :SoundSettingsViewController, bitsDidChange bits: Int16)
	func soundView(_ :SoundSettingsViewController, rateDidChange rat: UInt32)
	func soundView(_ :SoundSettingsViewController, reverbDidChangeActive isAct: Bool)
	func soundView(_ :SoundSettingsViewController, oversamplingDidChangeActive isAct: Bool)
	func soundView(_ :SoundSettingsViewController, stereoDelayDidChangeActive isAct: Bool)
	func soundView(_ :SoundSettingsViewController, surroundDidChangeActive isAct: Bool)
	func soundView(_ :SoundSettingsViewController, reverbStrengthDidChange rev: Int32)
	func soundView(_ :SoundSettingsViewController, reverbSizeDidChange rev: Int32)
	func soundView(_ :SoundSettingsViewController, oversamplingAmountDidChange ovs: Int32)
	func soundView(_ :SoundSettingsViewController, stereoDelayAmountDidChange std: Int32)
}

@objc public protocol SoundSettingsViewWithDriverControllerDelegate : SoundSettingsViewControllerDelegate {
	func soundView(_: SoundSettingsViewController, driverDidChange driv: MADSoundOutput)
}

open class SoundSettingsViewController: NSViewController {
	private var reverbActive		= false
	private var surroundActive		= false
	private var stereoDelayActive	= false
	private var oversamplingActive	= false
	
	@IBOutlet weak public var oversampling:		NSButton!
	@IBOutlet weak public var reverb:			NSButton!
	@IBOutlet weak public var stereoDelay:		NSButton!
	@IBOutlet weak public var surround:			NSButton!
	@IBOutlet weak public var rate:				NSMatrix!
	@IBOutlet weak public var outputBits:		NSMatrix!
	@IBOutlet weak public var soundDriver:		NSMatrix!
	@IBOutlet weak public var oversamplingNum:	NSPopUpButton!
	@IBOutlet weak public var reverbNum:		NSPopUpButton!
	@IBOutlet weak public var stereoDelayNum:	NSPopUpButton!
	@IBOutlet weak public var reverbPercent:	NSPopUpButton!
	@IBOutlet weak public var delegate:			SoundSettingsViewControllerDelegate? = nil
	
    @available(OSX 10.10, *)
    override open func viewDidLoad() {
        super.viewDidLoad()
        // Do view setup here.
    }
	
	private func set(bits: BitRate) {
		switch (bits) {
		case .bits8:
			outputBits.selectCell(atRow: 0, column: 0)
			
		case .bits20:
			outputBits.selectCell(atRow: 0, column: 2)
			
		case .bits24:
			outputBits.selectCell(atRow: 0, column: 3)
			
		default:
			outputBits.selectCell(atRow: 0, column: 1)
		}
	}
	
	private func set(rate currRate: SoundRate) {
		switch currRate {
		case .rate11Khz:
			rate.selectCell(atRow: 0, column: 0)
			
		case .rate22Khz:
			rate.selectCell(atRow: 0, column: 1)
			
			
		case .rate48Khz:
			rate.selectCell(atRow: 0, column: 3)
			
		default:
			rate.selectCell(atRow: 0, column: 2)
		}
	}
	
	private func currentRate() -> SoundRate {
		let curSelected: AnyObject! = rate.selectedCell()
		if rate.cell(atRow: 0, column: 0) === curSelected {
			return .rate11Khz
		} else if rate.cell(atRow: 0, column: 1) === curSelected {
			return .rate22Khz
		} else if rate.cell(atRow: 0, column: 2) === curSelected {
			return .rate44Khz
		} else if rate.cell(atRow: 0, column: 3) === curSelected {
			return .rate48Khz
		} else {
			return .rate44Khz
		}
	}
	
	private func setCurrentSoundDriver(_ theDriver: MADSoundOutput) {
		switch theDriver {
		case .CoreAudioDriver:
			soundDriver.selectCell(atRow: 0, column:0)
			
		case .MIDISoundDriver:
			soundDriver.selectCell(atRow: 2, column:0)
			
		default:
			soundDriver.selectCell(atRow: 3, column:0)
		}
	}
	
	private func currentSoundDriver() -> MADSoundOutput {
		let curSelected: AnyObject! = soundDriver.selectedCell()
		if soundDriver.cell(atRow: 0, column: 0) === curSelected {
			return .CoreAudioDriver;
			//} else if ([soundDriver cellAtRow:1 column:0] == curSelected) {
			//	return NoHardwareDriver;
		} else if soundDriver.cell(atRow: 2, column: 0) === curSelected {
			return .MIDISoundDriver;
		} else {
			return .NoHardwareDriver;
		}
	}
	
	private func currentBits() -> BitRate {
		let curSelected: AnyObject! = outputBits.selectedCell()
		if outputBits.cell(atRow: 0, column: 0) === curSelected {
			return .bits8;
		} else if outputBits.cell(atRow: 0, column: 1) === curSelected {
			return .bits16;
		} else if outputBits.cell(atRow: 0, column: 2) === curSelected {
			return .bits20;
		} else if outputBits.cell(atRow: 0, column: 3) === curSelected {
			return .bits24;
		} else {
			return .bits16;
		}
	}
	
	required override public init(nibName nibNameOrNil: NSNib.Name?, bundle nibBundleOrNil: Bundle?) {
		super.init(nibName: nibNameOrNil, bundle: nibBundleOrNil)
	}
	
	open class func newSoundSettingWindow() -> Self? {
		return self.init(nibName: NSNib.Name(rawValue: "PPSoundSettingsViewController"), bundle: Bundle(for: SoundSettingsViewController.self))
	}

	public func settingsFromDriverSettings(_ sett: MADDriverSettings) {
		let oversamplingState: Bool = sett.oversampling > 1
		let reverbState = sett.Reverb
		let stereoDelayState: Bool = sett.MicroDelaySize > 0
		
		self.reverbActive = reverbState;
		self.oversamplingActive = oversamplingState;
		self.stereoDelayActive = stereoDelayState;
		self.surroundActive = sett.surround;
		
		oversampling.state = oversamplingState ? .on : .off
		reverb.state = reverbState ? .on : .off
		stereoDelay.state = stereoDelayState ? .on : .off
		surround.state = sett.surround ? .on : .off
		
		oversamplingNum.isEnabled = oversamplingState
		reverbNum.isEnabled = reverbState
		reverbPercent.isEnabled = reverbState
		stereoDelayNum.isEnabled = stereoDelayState
		
		setCurrentSoundDriver(sett.driverMode)
		
		let unConvBits = sett.outPutBits
		var ConvBits = BitRate.bits8
		switch unConvBits {
		case 8:
			ConvBits = .bits8;
			
		case 20:
			ConvBits = .bits20;
			
		case 24:
			ConvBits = .bits24;
			
		default:
			ConvBits = .bits16;
		}
		set(bits: ConvBits)
		
		let unConvRate = sett.outPutRate
		var convRate = SoundRate.rate11Khz
		switch unConvRate {
		case 11025:
			convRate = .rate11Khz;
			
		case 22050:
			convRate = .rate22Khz;
			
		case 48000:
			convRate = .rate48Khz;
			
		default:
			convRate = .rate44Khz;
		}
		set(rate: convRate)
		
		let theRate = sett.MicroDelaySize
		var toSet: Int? = nil
		for i in ReverbPercentCoupling {
			if i.amount == theRate {
				toSet = i.tag
				break
			}
		}
		
		if toSet == nil {
			toSet = 4
		}
		
		stereoDelayNum.selectItem(at: toSet! - 1)
		
		toSet = nil
		let reverbAmount = sett.ReverbSize
		for i in ReverbAmountCoupling {
			if i.amount == reverbAmount {
				toSet = i.tag
				break
			}
		}
		
		if toSet == nil {
			toSet = 4
		}
		reverbNum.selectItem(at: toSet! - 1)
		
		toSet = nil
		let reverbPercentage = sett.ReverbStrength;
		for i in ReverbPercentCoupling {
			if i.amount == reverbPercentage {
				toSet = i.tag
				break
			}
		}
		
		if toSet == nil {
			toSet = 3
		}
		reverbPercent.selectItem(at: toSet! - 1)
		
		toSet = nil
		let oversamplingAmount = sett.oversampling;
		for i in OversamplingCoupling {
			if i.amount == oversamplingAmount {
				toSet = i.tag
				break
			}
		}
		
		if toSet == nil {
			toSet = 1
		}
		oversamplingNum.selectItem(at: toSet! - 1)
		
		if delegate is SoundSettingsViewWithDriverControllerDelegate {
			soundDriver.isEnabled = true
		} else {
			soundDriver.isEnabled = false
		}
	}

	required public init?(coder: NSCoder) {
		super.init(coder: coder)
	}
	
	@IBAction public func changeDriver(_ sender: AnyObject!) {
		let driver = currentSoundDriver()
		if let delegate1 = delegate as? SoundSettingsViewWithDriverControllerDelegate {
			delegate1.soundView(self, driverDidChange: driver)
		}
	}
	
	@IBAction public func changeBits(_ sender: AnyObject!) {
		let theBits = currentBits()
		var returnBits: Int16 = 0
		switch theBits {
		case .bits8:
			returnBits = 8;
			
		case .bits20:
			returnBits = 20;
			
		case .bits24:
			returnBits = 24;
			
		default:
			returnBits = 16;
		}
		delegate?.soundView(self, bitsDidChange: returnBits)
	}
	
	@IBAction public func changeRate(_ sender: AnyObject!) {
		let rate1 = currentRate()
		var returnRate: UInt32 = 0
		switch rate1 {
		case .rate11Khz:
			returnRate = 11025;
			
		case .rate22Khz:
			returnRate = 22050;
			
		case .rate48Khz:
			returnRate = 48000;
			
		default:
			returnRate = 44100;
		}
		delegate?.soundView(self, rateDidChange: returnRate)
	}
	
	@IBAction public func changeChecked(_ sender: AnyObject!) {
		let reverbState = reverb.state == .off ? false : true;
		let stereoDelayState = stereoDelay.state == .off ? false : true
		let oversamplingState = oversampling.state == .off ? false : true
		let surroundState = surround.state == .off ? false : true
		
		oversamplingNum.isEnabled = oversamplingState
		reverbNum.isEnabled = reverbState
		reverbPercent.isEnabled = reverbState
		stereoDelayNum.isEnabled = stereoDelayState
		
		if reverbState != reverbActive {
			delegate?.soundView(self, reverbDidChangeActive: reverbState)
		}
		if stereoDelayState != stereoDelayActive {
			delegate?.soundView(self, stereoDelayDidChangeActive: stereoDelayState)
			if stereoDelayState {
				delegate?.soundView(self, stereoDelayAmountDidChange: stereoDelayFromTag(stereoDelayNum.selectedItem!.tag))
			}
		}
		if oversamplingState != oversamplingActive {
			delegate?.soundView(self, oversamplingDidChangeActive: oversamplingState)
			if (oversamplingState) {
				delegate?.soundView(self, oversamplingAmountDidChange: oversamplingFromTag(oversamplingNum.selectedItem!.tag))
			}
		}
		if surroundState != surroundActive {
			delegate?.soundView(self, surroundDidChangeActive: surroundState)
		}
		self.reverbActive = reverbState;
		self.stereoDelayActive = stereoDelayState;
		self.oversamplingActive = oversamplingState;
		self.surroundActive = surroundState;
	}
	
	@IBAction func changeOversampling(_ sender: NSMenuItem!) {
		let toSet = oversamplingFromTag(sender.tag)
		delegate?.soundView(self, oversamplingAmountDidChange: toSet)
	}
	
	@IBAction public func changeReverbAmount(_ sender: NSMenuItem!) {
		var toSet: Int32 = 0;
		let tag = sender.tag

		for i in ReverbAmountCoupling {
			if i.tag == tag {
				toSet = i.amount
				break
			}
		}
		
		if toSet == 0 {
			toSet = 25
		}
		
		delegate?.soundView(self, reverbSizeDidChange: toSet)
	}
	
	@IBAction public func changeReverbPercent(_ sender: NSMenuItem!) {
		var toSet: Int32 = 0
		let tag = sender.tag
		
		for i in ReverbPercentCoupling {
			if i.tag == tag {
				toSet = i.amount
				break
			}
		}

		if toSet == 0 {
			toSet = 30
		}
		delegate?.soundView(self, reverbStrengthDidChange: toSet)
	}
	
	@IBAction public func changeStereoDelay(_ sender: NSMenuItem!) {
		let toSet = stereoDelayFromTag(sender.tag)
		delegate?.soundView(self, stereoDelayAmountDidChange: toSet)
	}
}

