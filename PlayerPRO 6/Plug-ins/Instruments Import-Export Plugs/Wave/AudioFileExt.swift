//
//  AudioFileExt.swift
//  PPMacho
//
//  Created by C.W. Betts on 8/28/14.
//
//

import Foundation
import AudioToolbox
import CoreAudio
import PlayerPROKit

// MARK: Audio File

internal enum AudioFileType: OSType {
	case AIFF				= 1095321158
	case AIFC				= 1095321155
	case WAVE				= 1463899717
	case SoundDesigner2		= 1399075430
	case Next				= 1315264596
	case MP3				= 1297106739
	case MP2				= 1297106738
	case MP1				= 1297106737
	case AC3				= 1633889587
	case AAC_ADTS			= 1633973363
	case MPEG4				= 1836069990
	case M4A				= 1832149350
	case M4B				= 1832149606
	case CAF				= 1667327590
	case threeGP			= 862417008
	case threeGP2			= 862416946
	case AMR				= 1634562662
	
	func stringValue() -> String {
		return OSTypeToString(self.rawValue)!
	}
}

internal enum AudioFileFlags: UInt32 {
	case EraseFile				= 1
	case DontPageAlignAudioData	= 2
}

internal func AudioFileCreate(withURL inFileRef: CFURL, fileType inFileType: AudioFileType, inout format inFormat: AudioStreamBasicDescription, flags inFlags: AudioFileFlags, inout audioFile outAudioFile: AudioFileID) -> OSStatus {
	return AudioFileCreateWithURL(inFileRef, inFileType.rawValue, &inFormat, inFlags.rawValue, &outAudioFile)
}

// MARK: Audio Format

internal enum AudioFormat: OSType {
	case DVIIntelIMA			= 0x6D730011
	case MicrosoftGSM			= 0x6D730031
	case LinearPCM				= 1819304813
	case AC3					= 1633889587
	case Six0958AC3				= 1667326771
	case AppleIMA4				= 1768775988
	case MPEG4AAC				= 1633772320
	case MPEG4CELP				= 1667591280
	case MPEG4HVXC				= 1752594531
	case MPEG4TwinVQ			= 1953986161
	case MACE3					= 1296122675
	case MACE6					= 1296122678
	case ULaw					= 1970037111
	case ALaw					= 1634492791
	case QDesign				= 1363430723
	case QDesign2				= 1363430706
	case QUALCOMM				= 1365470320
	case MPEGLayer1				= 778924081
	case MPEGLayer2				= 778924082
	case MPEGLayer3				= 778924083
	case TimeCode				= 1953066341
	case MIDIStream				= 1835623529
	case ParameterValueStream	= 1634760307
	case AppleLossless			= 1634492771
	case MPEG4AAC_HE			= 1633772392
	case MPEG4AAC_LD			= 1633772396
	case MPEG4AAC_ELD			= 1633772389
	case MPEG4AAC_ELD_SBR		= 1633772390
	case MPEG4AAC_ELD_V2		= 1633772391
	case MPEG4AAC_HE_V2			= 1633772400
	case MPEG4AAC_Spatial		= 1633772403
	case AMR					= 1935764850
	case AMR_WB					= 1935767394
	case Audible				= 1096107074
	case iLBC					= 1768710755
	case AES3					= 1634038579
	
	func stringValue() -> String {
		return OSTypeToString(self.rawValue)!
	}
}

internal struct AudioFormatFlag : RawOptionSetType {
	private var value: UInt32 = 0
	var rawValue: UInt32 {
		get {
			return value
		}
	}
	init(rawValue value: UInt32) { self.value = value }
	static var allZeros: AudioFormatFlag { return self(rawValue: 0) }
	static func fromMask(raw: UInt32) -> AudioFormatFlag { return self(rawValue: raw) }
	init(nilLiteral: ()) { self = AudioFormatFlag.FlagsAreAllClear }
	
	static var NativeFloatPacked: AudioFormatFlag {
		return Float | NativeEndian | Packed
	}
	
	static var Float:				AudioFormatFlag { return self(rawValue: 1 << 0) }
	static var BigEndian:			AudioFormatFlag { return self(rawValue: 1 << 1) }
	static var SignedInteger:		AudioFormatFlag { return self(rawValue: 1 << 2) }
	static var Packed:				AudioFormatFlag { return self(rawValue: 1 << 3) }
	static var AlignedHigh:			AudioFormatFlag { return self(rawValue: 1 << 4) }
	static var NonInterleaved:		AudioFormatFlag { return self(rawValue: 1 << 5) }
	static var NonMixable:			AudioFormatFlag { return self(rawValue: 1 << 6) }
	static var FlagsAreAllClear:	AudioFormatFlag { return self(rawValue: 1 << 31) }
	static var NativeEndian:		AudioFormatFlag {
		#if __BIG_ENDIAN__
			return IsBigEndian
		#else
			return self(rawValue: 0)
		#endif
	}
	init(_ value: LinearPCMFormatFlag) {
		self.value = value.rawValue
	}
}

internal struct LinearPCMFormatFlag : RawOptionSetType {
	private var value: UInt32 = 0
	var rawValue: UInt32 {
		get {
			return value
		}
	}
	init(rawValue value: UInt32) { self.value = value }
	static var allZeros: LinearPCMFormatFlag { return self(rawValue: 0) }
	static func fromMask(raw: UInt32) -> LinearPCMFormatFlag { return self(rawValue: raw) }
	init(nilLiteral: ()) { self = LinearPCMFormatFlag.FlagsAreAllClear }
	
	static var NativeFloatPacked: LinearPCMFormatFlag {
		return Float | NativeEndian | Packed
	}
	
	static var Float:				LinearPCMFormatFlag { return self(rawValue: 1 << 0) }
	static var BigEndian:			LinearPCMFormatFlag { return self(rawValue: 1 << 1) }
	static var SignedInteger:		LinearPCMFormatFlag { return self(rawValue: 1 << 2) }
	static var Packed:				LinearPCMFormatFlag { return self(rawValue: 1 << 3) }
	static var AlignedHigh:			LinearPCMFormatFlag { return self(rawValue: 1 << 4) }
	static var NonInterleaved:		LinearPCMFormatFlag { return self(rawValue: 1 << 5) }
	static var NonMixable:			LinearPCMFormatFlag { return self(rawValue: 1 << 6) }
	static var FlagsAreAllClear:	LinearPCMFormatFlag { return self(rawValue: 1 << 31) }
	static var NativeEndian:		LinearPCMFormatFlag {
		#if __BIG_ENDIAN__
			return BigEndian
		#else
			return self(rawValue: 0)
		#endif
	}
	static var FlagsSampleFractionShift: LinearPCMFormatFlag { return self(rawValue: 7) }
	static var FlagsSampleFractionMask : LinearPCMFormatFlag { return self(rawValue: 0x3F << FlagsSampleFractionShift.rawValue) }
}

extension AudioStreamBasicDescription {
	internal var audioFormatNativeEndian: Bool {
		get {
			if (formatID == .LinearPCM) {
				let ourFlags = formatFlags & .BigEndian
				if ourFlags == .NativeEndian {
					return true
				}
			}
			return false
		}
	}
	
	internal var formatID: AudioFormat {
		get {
			return AudioFormat(rawValue: mFormatID)!
		}
		set {
			mFormatID = newValue.rawValue
		}
	}
	
	internal var formatFlags: AudioFormatFlag {
		get {
			return AudioFormatFlag.fromMask(mFormatFlags)
		}
		set {
			mFormatFlags = newValue.rawValue
		}
	}
	
	internal init() {
		mSampleRate = 0
		mFormatID = 0
		mFormatFlags = 0
		mBytesPerPacket = 0
		mFramesPerPacket = 1
		mBytesPerFrame = 1
		mChannelsPerFrame = 1
		mBitsPerChannel = 1
		mReserved = 0
	}
	
	internal init(sampleRate: Float64, formatID: AudioFormat = .LinearPCM, formatFlags: AudioFormatFlag = .NativeFloatPacked, bitsPerChannel: UInt32, channelsPerFrame: UInt32, framesPerPacket: UInt32 = 1) {
		mSampleRate = sampleRate
		mFormatID = formatID.rawValue
		mFormatFlags = formatFlags.rawValue
		mBitsPerChannel = bitsPerChannel
		mChannelsPerFrame = channelsPerFrame
		mFramesPerPacket = framesPerPacket
		mBytesPerFrame = mBitsPerChannel * mChannelsPerFrame / 8
		mBytesPerPacket = mBytesPerFrame * mFramesPerPacket
		mReserved = 0
	}

	internal init(sampleRate: Float64, formatID: Int, formatFlags: Int, bitsPerChannel: UInt32, channelsPerFrame: UInt32, framesPerPacket: UInt32 = 1) {
		mSampleRate = sampleRate
		mFormatID = UInt32(formatID)
		mFormatFlags = UInt32(formatFlags)
		mBitsPerChannel = bitsPerChannel
		mChannelsPerFrame = channelsPerFrame
		mFramesPerPacket = framesPerPacket
		mBytesPerFrame = mBitsPerChannel * mChannelsPerFrame / 8
		mBytesPerPacket = mBytesPerFrame * mFramesPerPacket
		mReserved = 0
	}
}
