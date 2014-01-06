#ifndef __PAT_H
#define __PAT_H

#pragma pack(push, 2)

typedef struct _PatchHeader
{
	char	ID[12];
	char	GravisID[10];
	char	Description[60];
	
	Byte	InsNo;
	Byte	VoiceNo;
	Byte	Channels;
	
	Byte	HiSamp;
	Byte	LoSamp;
	
	Byte	HiVol;
	Byte	LoVol;
	
	Byte	Size1;
	Byte	Size2;
	Byte	Size3;
	Byte	Size4;
	
	char	reserved[36];
} PatchHeader;

typedef struct _PatInsHeader
{
	short	ID;
	char	name[16];
	SInt32	size;
	Byte	layer;
	char	reserved[40];
} PatInsHeader;

typedef struct _LayerHeader
{
	Byte	dup;
	Byte	id;
	SInt32	size;
	Byte	SampNo;
	char	reserved[40];
} LayerHeader;

typedef struct _PatSampHeader
{
	char			name[ 7];
	Byte			fractions;
	SInt32			size;
	SInt32			startLoop;
	SInt32			endLoop;
	unsigned short	rate;
	SInt32			minFreq;
	SInt32			maxFreq;
	SInt32			originRate;
	short			tune;
	Byte			balance;
	Byte			Filter[6];
	Byte			FilterOffset[6];
	Byte			TremoloSweep;
	Byte			TremoloRate;
	Byte			TremoloDepth;
	Byte			VibratoSweep;
	Byte			VibratoRate;
	Byte			VibratoDepth;
	
	Byte			Flag;
	short			FreqScale;
	unsigned short	FreqScaleFactor;
	char			reserved[36];
	
} PatSampHeader;

#pragma pack(pop)

/*
Document converted to plain ASCII for inclusion in Wotsit's Format

{                     ** FORTE's patch format, v1.10 **                      }
{   (compiled from various sources, most notably Francois Dion's PATCHKIT)   }



{  Patch file structure:

+-------------------------------+
|  PatchHeader                  |
+-------------------------------+
|  instruments *                |
|  +----------------------------+--+
|  |  InstrumentHeader             |
|  +-------------------------------+
|  |  layers *                     |
|  |  +----------------------------+--+
|  |  |  LayerHeader                  |
|  |  +-------------------------------+
|  |  |  samples *                    |
|  |  |  +----------------------------+--+
+--+  |  |  WaveHeader                   |
   +--+  +-------------------------------+
      +--+  The actual sample data       |
         +-------------------------------+ }


type
  PatchHeader = record
    header        : array[0..11] of Char; { asciz, Currently 'GF1PATCH110'#0  }
    gravis_id     : array[0..9] of Char;  { asciz, Always 'ID#000002'         }
    description   : array[0..59] of Char; { asciz, Whatever you want, (Gravis }
                                          { uses it for a copyright notice)   }
    instruments   : Byte;                 { 1, number of instruments in patch }
                                          { Could be > 1 but why such a thing?}
    voices        : Byte;                 { 14, Playback voices, ignored      }
    channels      : Byte;                 { 0, ignored, Wav channels that can }
                                          { be played beside the patch        }
    waveforms     : Word;                 { Total number of waveforms in file }
    master_volume : Word;                 { Master Volume, 00..7F             }
    data_size     : DWord;                { Memory patch takes in gusram?     }
    reserved      : array[0..35] of Byte; { For future extensions }
  end;

  InstrumentHeader : record
    instrument_id  : Word;                { Instrument id: 0..FFFF            }
    instrument_name: array[0..15] of Char;{ Name of instrument                }
                                          { Not used in patches of 2.06 disks }
                                          { but you SHOULD use it, This is    }
                                          { what's seen when you choose  'Use }
                                          { Names from Patch files' in the Win}
                                          { Patch Manager, and it will become }
                                          { essential once we have bank-switch}
                                          { and not only GM-sounds            }
    instrument_size: DWord;               { Number of bytes for the instrument}
                                          { incl. header.To skip to next instr}
    layers         : Byte;                { 1, Number of layers in instr: 1-4 }
                                          { Current drivers only support 1    }
    reserved       : array[0..39] of Byte;{ For future extensions             }
  end;

  LayerHeader = record
    layer_dup : Byte;             { If not 0 the wavesample to use is from the }
			 	  { previous layer. Waveheader is still needed }
    layer_id  : Byte;             { 0, Layer id: 0..3                          }
    layer_size: DWord;            { Data size in bytes in the layer, excluding }
                                  { the header. To skip to next layer          }
    samples   : Byte;             { number of wavesamples                      }
    reserved  : array[0..39] of Byte; { For future extensions                  }
  end;

  WaveHeader = record
    wave_name : array[0..6] of Char; { asciz. name of the wave. 'NoName'#0 if }
                                     { none, use 'C3', 'High' and such names  }
    fractions      : Byte;        { Start loop point fraction in 4 bits + End }
                                  { loop point fraction in the 4 other bits;  }
				  { used when looppoint is between two samples}
    wave_size      : DWord;       { Total size of wavesample. Limited to 64KB }
                                  { now by the drivers,card can do up to 256KB}
    start_loop     : DWord;       { Start loop position                       }
    end_loop       : DWord;       { End loop position                         }
    sample_rate    : Word;        { Sample frequency, in Hz                   }
    low_frequency  : DWord;       { Lowest frequency to play, see scale_table }
    high_frequency : DWord;       { Highest frequency to play, see scale_table}
    root_frequency : DWord;       { Root frequency to play, see scale_table   }
                                  { The 'frequency' wich correspond to the    }
                                  { sample_rate.(this is the freq that sounds }
                                  { in the drum bank are played at)           }
    tune           : Integer;     { Always 1, not used anymore                }
    balance        : Byte;        { Pan, 0..15, (0-full left, 15-full right)  }
    envelope_rate  : array[0..5] of Byte; { attack rates                      }
    envelope_offset: array[0..5] of Byte; { attack offsets                    }
    { I have not been able to fully understand the envelope bytes:            }
    { The rates are the rate of amplitude change (I think the alowed range is }
    { 1..3F for point 1-5 and 1..CF for point 6 or something...) wich goes on }
    { until the corresponding 'offset' (what time unit?) is reached (maybe its}
    { somekind of dy/dx desciption).    Different parts of the  envelope  are }
    { enabled/disabled via the mode byte below.You should try Gravis PATCH.EXE}
    { (from the SDK) or another patch editor to fiddle around with it a little}
    tremolo_sweep  : Byte;          { tremolo sweep (sensitivity? amplitude?) }
    tremolo_rate   : Byte;          { tremolo rate  }
    tremolo_depth  : Byte;          { tremolo depth }
    vibrato_sweep  : Byte;          { vibrato sweep  }
    vibrato_rate   : Byte;          { vibrato rate (lfo - sine) }
    vibrato_depth  : Byte;          { vibrato depth }
    modes          : Byte;          { A set of flags: }
    { bit 0 = 0=8 bit, 1=16 bit wave data.                                    }
    { bit 1 = 0=Signed data, 1=Unsigned data.                                 }
    { bit 2 = 1=Enable looping                                                }
    { bit 3 = 1=Enable bidirectional looping                                  }
    { bit 4 = 1=Enable looping backward                                       }
    { bit 5 = 1=Enable sustain (3rd point of envelope)                        }
    { bit 6 = 1=Enable envelopes, must be set if you want to hear anything    }
    { bit 7 = 1=Enable clamped release (6th point of envelope) }
    scale_frequency: Integer;       { 60, Dunno what it is }
    scale_factor   : Word;          { from 0 to 2048, 1024 is normal          }
                                    { freq := (freq*scale_factor) shr 10 }
    reserved: array[0..35] of Byte; { For future extensions }
  end;


const
  scale_table : array[0..107] of DWord = (  {C-0..B-8}
  { Octave 0 }  16351, 17323, 18354, 19445, 20601, 21826, 23124, 24499, 25956, 27500, 29135, 30867,
  { Octave 1 }  32703, 34647, 36708, 38890, 41203, 43653, 46249, 48999, 51913, 54999, 58270, 61735,
  { Octave 2 }  65406, 69295, 73416, 77781, 82406, 87306, 92498, 97998, 103826, 109999, 116540, 123470,
  { Octave 3 }  130812, 138591, 146832, 155563, 164813, 174614, 184997, 195997, 207652, 219999, 233081, 246941,
  { Octave 4 }  261625, 277182, 293664, 311126, 329627, 349228, 369994, 391995, 415304, 440000, 466163, 493883,
  { Octave 5 }  523251, 554365, 587329, 622254, 659255, 698456, 739989, 783991, 830609, 880000, 932328, 987767,
  { Octave 6 }  1046503, 1108731, 1174660, 1244509, 1318511, 1396914, 1479979, 1567983, 1661220, 1760002, 1864657, 1975536,
  { Octave 7 }  2093007, 2217464, 2349321, 2489019, 2637024, 2793830, 2959960, 3135968, 3322443, 3520006, 3729316, 3951073,
  { Octave 8 }  4186073, 4434930, 4698645, 4978041, 5274051, 5587663, 5919922, 6271939, 6644889, 7040015, 7458636, 7902150 );
*/

#endif
