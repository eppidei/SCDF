/*
 * UsbSpecCodes.h
 *
 *  Created on: 11/dic/2013
 *      Author: athos
 */

#ifndef CODESANDSELECTORS_H_
#define CODESANDSELECTORS_H_

// Audio Interface Class Code
#define USB_CLASS_AUDIO		0x01
// Audio Interface Subclass Codes (added USB_SUBCLASS_)
#define USB_SUBCLASS_UNDEFINED		0x00
#define USB_SUBCLASS_AUDIOCONTROL	0x01
#define USB_SUBCLASS_AUDIOSTREAMING	0x02
#define USB_SUBCLASS_MIDISTREAMING	0x03

// Audio Interface Protocol Code
#define USB_AUDIO_CLASS_1		0x00
#define USB_AUDIO_CLASS_2		0x20
// Audio Function Class Code
#define USB_AUDIO_FUNCTION	USB_CLASS_AUDIO
//Audio Function Subclass Code
#define FUNCTION_SUBCLASS_UNDEFINED	0x00
//Audio Function Protocol Codes
#define FUNCTION_PROTOCOL_UNDEFINED 0x00
#define AF_VERSION_02_00 			IP_VERSION_02_00

//Audio Function Category Codes (added CAT_ )
#define CAT_UNDEFINED				0x00
#define CAT_DESKTOP_SPEAKER			0x01
#define CAT_HOME_THEATER			0x02
#define CAT_MICROPHONE				0x03
#define CAT_HEADSET					0x04
#define CAT_TELEPHONE				0x05
#define CAT_CONVERTER				0x06
#define CAT_VOICE_SOUND_RECORDER	0x07
#define CAT_IO_BOX					0x08
#define CAT_MUSICAL_INSTRUMENT		0x09
#define CAT_PRO_AUDIO				0x0A
#define CAT_AUDIO_VIDEO				0x0B
#define CAT_CONTROL_PANEL			0x0C
#define CAT_OTHER					0xFF

// Audio Class-Specific Descriptor Types (added DESC_TYPE_ )
#define DESC_TYPE_UNDEFINED			0x20
#define DESC_TYPE_DEVICE			0x21
#define DESC_TYPE_CONFIGURATION		0x22
#define DESC_TYPE_STRING			0x23
#define DESC_TYPE_INTERFACE			0x24
#define DESC_TYPE_ENDPOINT			0x25

// endpoint class specific descriptor subtypes:
#define EP_UNDEFINED 0x00
#define EP_GENERAL 0x01

// Audio Control (AC) Interface Descriptor Subtypes (added AC_DESC_ )
#define UAC_DESC_UNDEFINED				0x00
#define UAC_DESC_HEADER					0x01
#define UAC_DESC_INPUT_TERMINAL			0x02
#define UAC_DESC_OUTPUT_TERMINAL		0x03
#define UAC_DESC_MIXER_UNIT				0x04
#define UAC_DESC_SELECTOR_UNIT			0x05
#define UAC_DESC_FEATURE_UNIT			0x06
// UAC1-only subtypes:
#define UAC1_DESC_PROCESSING_UNIT		0x07
#define UAC1_DESC_EXTENSION_UNIT		0x08
// UAC2-only subtypes:
#define UAC2_DESC_EFFECT_UNIT			0x07
#define UAC2_DESC_PROCESSING_UNIT		0x08
#define UAC2_DESC_EXTENSION_UNIT		0x09
#define UAC2_DESC_CLOCK_SOURCE			0x0A
#define UAC2_DESC_CLOCK_SELECTOR		0x0B
#define UAC2_DESC_CLOCK_MULTIPLIER		0x0C
#define UAC2_DESC_SAMPLE_RATE_CONVERTER	0x0D

// Audio Streaming (AS) Interface Descriptor Subtypes (added AS_DESC_ )
#define AS_DESC_UNDEFINED	0x00
#define AS_DESC_GENERAL		0x01
#define AS_DESC_FORMAT_TYPE	0x02
#define AS_DESC_FORMAT_SPECIFIC 0x03
// UAC2 only:
#define AS_DESC_ENCODER		0x03
#define AS_DESC_DECODER		0x04

#define FORMAT_TYPE_UNDEFINED 0x00
#define FORMAT_TYPE_I	0x01
#define FORMAT_TYPE_II  0x02
#define FORMAT_TYPE_III 0x03

#define FORMAT_TYPE_I_UNDEFINED		0x0000
#define FORMAT_TYPE_I_PCM 			0x0001
#define FORMAT_TYPE_I_PCM8 			0x0002
#define FORMAT_TYPE_I_IEEE_FLOAT	0x0003
#define FORMAT_TYPE_I_ALAW			0x0004
#define FORMAT_TYPE_I_MULAW			0x0005

#define FORMAT_TYPE_II_UNDEFINED	0x1000
#define FORMAT_TYPE_II_MPEG			0x1001
#define FORMAT_TYPE_II_AC3			0x1002

#define FORMAT_TYPE_III_UNDEFINED		 0x2000
#define FORMAT_TYPE_III_AC3				 0x2001
#define FORMAT_TYPE_III_MPEG1_LAYER1	 0x2002
#define FORMAT_TYPE_III_MPEG1_LAYER23	 0x2003 // Mpeg 1 Layer2/3 or IEC1937_MPEG-2_NOEXT
#define FORMAT_TYPE_III_MPEG2_EXT		 0x2004
#define FORMAT_TYPE_III_MPEG2_LAYER1_LS	 0x2005
#define FORMAT_TYPE_III_MPEG2_LAYER23_LS 0x2006



// Effect Unit Effect Types (wEffectType) (added EU_TYPE_ )
#define EU_TYPE_UNDEFINED			0x00
#define EU_TYPE_PARAM_EQ_SECTION	0x01
#define EU_TYPE_REVERBERATION		0x02
#define EU_TYPE_MOD_DELAY			0x03
#define EU_TYPE_DYN_RANGE_COMP		0x04

// Processing Unit Process Types (wProcessType) (added PU_TYPE_ )
#define PU_TYPE_UNDEFINED		0x00
#define PU_TYPE_UP_DOWNMIX		0x01
#define PU_TYPE_DOLBY_PROLOGIC	0x02
#define PU_TYPE_STEREO_EXTENDER	0x03

// Audio Class-Specific Endpoint Descriptor Subtypes (added EP_SUBTYPE_ )
#define EP_SUBTYPE_UNDEFINED	0x00
#define EP_SUBTYPE_GENERAL		0x01

// Audio Class-Specific Request Codes (added REQ_ )
#define REQ_UNDEFINED	0x00
// for UAC2:
#define UAC2_CUR	0x01
#define UAC2_RANGE	0x02
#define UAC2_MEM	0x03
// for UAC1:
#define UAC1_SET_CUR	0x01
#define UAC1_GET_CUR	0x81
#define UAC1_SET_MIN	0x02
#define UAC1_GET_MIN	0x82
#define UAC1_SET_MAX	0x03
#define UAC1_GET_MAX	0x83
#define UAC1_SET_RES	0x04
#define UAC1_GET_RES	0x84
#define UAC1_SET_MEM	0x05
#define UAC1_GET_MEM	0x85
#define UAC1_GET_STAT	0xFF

// Encoder Type Codes
#define ENCODER_UNDEFINED	0x00
#define OTHER_ENCODER		0x01
#define MPEG_ENCODER		0x02
#define AC3_ENCODER			0x03
#define WMA_ENCODER			0x04
#define DTS_ENCODER			0x05

// Decoder Type Codes
#define DECODER_UNDEFINED	0x00
#define OTHER_DECODER		0x01
#define MPEG_DECODER		0x02
#define AC3_DECODER			0x03
#define WMA_DECODER			0x04
#define DTS_DECODER			0x05


// CONTROL SELECTOR CODES:

// AudioStreaming Interface Control Selectors
#define AS_CONTROL_UNDEFINED			0x00
#define AS_ACT_ALT_SETTING_CONTROL		0x01
#define AS_VAL_ALT_SETTINGS_CONTROL		0x02
#define AS_AUDIO_DATA_FORMAT_CONTROL	0x03

// Endpoint Control Selectors

#define EP_CONTROL_UNDEFINED	0x00
#define EP_CONTROL_SAMPLERATE	0x01
#define EP_CONTROL_PITCH 		0x02


// maybe for 2.0 its like this: (check!)
#define EP_PITCH_CONTROL			0x01
#define EP_DATA_OVERRUN_CONTROL		0x02
#define EP_DATA_UNDERRUN_CONTROL	0x03

// Clock Source Control Selectors
#define CS_CONTROL_UNDEFINED	0x00 // NOT USING THESE MACROS
#define CS_SAM_FREQ_CONTROL		0x01 // WE MADE ENUMS IN THE
#define CS_CLOCK_VALID_CONTROL	0x02 // CLOCK SOURCE CLASS

// Clock Selector Control Selectors
#define CX_CONTROL_UNDEFINED		0x00
#define CX_CLOCK_SELECTOR_CONTROL	0x01

// Clock Multiplier Control Selectors
#define CM_CONTROL_UNDEFINED	0x00
#define CM_NUMERATOR_CONTROL	0x01
#define CM_DENOMINATOR_CONTROL	0x02

// Terminal Control Selectors
#define TE_CONTROL_UNDEFINED	0x00
#define TE_COPY_PROTECT_CONTROL	0x01
#define TE_CONNECTOR_CONTROL	0x02
#define TE_OVERLOAD_CONTROL		0x03
#define TE_CLUSTER_CONTROL		0x04
#define TE_UNDERFLOW_CONTROL	0x05
#define TE_OVERFLOW_CONTROL		0x06
#define TE_LATENCY_CONTROL		0x07

//Feature Unit Control Selectors
#define FU_CTRL_UNDEFINED	0x00
#define FU_CTRL_MUTE		0x01
#define FU_CTRL_VOLUME		0x02
#define FU_CTRL_BASS		0x03
#define FU_CTRL_MID			0x04
#define FU_CTRL_TREBLE		0x05
#define FU_CTRL_EQ			0x06
#define FU_CTRL_AUTO_GAIN	0x07
#define FU_CTRL_DELAY		0x08
#define FU_CTRL_BASS_BOOST	0x09
#define FU_CTRL_LOUDNESS	0x0A
// only from usb 2.0:
#define FU_CTRL_INPUT_GAIN			0x0B
#define FU_CTRL_INPUT_GAIN_PAD		0x0C
#define FU_CTRL_PHASE_INVERTER		0x0D
#define FU_CTRL_UNDERFLOW			0x0E
#define FU_CTRL_OVERFLOW			0x0F
#define FU_CTRL_LATENCY				0x10

#define FU_CTRL_NUMBER_UAC_1  10
#define FU_CTRL_NUMBER_UAC_2  16

// Mixer Control Selectors
#define MU_CONTROL_UNDEFINED	0x00
#define MU_MIXER_CONTROL		0x01
#define MU_CLUSTER_CONTROL		0x02
#define MU_UNDERFLOW_CONTROL	0x03
#define MU_OVERFLOW_CONTROL		0x04
#define MU_LATENCY_CONTROL		0x05

// Selector Control Selectors
#define SU_CONTROL_UNDEFINED	0x00
#define SU_SELECTOR_CONTROL		0x01
#define SU_LATENCY_CONTROL		0x02

// Effect Unit Control Selectors
// Parametric Equalizer Section Effect Unit Control Selectors
#define PE_CONTROL_UNDEFINED	0x00
#define PE_ENABLE_CONTROL		0x01
#define PE_CENTERFREQ_CONTROL	0x02
#define PE_QFACTOR_CONTROL		0x03
#define PE_GAIN_CONTROL			0x04
#define PE_UNDERFLOW_CONTROL	0x05
#define PE_OVERFLOW_CONTROL		0x06
#define PE_LATENCY_CONTROL		0x07
// Reverberation Effect Unit Control Selectors
#define RV_CONTROL_UNDEFINED		0x00
#define RV_ENABLE_CONTROL			0x01
#define RV_TYPE_CONTROL				0x02
#define RV_LEVEL_CONTROL			0x03
#define RV_TIME_CONTROL				0x04
#define RV_FEEDBACK_CONTROL			0x05
#define RV_PREDELAY_CONTROL			0x06
#define RV_DENSITY_CONTROL			0x07
#define RV_HIFREQ_ROLLOFF_CONTROL	0x08
#define RV_UNDERFLOW_CONTROL		0x09
#define RV_OVERFLOW_CONTROL			0x0A
#define RV_LATENCY_CONTROL			0x0B
// Modulation Delay Effect Unit Control Selectors
#define MD_CONTROL_UNDEFINED	0x00
#define MD_ENABLE_CONTROL		0x01
#define MD_BALANCE_CONTROL		0x02
#define MD_RATE_CONTROL			0x03
#define MD_DEPTH_CONTROL		0x04
#define MD_TIME_CONTROL			0x05
#define MD_FEEDBACK_CONTROL		0x06
#define MD_UNDERFLOW_CONTROL	0x07
#define MD_OVERFLOW_CONTROL		0x08
#define MD_LATENCY_CONTROL		0x09
// Dynamic Range Compressor Effect Unit Control Selectors
#define DR_CONTROL_UNDEFINED		0x00
#define DR_ENABLE_CONTROL			0x01
#define DR_COMPRESSION_RATE_CONTROL	0x02
#define DR_MAXAMPL_CONTROL			0x03
#define DR_THRESHOLD_CONTROL		0x04
#define DR_ATTACK_TIME_CONTROL		0x05
#define DR_RELEASE_TIME_CONTROL		0x06
#define DR_UNDERFLOW_CONTROL		0x07
#define DR_OVERFLOW_CONTROL			0x08
#define DR_LATENCY_CONTROL			0x09
// (end of effect control selectors)

// Processing Unit Control Selectors
// Up/Down-mix Processing Unit Control Selectors
#define UD_CONTROL_UNDEFINED	0x00
#define UD_ENABLE_CONTROL		0x01
#define UD_MODE_SELECT_CONTROL	0x02
#define UD_CLUSTER_CONTROL		0x03
#define UD_UNDERFLOW_CONTROL	0x04
#define UD_OVERFLOW_CONTROL		0x05
#define UD_LATENCY_CONTROL		0x06
// Dolby PrologicTM Processing Unit Control Selectors
#define DP_CONTROL_UNDEFINED	0x00
#define DP_ENABLE_CONTROL		0x01
#define DP_MODE_SELECT_CONTROL	0x02
#define DP_CLUSTER_CONTROL		0x03
#define DP_UNDERFLOW_CONTROL	0x04
#define DP_OVERFLOW_CONTROL		0x05
#define DP_LATENCY_CONTROL		0x06
// Stereo Extender Processing Unit Control Selectors
#define ST_EXT_CONTROL_UNDEFINED	0x00
#define ST_EXT_ENABLE_CONTROL		0x01
#define ST_EXT_WIDTH_CONTROL		0x02
#define ST_EXT_UNDERFLOW_CONTROL	0x03
#define ST_EXT_OVERFLOW_CONTROL		0x04
#define ST_EXT_LATENCY_CONTROL		0x05
// (end of processing unit control selectors)

// Extension Unit Control Selectors
#define XU_CONTROL_UNDEFINED	0x00
#define XU_ENABLE_CONTROL		0x01
#define XU_CLUSTER_CONTROL		0x02
#define XU_UNDERFLOW_CONTROL	0x03
#define XU_OVERFLOW_CONTROL		0x04
#define XU_LATENCY_CONTROL		0x05

// Encoder Control Selectors
#define EN_CONTROL_UNDEFINED		0x00
#define EN_BIT_RATE_CONTROL			0x01
#define EN_QUALITY_CONTROL			0x02
#define EN_VBR_CONTROL				0x03
#define EN_TYPE_CONTROL				0x04
#define EN_UNDERFLOW_CONTROL		0x05
#define EN_OVERFLOW_CONTROL			0x06
#define EN_ENCODER_ERROR_CONTROL	0x07
#define EN_PARAM1_CONTROL			0x08
#define EN_PARAM2_CONTROL			0x09
#define EN_PARAM3_CONTROL			0x0A
#define EN_PARAM4_CONTROL			0x0B
#define EN_PARAM5_CONTROL			0x0C
#define EN_PARAM6_CONTROL			0x0D
#define EN_PARAM7_CONTROL			0x0E
#define EN_PARAM8_CONTROL			0x0F

// Decoder Control Selectors
// MPEG Decoder Control Selectors
#define MD_CONTROL_UNDEFINED		0x00
#define MD_DUAL_CHANNEL_CONTROL		0x01
#define MD_SECOND_STEREO_CONTROL	0x02
#define MD_MULTILINGUAL_CONTROL		0x03
#define MD_DYN_RANGE_CONTROL		0x04
#define MD_SCALING_CONTROL			0x05
#define MD_HILO_SCALING_CONTROL		0x06
#define MD_UNDERFLOW_CONTROL		0x07
#define MD_OVERFLOW_CONTROL			0x08
#define MD_DECODER_ERROR_CONTROL	0x09
// AC-3 Decoder Control Selectors
#define AD_CONTROL_UNDEFINED		0x00
#define AD_MODE_CONTROL				0x01
#define AD_DYN_RANGE_CONTROL		0x02
#define AD_SCALING_CONTROL			0x03
#define AD_HILO_SCALING_CONTROL		0x04
#define AD_UNDERFLOW_CONTROL		0x05
#define AD_OVERFLOW_CONTROL			0x06
#define AD_DECODER_ERROR_CONTROL	0x07
// WMA Decoder Control Selectors
#define WD_CONTROL_UNDEFINED		0x00
#define WD_UNDERFLOW_CONTROL		0x01
#define WD_OVERFLOW_CONTROL			0x02
#define WD_DECODER_ERROR_CONTROL	0x03
//DTS Decoder Control Selectors
#define DD_CONTROL_UNDEFINED		0x00
#define DD_UNDERFLOW_CONTROL		0x01
#define DD_OVERFLOW_CONTROL			0x02
#define DD_DECODER_ERROR_CONTROL	0x03
// (end of decoder control selectors)

// Usb Audio Terminal Types (1.0):
// http://www.usb.org/developers/devclass_docs/termt10.pdf
#define TT_USB_UNDEFINED	0x0100
#define TT_USB_STREAMING	0x0101
#define TT_USB_VENDOR_SPEC	0x01FF
#define TT_INPUT_UNDEFINED	0x0200
#define TT_MICROPHONE		0x0201
#define TT_DESKTOP_MIC		0x0202
#define TT_PERSONAL_MIC		0x0203
#define TT_OMNI_DIR_MIC		0x0204  // omni-directional microphone
#define TT_MIC_ARRAY		0x0205
#define TT_PROC_MIC_ARRAY	0x0206  // processing microphone array
#define TT_OUTPUT_UNDEFINED 0x0300
#define TT_SPEAKER			0x0301
#define TT_HEADPHONES		0x0302
#define TT_HEAD_DISPLAY		0x0303  // head mounted display audio
#define TT_DESKTOP_SPEAKER	0x0304
#define TT_ROOM_SPEAKER		0x0305
#define TT_COMM_SPEAKER		0x0306	// communication speaker
#define TT_LFE_SPEAKER		0x0307	// low frequency effects speaker
#define TT_BIDIR_UNDEFINED	0x0400  // bidirectional undefined
#define TT_HANDSET			0x0401
#define TT_HEADSET			0x0402
#define TT_SPEAKERPHONE		0x0403
#define TT_ECHO_SUPPRESSING	0x0404 // echo suppressing speakerphone
#define TT_ECHO_CANCELING	0x0405 // echo canceling speakerphone
#define TT_TELEPHONY_UNDEF	0x0500
#define TT_PHONE_LINE		0x0501
#define TT_TELEPHONE		0x0502
#define TT_DOWN_LINE_PHONE	0x0503
#define TT_EXTERNAL_UNDEF	0x0600
#define TT_ANALOG_CONNECTOR	0x0601
#define TT_DIGITAL_AUDIO_I	0x0602 // digital audio interface
#define TT_LINE_CONNECTOR	0x0603
#define TT_LEGACY_AUDIO_CON	0x0604
#define TT_SPDIF_INTERFACE	0x0605
#define TT_1394_DA_STREAM	0x0606
#define TT_1394_DV_STR_ST	0x0607 // 1394 DV stream soundtrack
#define TT_EMBEDDED_UNDEF	0x0700
#define TT_LEVEL_CALIB_NS	0x0701 // level calibration noise source
#define TT_EQ_NOISE			0x0702 // equalization noise
#define TT_CD_PLAYER		0x0703
#define TT_DAT				0x0704
#define TT_DCC				0x0705
#define TT_MINIDISK			0x0706
#define TT_ANALOG_TAPE		0x0707
#define TT_PHONOGRAPH		0x0708
#define TT_VCR_AUDIO		0x0709
#define TT_VIDEODISC_AUDIO	0x070A
#define TT_DVD_AUDIO		0x070B
#define TT_TVTUNER_AUDIO	0x070C
#define TT_SAT_REC_AUDIO	0x070D // satellite receiver audio
#define TT_CABLE_TUN_AUDIO	0x070E // cable tuner audio
#define TT_DSS_AUDIO		0x070F
#define TT_RADIO_RECEIVER   0x0710
#define TT_RADIO_TRANSM		0x0711
#define TT_MULTITRACK_REC	0x0712
#define TT_SYNTHESIZER		0x0713




#endif /* CODESANDSELECTORS_H_ */
