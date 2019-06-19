/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/soundcard.h,v $
 * $Name:  $
 *
 * $Revision: 1.6 $     $Date: 2004/02/06 05:46:44 $
 * 
\**************************************************************************/

#ifndef _HOPEN_SOUNDCARD_H_
#define _HOPEN_SOUNDCARD_H_

/*
 * Minor numbers for the sound driver.
 */

#define SND_DEV_CTL			0	/* Control port /dev/mixer */
#define SND_DEV_SEQ			1	/* Sequencer output /dev/sequencer (FM
							synthesizer and MIDI output) */
#define SND_DEV_MIDIN		2	/* Raw midi access */
#define SND_DEV_DSP			3	/* Digitized voice /dev/dsp */
#define SND_DEV_AUDIO		4	/* Sparc compatible /dev/audio */
#define SND_DEV_DSP16		5	/* Like /dev/dsp but 16 bits/sample */
#define SND_DEV_UNUSED		6
#define SND_DEV_AWFM		7	/* Reserved */
#define SND_DEV_SEQ2		8	/* /dev/sequencer, level 2 interface */
#define SND_DEV_SYNTH		9	/* Raw synth access /dev/synth (same as /dev/dmfm) */
#define SND_DEV_DMFM		10	/* Raw synth access /dev/dmfm */
#define SND_DEV_UNKNOWN11	11


#define	_SIO		_IO
#define	_SIOR		_IOR
#define	_SIOW		_IOW
#define	_SIOWR		_IOWR

#define _SIOC_SIZE	_IOC_SIZE
#define _SIOC_DIR	_IOC_DIR
#define _SIOC_READ	_IOC_READ
#define _SIOC_WRITE	_IOC_WRITE


/********************************************
 * IOCTL commands for /dev/dsp and /dev/audio
 */

#define SNDCTL_DSP_RESET			_SIO  ('P', 0)
#define SNDCTL_DSP_SYNC				_SIO  ('P', 1)
#define SNDCTL_DSP_SPEED			_SIOWR('P', 2, int)
#define SNDCTL_DSP_STEREO			_SIOWR('P', 3, int)
#define SNDCTL_DSP_GETBLKSIZE		_SIOWR('P', 4, int)
#define SNDCTL_DSP_SAMPLESIZE		SNDCTL_DSP_SETFMT
#define SNDCTL_DSP_CHANNELS			_SIOWR('P', 6, int)
#define SOUND_PCM_WRITE_CHANNELS	SNDCTL_DSP_CHANNELS
#define SOUND_PCM_WRITE_FILTER		_SIOWR('P', 7, int)
#define SNDCTL_DSP_POST				_SIO  ('P', 8)
#define SNDCTL_DSP_SUBDIVIDE		_SIOWR('P', 9, int)
#define SNDCTL_DSP_SETFRAGMENT		_SIOWR('P',10, int)

/*	Audio data formats (Note! U8=8 and S16_LE=16 for compatibility) */
#define SNDCTL_DSP_GETFMTS		_SIOR ('P',11, int) /* Returns a mask */
#define SNDCTL_DSP_SETFMT		_SIOWR('P',5, int) /* Selects ONE fmt*/
	#define AFMT_QUERY			0x00000000	/* Return current fmt */
	#define AFMT_MU_LAW			0x00000001
	#define AFMT_A_LAW			0x00000002
	#define AFMT_IMA_ADPCM		0x00000004
	#define AFMT_U8				0x00000008
	#define AFMT_S16_LE			0x00000010	/* Little endian signed 16*/
	#define AFMT_S16_BE			0x00000020	/* Big endian signed 16 */
	#define AFMT_S8				0x00000040
	#define AFMT_U16_LE			0x00000080	/* Little endian U16 */
	#define AFMT_U16_BE			0x00000100	/* Big endian U16 */
	#define AFMT_MPEG			0x00000200	/* MPEG (2) audio */
	#define AFMT_AC3			0x00000400	/* Dolby Digital AC3 */

#define SNDCTL_DSP_NONBLOCK		_SIO  ('P',14)
#define SNDCTL_DSP_GETCAPS		_SIOR ('P',15, int)
	#define DSP_CAP_REVISION		0x000000ff	/* Bits for revision level (0 to 255) */
	#define DSP_CAP_DUPLEX		0x00000100	/* Full duplex record/playback */
	#define DSP_CAP_REALTIME		0x00000200	/* Real time capability */
	#define DSP_CAP_BATCH		0x00000400	/* Device has some kind of */
								/* internal buffers which may */
								/* cause some delays and */
								/* decrease precision of timing */
	#define DSP_CAP_COPROC		0x00000800	/* Has a coprocessor */
								/* Sometimes it's a DSP */
								/* but usually not */
	#define DSP_CAP_TRIGGER		0x00001000	/* Supports SETTRIGGER */
	#define DSP_CAP_MMAP		0x00002000	/* Supports mmap() */
	#define DSP_CAP_MULTI		0x00004000	/* support multiple open */
	#define DSP_CAP_BIND		0x00008000	/* channel binding to front/rear/cneter/lfe */


#define SNDCTL_DSP_GETTRIGGER		_SIOR ('P',16, int)
#define SNDCTL_DSP_SETTRIGGER		_SIOW ('P',16, int)
	#define PCM_ENABLE_INPUT		0x00000001
	#define PCM_ENABLE_OUTPUT		0x00000002

#define SNDCTL_DSP_SETSYNCRO		_SIO  ('P', 21)
#define SNDCTL_DSP_SETDUPLEX		_SIO  ('P', 22)
#define SNDCTL_DSP_GETODELAY		_SIOR ('P', 23, int)

#define SNDCTL_SET_INTERNAL_CALL			_SIO  ('P', 24)
#define SNDCTL_SET_INTERNAL_TONE			_SIO  ('P', 25)
#define SNDCTL_SET_INTERNAL_BOTH			_SIO  ('P', 26)
#define SNDCTL_SET_EXTERNAL_CALL			_SIO  ('P', 27)
#define SNDCTL_SET_EXTERNAL_TONE			_SIO  ('P', 28)
#define SNDCTL_SET_EXTERNAL_BOTH			_SIO  ('P', 29)

/*********************************************
 * IOCTL commands for /dev/mixer
 */
	
/* 
 * Mixer devices
 *
 * There can be up to 20 different analog mixer channels. The
 * SOUND_MIXER_NRDEVICES gives the currently supported maximum. 
 * The SOUND_MIXER_READ_DEVMASK returns a bitmask which tells
 * the devices supported by the particular mixer.
 */

#define SOUND_MIXER_NRDEVICES	25
#define SOUND_MIXER_VOLUME	0
#define SOUND_MIXER_BASS	1
#define SOUND_MIXER_TREBLE	2
#define SOUND_MIXER_SYNTH	3
#define SOUND_MIXER_PCM		4
#define SOUND_MIXER_SPEAKER	5
#define SOUND_MIXER_LINE	6
#define SOUND_MIXER_MIC		7
#define SOUND_MIXER_CD		8
#define SOUND_MIXER_IMIX	9	/*  Recording monitor  */
#define SOUND_MIXER_ALTPCM	10
#define SOUND_MIXER_RECLEV	11	/* Recording level */
#define SOUND_MIXER_IGAIN	12	/* Input gain */
#define SOUND_MIXER_OGAIN	13	/* Output gain */
/* 
 * The AD1848 codec and compatibles have three line level inputs
 * (line, aux1 and aux2). Since each card manufacturer have assigned
 * different meanings to these inputs, it's inpractical to assign
 * specific meanings (line, cd, synth etc.) to them.
 */
#define SOUND_MIXER_LINE1	14	/* Input source 1  (aux1) */
#define SOUND_MIXER_LINE2	15	/* Input source 2  (aux2) */
#define SOUND_MIXER_LINE3	16	/* Input source 3  (line) */
#define SOUND_MIXER_DIGITAL1	17	/* Digital (input) 1 */
#define SOUND_MIXER_DIGITAL2	18	/* Digital (input) 2 */
#define SOUND_MIXER_DIGITAL3	19	/* Digital (input) 3 */
#define SOUND_MIXER_PHONEIN	20	/* Phone input */
#define SOUND_MIXER_PHONEOUT	21	/* Phone output */
#define SOUND_MIXER_VIDEO	22	/* Video/TV (audio) in */
#define SOUND_MIXER_RADIO	23	/* Radio in */
#define SOUND_MIXER_MONITOR	24	/* Monitor (usually mic) volume */

/* Some on/off settings (SOUND_SPECIAL_MIN - SOUND_SPECIAL_MAX) */
/* Not counted to SOUND_MIXER_NRDEVICES, but use the same number space */
#define SOUND_ONOFF_MIN		28
#define SOUND_ONOFF_MAX		30

/* Note!	Number 31 cannot be used since the sign bit is reserved */
#define SOUND_MIXER_NONE	31

/*
 * The following unsupported macros are no longer functional.
 * Use SOUND_MIXER_PRIVATE# macros in future.
 */
#define SOUND_MIXER_ENHANCE	SOUND_MIXER_NONE
#define SOUND_MIXER_MUTE	SOUND_MIXER_NONE
#define SOUND_MIXER_LOUD	SOUND_MIXER_NONE


#define SOUND_DEVICE_LABELS	{"Vol  ", "Bass ", "Trebl", "Synth", "Pcm  ", "Spkr ", "Line ", \
				 "Mic  ", "CD   ", "Mix  ", "Pcm2 ", "Rec  ", "IGain", "OGain", \
				 "Line1", "Line2", "Line3", "Digital1", "Digital2", "Digital3", \
				 "PhoneIn", "PhoneOut", "Video", "Radio", "Monitor"}

#define SOUND_DEVICE_NAMES	{"vol", "bass", "treble", "synth", "pcm", "speaker", "line", \
				 "mic", "cd", "mix", "pcm2", "rec", "igain", "ogain", \
				 "line1", "line2", "line3", "dig1", "dig2", "dig3", \
				 "phin", "phout", "video", "radio", "monitor"}

/*	Device bitmask identifiers	*/

#define SOUND_MIXER_RECSRC	0xff	/* Arg contains a bit for each recording source */
#define SOUND_MIXER_DEVMASK	0xfe	/* Arg contains a bit for each supported device */
#define SOUND_MIXER_RECMASK	0xfd	/* Arg contains a bit for each supported recording source */
#define SOUND_MIXER_CAPS	0xfc
#	define SOUND_CAP_EXCL_INPUT	0x00000001	/* Only one recording source at a time */
#define SOUND_MIXER_STEREODEVS	0xfb	/* Mixer channels supporting stereo */
#define SOUND_MIXER_OUTSRC	0xfa	/* Arg contains a bit for each input source to output */
#define SOUND_MIXER_OUTMASK	0xf9	/* Arg contains a bit for each supported input source to output */

/*	Device mask bits	*/

#define SOUND_MASK_VOLUME	(1 << SOUND_MIXER_VOLUME)
#define SOUND_MASK_BASS		(1 << SOUND_MIXER_BASS)
#define SOUND_MASK_TREBLE	(1 << SOUND_MIXER_TREBLE)
#define SOUND_MASK_SYNTH	(1 << SOUND_MIXER_SYNTH)
#define SOUND_MASK_PCM		(1 << SOUND_MIXER_PCM)
#define SOUND_MASK_SPEAKER	(1 << SOUND_MIXER_SPEAKER)
#define SOUND_MASK_LINE		(1 << SOUND_MIXER_LINE)
#define SOUND_MASK_MIC		(1 << SOUND_MIXER_MIC)
#define SOUND_MASK_CD		(1 << SOUND_MIXER_CD)
#define SOUND_MASK_IMIX		(1 << SOUND_MIXER_IMIX)
#define SOUND_MASK_ALTPCM	(1 << SOUND_MIXER_ALTPCM)
#define SOUND_MASK_RECLEV	(1 << SOUND_MIXER_RECLEV)
#define SOUND_MASK_IGAIN	(1 << SOUND_MIXER_IGAIN)
#define SOUND_MASK_OGAIN	(1 << SOUND_MIXER_OGAIN)
#define SOUND_MASK_LINE1	(1 << SOUND_MIXER_LINE1)
#define SOUND_MASK_LINE2	(1 << SOUND_MIXER_LINE2)
#define SOUND_MASK_LINE3	(1 << SOUND_MIXER_LINE3)
#define SOUND_MASK_DIGITAL1	(1 << SOUND_MIXER_DIGITAL1)
#define SOUND_MASK_DIGITAL2	(1 << SOUND_MIXER_DIGITAL2)
#define SOUND_MASK_DIGITAL3	(1 << SOUND_MIXER_DIGITAL3)
#define SOUND_MASK_PHONEIN	(1 << SOUND_MIXER_PHONEIN)
#define SOUND_MASK_PHONEOUT	(1 << SOUND_MIXER_PHONEOUT)
#define SOUND_MASK_RADIO	(1 << SOUND_MIXER_RADIO)
#define SOUND_MASK_VIDEO	(1 << SOUND_MIXER_VIDEO)
#define SOUND_MASK_MONITOR	(1 << SOUND_MIXER_MONITOR)

/* Obsolete macros */
#define SOUND_MASK_MUTE		(1 << SOUND_MIXER_MUTE)
#define SOUND_MASK_ENHANCE	(1 << SOUND_MIXER_ENHANCE)
#define SOUND_MASK_LOUD		(1 << SOUND_MIXER_LOUD)

#define MIXER_READ(dev)		_SIOR('M', dev, int)
#define SOUND_MIXER_READ_VOLUME		MIXER_READ(SOUND_MIXER_VOLUME)
#define SOUND_MIXER_READ_BASS		MIXER_READ(SOUND_MIXER_BASS)
#define SOUND_MIXER_READ_TREBLE		MIXER_READ(SOUND_MIXER_TREBLE)
#define SOUND_MIXER_READ_SYNTH		MIXER_READ(SOUND_MIXER_SYNTH)
#define SOUND_MIXER_READ_PCM		MIXER_READ(SOUND_MIXER_PCM)
#define SOUND_MIXER_READ_SPEAKER	MIXER_READ(SOUND_MIXER_SPEAKER)
#define SOUND_MIXER_READ_LINE		MIXER_READ(SOUND_MIXER_LINE)
#define SOUND_MIXER_READ_MIC		MIXER_READ(SOUND_MIXER_MIC)
#define SOUND_MIXER_READ_CD		MIXER_READ(SOUND_MIXER_CD)
#define SOUND_MIXER_READ_IMIX		MIXER_READ(SOUND_MIXER_IMIX)
#define SOUND_MIXER_READ_ALTPCM		MIXER_READ(SOUND_MIXER_ALTPCM)
#define SOUND_MIXER_READ_RECLEV		MIXER_READ(SOUND_MIXER_RECLEV)
#define SOUND_MIXER_READ_IGAIN		MIXER_READ(SOUND_MIXER_IGAIN)
#define SOUND_MIXER_READ_OGAIN		MIXER_READ(SOUND_MIXER_OGAIN)
#define SOUND_MIXER_READ_LINE1		MIXER_READ(SOUND_MIXER_LINE1)
#define SOUND_MIXER_READ_LINE2		MIXER_READ(SOUND_MIXER_LINE2)
#define SOUND_MIXER_READ_LINE3		MIXER_READ(SOUND_MIXER_LINE3)

/* Obsolete macros */
#define SOUND_MIXER_READ_MUTE		MIXER_READ(SOUND_MIXER_MUTE)
#define SOUND_MIXER_READ_ENHANCE	MIXER_READ(SOUND_MIXER_ENHANCE)
#define SOUND_MIXER_READ_LOUD		MIXER_READ(SOUND_MIXER_LOUD)

#define SOUND_MIXER_READ_RECSRC		MIXER_READ(SOUND_MIXER_RECSRC)
#define SOUND_MIXER_READ_DEVMASK	MIXER_READ(SOUND_MIXER_DEVMASK)
#define SOUND_MIXER_READ_RECMASK	MIXER_READ(SOUND_MIXER_RECMASK)
#define SOUND_MIXER_READ_STEREODEVS	MIXER_READ(SOUND_MIXER_STEREODEVS)
#define SOUND_MIXER_READ_CAPS		MIXER_READ(SOUND_MIXER_CAPS)

#define MIXER_WRITE(dev)		_SIOWR('M', dev, int)
#define SOUND_MIXER_WRITE_VOLUME	MIXER_WRITE(SOUND_MIXER_VOLUME)
#define SOUND_MIXER_WRITE_BASS		MIXER_WRITE(SOUND_MIXER_BASS)
#define SOUND_MIXER_WRITE_TREBLE	MIXER_WRITE(SOUND_MIXER_TREBLE)
#define SOUND_MIXER_WRITE_SYNTH		MIXER_WRITE(SOUND_MIXER_SYNTH)
#define SOUND_MIXER_WRITE_PCM		MIXER_WRITE(SOUND_MIXER_PCM)
#define SOUND_MIXER_WRITE_SPEAKER	MIXER_WRITE(SOUND_MIXER_SPEAKER)
#define SOUND_MIXER_WRITE_LINE		MIXER_WRITE(SOUND_MIXER_LINE)
#define SOUND_MIXER_WRITE_MIC		MIXER_WRITE(SOUND_MIXER_MIC)
#define SOUND_MIXER_WRITE_CD		MIXER_WRITE(SOUND_MIXER_CD)
#define SOUND_MIXER_WRITE_IMIX		MIXER_WRITE(SOUND_MIXER_IMIX)
#define SOUND_MIXER_WRITE_ALTPCM	MIXER_WRITE(SOUND_MIXER_ALTPCM)
#define SOUND_MIXER_WRITE_RECLEV	MIXER_WRITE(SOUND_MIXER_RECLEV)
#define SOUND_MIXER_WRITE_IGAIN		MIXER_WRITE(SOUND_MIXER_IGAIN)
#define SOUND_MIXER_WRITE_OGAIN		MIXER_WRITE(SOUND_MIXER_OGAIN)
#define SOUND_MIXER_WRITE_LINE1		MIXER_WRITE(SOUND_MIXER_LINE1)
#define SOUND_MIXER_WRITE_LINE2		MIXER_WRITE(SOUND_MIXER_LINE2)
#define SOUND_MIXER_WRITE_LINE3		MIXER_WRITE(SOUND_MIXER_LINE3)

#define SOUND_MIXER_WRITE_RECSRC	MIXER_WRITE(SOUND_MIXER_RECSRC)

typedef struct mixer_info
{
  char id[16];
  char name[32];
  int  modify_counter;
  int fillers[10];
} mixer_info;

typedef struct _old_mixer_info /* Obsolete */
{
  char id[16];
  char name[32];
} _old_mixer_info;

#define SOUND_MIXER_INFO		_SIOR ('M', 101, mixer_info)
#define SOUND_OLD_MIXER_INFO		_SIOR ('M', 101, _old_mixer_info)

/*
 * A mechanism for accessing "proprietary" mixer features. This method
 * permits passing 128 bytes of arbitrary data between a mixer application
 * and the mixer driver. Interpretation of the record is defined by
 * the particular mixer driver.
 */
typedef unsigned char mixer_record[128];

#define SOUND_MIXER_ACCESS		_SIOWR('M', 102, mixer_record)

/*
 * Two ioctls for special souncard function
 */
#define SOUND_MIXER_AGC  _SIOWR('M', 103, int)
#define SOUND_MIXER_3DSE  _SIOWR('M', 104, int)

/*
 * The SOUND_MIXER_PRIVATE# commands can be redefined by low level drivers.
 * These features can be used when accessing device specific features.
 */
#define SOUND_MIXER_PRIVATE1		_SIOWR('M', 111, int)
#define SOUND_MIXER_PRIVATE2		_SIOWR('M', 112, int)
#define SOUND_MIXER_PRIVATE3		_SIOWR('M', 113, int)
#define SOUND_MIXER_PRIVATE4		_SIOWR('M', 114, int)
#define SOUND_MIXER_PRIVATE5		_SIOWR('M', 115, int)

/*
 * SOUND_MIXER_GETLEVELS and SOUND_MIXER_SETLEVELS calls can be used
 * for querying current mixer settings from the driver and for loading
 * default volume settings _prior_ activating the mixer (loading
 * doesn't affect current state of the mixer hardware). These calls
 * are for internal use only.
 */

typedef struct mixer_vol_table {
  int num;	/* Index to volume table */
  char name[32];
  int levels[32];
} mixer_vol_table;

#define SOUND_MIXER_GETLEVELS		_SIOWR('M', 116, mixer_vol_table)
#define SOUND_MIXER_SETLEVELS		_SIOWR('M', 117, mixer_vol_table)

/* 
 * An ioctl for identifying the driver version. It will return value
 * of the SOUND_VERSION macro used when compiling the driver.
 * This call was introduced in OSS version 3.6 and it will not work
 * with earlier versions (returns EINVAL).
 */
#define OSS_GETVERSION			_SIOR ('M', 118, int)

/***********************************************************\
*	these defines are for ak4538 codec 
\***********************************************************/
//////////////////////////////////sample rate definition///////////
#define		SAMPLE_RATE_8000	8000
#define		SAMPLE_RATE_11025	11025
#define		SAMPLE_RATE_16000	16000
#define		SAMPLE_RATE_22050	22050
#define		SAMPLE_RATE_24000	24000
#define 	SAMPLE_RATE_32000	32000
#define		SAMPLE_RATE_44100	44100
#define		SAMPLE_RATE_48000	48000

#define		SWITCH_CHANNEL		_SIOW('M', 120, int)
#define		GET_CHANNEL		_SIOW('M', 121, int)
#define		SHUT_CHANNEL		_SIOW('M', 122, int)

#define 	INTMIC_TO_ADC		0x01
#define		INTMIC_TO_MOUT		0x02
#define 	EXTMIC_TO_ADC		0x04
#define		EXTMIC_TO_MOUT		0x08
#define		AUXIN_TO_ADC		0x10
#define		AUXIN_TO_HEADPH		0x20
#define		DAC_TO_HEADPH		0x40
#define		DAC_TO_SPK		0x80
#define		ALL_BLK_OFF		-1

/*
#define 	INTMIC_TO_ADC		_SIO('M', 120)
#define		INTMIC_TO_MOUT		_SIO('M', 121)
#define 	EXTMIC_TO_ADC		_SIO('M', 122)
#define		EXTMIC_TO_MOUT		_SIO('M', 123)
#define		AUXIN_TO_ADC		_SIO('M', 124)
#define		AUXIN_TO_HEADPH		_SIO('M', 125)
#define		DAC_TO_HEADPH		_SIO('M', 126)
#define		DAC_TO_SPK		_SIO('M', 127)
*/

#endif // _HOPEN_SOUNDCARD_H_

