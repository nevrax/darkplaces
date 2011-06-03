#ifndef _MSC_VER
#include <stdint.h>
#endif
#include <sys/types.h>

#include "quakedef.h"
#include "client.h"
#include "cap_lavc.h"

#ifdef DP_LINK_TO_LAVC

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>

#define qavcodec_register_all avcodec_register_all
#define qav_register_all av_register_all
#define qav_write_trailer av_write_trailer
#define qavcodec_close avcodec_close
#define qav_free av_free
#define qavcodec_get_frame_defaults avcodec_get_frame_defaults
#define qavcodec_encode_video avcodec_encode_video
#define qavcodec_encode_audio avcodec_encode_audio
#define qav_init_packet av_init_packet
#define qav_interleaved_write_frame av_interleaved_write_frame
#define qavformat_alloc_context avformat_alloc_context
#define qav_guess_format av_guess_format
#define qav_new_stream av_new_stream
#define qavcodec_find_encoder_by_name avcodec_find_encoder_by_name
#define qavcodec_open avcodec_open
#define qav_get_bits_per_sample av_get_bits_per_sample
#define qav_alloc_put_byte av_alloc_put_byte
#define qav_write_header av_write_header
#define qav_set_string3 av_set_string3
#define qav_get_string av_get_string
#define qav_get_token av_get_token
#define qav_set_parameters av_set_parameters

typedef  int64_t qint64_t;
typedef uint64_t quint64_t;
typedef  int32_t qint32_t;
typedef uint32_t quint32_t;
typedef  int16_t qint16_t;
typedef uint16_t quint16_t;
typedef  int8_t  qint8_t;
typedef uint8_t  quint8_t;

qboolean SCR_CaptureVideo_Lavc_OpenLibrary(void)
{
	return 1;
}

void SCR_CaptureVideo_Lavc_CloseLibrary(void)
{
}

qboolean SCR_CaptureVideo_Lavc_Available(void)
{
	return 1;
}

#else

#include <errno.h>
#if EDOM > 0
#define AVERROR(e) (-(e))
#define AVUNERROR(e) (-(e))
#else
#define AVERROR(e) (e)
#define AVUNERROR(e) (e)
#endif

#ifdef WIN32
// MSVC does not know *int*_t
typedef   signed __int64 qint64_t;
typedef unsigned __int64 quint64_t;
typedef   signed __int32 qint32_t;
typedef unsigned __int32 quint32_t;
typedef   signed __int16 qint16_t;
typedef unsigned __int16 quint16_t;
typedef   signed __int8  qint8_t;
typedef unsigned __int8  quint8_t;
#else
// sane assumptions, but not always true...
typedef long long          qint64_t;
typedef unsigned long long quint64_t;
typedef int                qint32_t;
typedef unsigned int       quint32_t;
typedef short              qint16_t;
typedef unsigned short     quint16_t;
typedef signed char        qint8_t;
typedef unsigned char      quint8_t;
#endif

#define AV_NOPTS_VALUE (qint64_t)0x8000000000000000LLU
#define AV_TIME_BASE 1000000

#define FF_MIN_BUFFER_SIZE 16384
#define AVFMT_GLOBALHEADER 0x0040
#define CODEC_FLAG_GLOBAL_HEADER 0x00400000

#define LIBAVCODEC_VERSION_MAJOR 52
#define LIBAVCODEC_VERSION_MAJOR_STRING "52"
#define LIBAVFORMAT_VERSION_MAJOR 52
#define LIBAVFORMAT_VERSION_MAJOR_STRING "52"
#define LIBAVUTIL_VERSION_MAJOR 50
#define LIBAVUTIL_VERSION_MAJOR_STRING "50"

#define FF_API_OLD_METADATA            (LIBAVFORMAT_VERSION_MAJOR < 53)
#define FF_API_LAVF_UNUSED             (LIBAVFORMAT_VERSION_MAJOR < 53)
#define FF_API_MAX_STREAMS             (LIBAVFORMAT_VERSION_MAJOR < 53)
#define FF_API_PARAMETERS_CODEC_ID     (LIBAVFORMAT_VERSION_MAJOR < 53)

#ifdef FF_API_MAX_STREAMS
#define MAX_STREAMS 20
#endif

#define attribute_deprecated

#define AV_PKT_FLAG_KEY   0x0001

#define CODEC_FLAG_QSCALE 0x0002  ///< Use fixed qscale.
#define AVFMT_NOFILE        0x0001

enum AVColorPrimaries { AVCOL_PRI_UNSPECIFIED = 2 };
enum CodecID { CODEC_ID_NONE = 0 };
enum AVDiscard { AVDISCARD_DEFAULT = 0 };
enum AVStreamParseType { AVSTREAM_PARSE_NONE = 0 };
enum PixelFormat { PIX_FMT_YUV420P = 0 };
enum AVSampleFormat { AV_SAMPLE_FMT_S16 = 1 };
enum AVMediaType { AVMEDIA_TYPE_VIDEO = 0, AVMEDIA_TYPE_AUDIO = 1 };

typedef struct AVPanScan AVPanScan;
typedef struct AVClass AVClass;
typedef struct ByteIOContext ByteIOContext;
typedef struct AVProgram AVProgram;
typedef struct AVChapter AVChapter;
typedef struct AVMetadata AVMetadata;
typedef struct AVIndexEntry AVIndexEntry;
typedef struct RcOverride RcOverride;
typedef struct AVOption AVOption;
typedef struct AVMetadataConv AVMetadataConv;
typedef struct AVCodecContext AVCodecContext;

typedef struct AVRational {
	int num;
	int den;
} AVRational;

typedef struct AVFrac {
	qint64_t val, num, den;
} AVFrac;

typedef struct AVFormatParameters {
	AVRational time_base;
	int sample_rate;
	int channels;
	int width;
	int height;
	enum PixelFormat pix_fmt;
	int channel;
	const char *standard;
	unsigned int mpeg2ts_raw:1;
	unsigned int mpeg2ts_compute_pcr:1;
	unsigned int initial_pause:1;
	unsigned int prealloced_context:1;
#if FF_API_PARAMETERS_CODEC_ID
	attribute_deprecated enum CodecID video_codec_id;
	attribute_deprecated enum CodecID audio_codec_id;
#endif
} AVFormatParameters;

typedef struct AVPacket {
	qint64_t pts;
	qint64_t dts;
	quint8_t *data;
	int size;
	int stream_index;
	int flags;
	int duration;
	void (*destruct)(struct AVPacket *);
	void *priv;
	qint64_t pos;
	qint64_t convergence_duration;
} AVPacket;

typedef struct AVCodec {
	const char *name;
	enum AVMediaType type;
	enum CodecID id;
	int priv_data_size;
	int (*init)(AVCodecContext *);          
	int (*encode)(AVCodecContext *, uint8_t *buf, int buf_size, void *data);
	int (*close)(AVCodecContext *);         
	int (*decode)(AVCodecContext *, void *outdata, int *outdata_size, AVPacket *avpkt);
	int capabilities;
	struct AVCodec *next;
	void (*flush)(AVCodecContext *);
	const AVRational *supported_framerates;
	const enum PixelFormat *pix_fmts;
	const char *long_name;
	const int *supported_samplerates;
	const enum AVSampleFormat *sample_fmts;
	const int64_t *channel_layouts;
	uint8_t max_lowres;
	AVClass *priv_class;
} AVCodec;

typedef struct AVProbeData {
	const char *filename;
	unsigned char *buf;
	int buf_size;
} AVProbeData;

typedef struct AVFrame {
	quint8_t *data[4];
	int linesize[4];
	quint8_t *base[4];
	int key_frame;
	int pict_type;
	qint64_t pts;
	int coded_picture_number;
	int display_picture_number;
	int quality;
	int age;
	int reference;
	qint8_t *qscale_table;
	int qstride;
	quint8_t *mbskip_table;
	qint16_t (*motion_val[2])[2];
	quint32_t *mb_type;
	quint8_t motion_subsample_log2;
	void *opaque;
	quint64_t error[4];
	int type;
	int repeat_pict;
	int qscale_type;
	int interlaced_frame;
	int top_field_first;
	AVPanScan *pan_scan;
	int palette_has_changed;
	int buffer_hints;
	short *dct_coeff;
	qint8_t *ref_index[2];
	qint64_t reordered_opaque;
	void *hwaccel_picture_private;
	qint64_t pkt_pts;
	qint64_t pkt_dts;
	struct AVCodecContext *owner;
	void *thread_opaque;
} AVFrame;

typedef struct AVCodecContext {
	const AVClass *av_class;
	int bit_rate;
	int bit_rate_tolerance;
	int flags;
	int sub_id;
	int me_method;
	quint8_t *extradata;
	int extradata_size;
	AVRational time_base;
	int width, height;
	int gop_size;
	enum PixelFormat pix_fmt;
	int rate_emu;
	void (*draw_horiz_band)(struct AVCodecContext *s, const AVFrame *src, int offset[4], int y, int type, int height);
	int sample_rate;
	int channels;
	enum AVSampleFormat sample_fmt;
	int frame_size;
	int frame_number;
#if LIBAVCODEC_VERSION_MAJOR < 53
	int real_pict_num;
#endif
	int delay;
	float qcompress;
	float qblur;
	int qmin;
	int qmax;
	int max_qdiff;
	int max_b_frames;
	float b_quant_factor;
	int rc_strategy;
	int b_frame_strategy;
	int hurry_up;
	struct AVCodec *codec;
	void *priv_data;
	int rtp_payload_size;
	void (*rtp_callback)(struct AVCodecContext *avctx, void *data, int size, int mb_nb);
	int mv_bits;
	int header_bits;
	int i_tex_bits;
	int p_tex_bits;
	int i_count;
	int p_count;
	int skip_count;
	int misc_bits;
	int frame_bits;
	void *opaque;
	char codec_name[32];
	enum AVMediaType codec_type;
	enum CodecID codec_id;
	unsigned int codec_tag;
	int workaround_bugs;
	int luma_elim_threshold;
	int chroma_elim_threshold;
	int strict_std_compliance;
	float b_quant_offset;
	int error_recognition;
	int (*get_buffer)(struct AVCodecContext *c, AVFrame *pic);
	void (*release_buffer)(struct AVCodecContext *c, AVFrame *pic);
	int has_b_frames;
	int block_align;
	int parse_only;
	int mpeg_quant;
	char *stats_out;
	char *stats_in;
	float rc_qsquish;
	float rc_qmod_amp;
	int rc_qmod_freq;
	RcOverride *rc_override;
	int rc_override_count;
	const char *rc_eq;
	int rc_max_rate;
	int rc_min_rate;
	int rc_buffer_size;
	float rc_buffer_aggressivity;
	float i_quant_factor;
	float i_quant_offset;
	float rc_initial_cplx;
	int dct_algo;
	float lumi_masking;
	float temporal_cplx_masking;
	float spatial_cplx_masking;
	float p_masking;
	float dark_masking;
	int idct_algo;
	int slice_count;
	int *slice_offset;
	int error_concealment;
	unsigned dsp_mask;
	int bits_per_coded_sample;
	int prediction_method;
	AVRational sample_aspect_ratio;
	AVFrame *coded_frame;
	int debug;
	int debug_mv;
	uint64_t error[4];
	int mb_qmin;
	int mb_qmax;
	int me_cmp;
	int me_sub_cmp;
	int mb_cmp;
	int ildct_cmp;
	int dia_size;
	int last_predictor_count;
	int pre_me;
	int me_pre_cmp;
	int pre_dia_size;
	int me_subpel_quality;
	enum PixelFormat (*get_format)(struct AVCodecContext *s, const enum PixelFormat * fmt);
	int dtg_active_format;
	int me_range;
	int intra_quant_bias;
	int inter_quant_bias;
	int color_table_id;
	int internal_buffer_count;
	void *internal_buffer;
	int global_quality;
	// remaining fields stripped
} AVCodecContext;

typedef struct AVStream {
	int index;
	int id;
	AVCodecContext *codec;
	AVRational r_frame_rate;
	void *priv_data;
	qint64_t first_dts;
	struct AVFrac pts;
	AVRational time_base;
	int pts_wrap_bits;
	int stream_copy;
	enum AVDiscard discard;
	float quality;
	qint64_t start_time;
	qint64_t duration;
#if FF_API_OLD_METADATA
	char language[4];
#endif
	enum AVStreamParseType need_parsing;
	struct AVCodecParserContext *parser;
	qint64_t cur_dts;
	int last_IP_duration;
	qint64_t last_IP_pts;
	AVIndexEntry *index_entries;
	int nb_index_entries;
	unsigned int index_entries_allocated_size;
	qint64_t nb_frames;
#if FF_API_LAVF_UNUSED
	qint64_t unused[4+1];
#endif
#if FF_API_OLD_METADATA
	char *filename;
#endif
	int disposition;
	AVProbeData probe_data;
	qint64_t pts_buffer[16 +1];
	AVRational sample_aspect_ratio;
	AVMetadata *metadata;
	const quint8_t *cur_ptr;
	int cur_len;
	AVPacket cur_pkt;
	qint64_t reference_dts;
	int probe_packets;
	struct AVPacketList *last_in_packet_buffer;
	AVRational avg_frame_rate;
	int codec_info_nb_frames;
	struct {
		qint64_t last_dts;
		qint64_t duration_gcd;
		int duration_count;
		double duration_error[(60*12+5)];
		qint64_t codec_info_duration;
	} *info;
} AVStream;

typedef struct AVFormatContext {
	const AVClass *av_class;
	struct AVInputFormat *iformat;
	struct AVOutputFormat *oformat;
	void *priv_data;
	ByteIOContext *pb;
	unsigned int nb_streams;
#if FF_API_MAX_STREAMS
	AVStream *streams[MAX_STREAMS];
#else
	AVStream **streams;
#endif
	char filename[1024];
	int64_t timestamp;
#if FF_API_OLD_METADATA
	char title[512];
	char author[512];
	char copyright[512];
	char comment[512];
	char album[512];
	int year;
	int track;
	char genre[32];
#endif
	int ctx_flags;
	struct AVPacketList *packet_buffer;
	int64_t start_time;
	int64_t duration;
	int64_t file_size;
	int bit_rate;
	AVStream *cur_st;
#if FF_API_LAVF_UNUSED
	const uint8_t *cur_ptr_deprecated;
	int cur_len_deprecated;
	AVPacket cur_pkt_deprecated;
#endif
	int64_t data_offset;
	int index_built;
	int mux_rate;
	unsigned int packet_size;
	int preload;
	int max_delay;
	// remaining fields stripped
} AVFormatContext;

typedef struct AVOutputFormat {
	const char *name;
	const char *long_name;
	const char *mime_type;
	const char *extensions;
	int priv_data_size;
	enum CodecID audio_codec;
	enum CodecID video_codec;
	int (*write_header)(struct AVFormatContext *);
	int (*write_packet)(struct AVFormatContext *, AVPacket *pkt);
	int (*write_trailer)(struct AVFormatContext *);
	int flags;
	int (*set_parameters)(struct AVFormatContext *, AVFormatParameters *);
	int (*interleave_packet)(struct AVFormatContext *, AVPacket *out,
			AVPacket *in, int flush);
	/**
	 * List of supported codec_id-codec_tag pairs, ordered by "better
	 * choice first". The arrays are all terminated by CODEC_ID_NONE.
	 */
	const struct AVCodecTag * const *codec_tag;
	enum CodecID subtitle_codec;
#if FF_API_OLD_METADATA
	const AVMetadataConv *metadata_conv;
#endif
	const AVClass *priv_class;
} AVOutputFormat;

void (*qavcodec_register_all) (void);
void (*qav_register_all) (void);
int (*qav_write_trailer) (AVFormatContext *s);
int (*qavcodec_close) (AVCodecContext *avctx);
void (*qav_free) (void *ptr);
void (*qavcodec_get_frame_defaults) (AVFrame *pic);
int (*qavcodec_encode_video) (AVCodecContext *avctx, quint8_t *buf, int buf_size, const AVFrame *pict);
int (*qavcodec_encode_audio) (AVCodecContext *avctx, quint8_t *buf, int buf_size, const short *samples);
void (*qav_init_packet) (AVPacket *pkt);
int (*qav_interleaved_write_frame) (AVFormatContext *s, AVPacket *pkt);
AVFormatContext * (*qavformat_alloc_context) (void);
AVOutputFormat * (*qav_guess_format) (const char *short_name, const char *filename, const char *mime_type);
AVStream * (*qav_new_stream) (AVFormatContext *s, int id);
AVCodec * (*qavcodec_find_encoder_by_name) (const char *name);
int (*qavcodec_open) (AVCodecContext *avctx, AVCodec *codec);
int (*qav_get_bits_per_sample) (enum CodecID codec_id);
ByteIOContext * (*qav_alloc_put_byte) (unsigned char *buffer, int buffer_size, int write_flag, void *opaque, int (*read_packet)(void *opaque, quint8_t *buf, int buf_size), int (*write_packet)(void *opaque, quint8_t *buf, int buf_size), qint64_t (*seek) (void *opaque, qint64_t offset, int whence));
int (*qav_write_header) (AVFormatContext *s);
int64_t (*qav_rescale_q)(int64_t a, AVRational bq, AVRational cq);
int (*qav_set_string3)(void *obj, const char *name, const char *val, int alloc, const AVOption **o_out);
const char * (*qav_get_string)(void *obj, const char *name, const AVOption **o_out, char *buf, int buf_len);
char * (*qav_get_token)(const char **buf, const char *term);
int (*qav_set_parameters)(AVFormatContext *s, AVFormatParameters *ap);
int (*qav_find_nearest_q_idx)(AVRational q, const AVRational* q_list);
int (*qavcodec_get_context_defaults3)(AVCodecContext *s, AVCodec *codec);

static dllhandle_t libavcodec_dll = NULL;
static dllfunction_t libavcodec_funcs[] =
{
	{"avcodec_close",			(void **) &qavcodec_close},
	{"avcodec_encode_audio",		(void **) &qavcodec_encode_audio},
	{"avcodec_encode_video",		(void **) &qavcodec_encode_video},
	{"avcodec_find_encoder_by_name",	(void **) &qavcodec_find_encoder_by_name},
	{"avcodec_get_frame_defaults",		(void **) &qavcodec_get_frame_defaults},
	{"avcodec_open",			(void **) &qavcodec_open},
	{"avcodec_register_all",		(void **) &qavcodec_register_all},
	{"av_get_bits_per_sample",		(void **) &qav_get_bits_per_sample},
	{"av_init_packet",			(void **) &qav_init_packet},
	{"av_get_context_defaults3",		(void **) &qavcodec_get_context_defaults3},
	{NULL, NULL}
};

static dllhandle_t libavformat_dll = NULL;
static dllfunction_t libavformat_funcs[] =
{
	{"av_alloc_put_byte",			(void **) &qav_alloc_put_byte},
	{"avformat_alloc_context",		(void **) &qavformat_alloc_context},
	{"av_guess_format",			(void **) &qav_guess_format},
	{"av_interleaved_write_frame",		(void **) &qav_interleaved_write_frame},
	{"av_new_stream",			(void **) &qav_new_stream},
	{"av_register_all",			(void **) &qav_register_all},
	{"av_write_header",			(void **) &qav_write_header},
	{"av_write_trailer",			(void **) &qav_write_trailer},
	{"av_set_parameters",			(void **) &qav_set_parameters},
	{NULL, NULL}
};

static dllhandle_t libavutil_dll = NULL;
static dllfunction_t libavutil_funcs[] =
{
	{"av_free",				(void **) &qav_free},
	{"av_rescale_q",			(void **) &qav_rescale_q},
	{"av_set_string3",			(void **) &qav_set_string3},
	{"av_get_string",			(void **) &qav_get_string},
	{"av_get_token",			(void **) &qav_get_token},
	{"av_find_nearest_q_idx",		(void **) &qav_find_nearest_q_idx},
	{NULL, NULL}
};

qboolean SCR_CaptureVideo_Lavc_OpenLibrary(void)
{
	const char* libavcodec_dllnames [] =
	{
#if defined(WIN32)
		"avcodec-" LIBAVCODEC_VERSION_MAJOR_STRING ".dll",
#else
		"libavcodec.so." LIBAVCODEC_VERSION_MAJOR_STRING,
#endif
		NULL
	};

	const char* libavformat_dllnames [] =
	{
#if defined(WIN32)
		"avformat-" LIBAVFORMAT_VERSION_MAJOR_STRING ".dll",
#else
		"libavformat.so." LIBAVFORMAT_VERSION_MAJOR_STRING,
#endif
		NULL
	};

	const char* libavutil_dllnames [] =
	{
#if defined(WIN32)
		"avutil-" LIBAVUTIL_VERSION_MAJOR_STRING ".dll",
#else
		"libavutil.so." LIBAVUTIL_VERSION_MAJOR_STRING,
#endif
		NULL
	};

	if (!libavcodec_dll)
		Sys_LoadLibrary (libavcodec_dllnames, &libavcodec_dll, libavcodec_funcs);
	if (!libavformat_dll)
		Sys_LoadLibrary (libavformat_dllnames, &libavformat_dll, libavformat_funcs);
	if (!libavutil_dll)
		Sys_LoadLibrary (libavutil_dllnames, &libavutil_dll, libavutil_funcs);

	return libavcodec_dll && libavformat_dll && libavutil_dll;
}

void SCR_CaptureVideo_Lavc_CloseLibrary(void)
{
	Sys_UnloadLibrary(&libavutil_dll);
	Sys_UnloadLibrary(&libavformat_dll);
	Sys_UnloadLibrary(&libavcodec_dll);
}

qboolean SCR_CaptureVideo_Lavc_Available(void)
{
	return libavcodec_dll && libavformat_dll && libavutil_dll;
}

#endif

#define QSCALE_NONE -99999

#if DEFAULT_VP8
static cvar_t cl_capturevideo_lavc_format = {CVAR_SAVE, "cl_capturevideo_lavc_format", "mkv", "video format to use"};
static cvar_t cl_capturevideo_lavc_formatoptions = {CVAR_SAVE, "cl_capturevideo_lavc_formatoptions", "", "space separated key=value pairs for video format flags"};
static cvar_t cl_capturevideo_lavc_vcodec = {CVAR_SAVE, "cl_capturevideo_lavc_vcodec", "libvpx", "video codec to use"};
static cvar_t cl_capturevideo_lavc_voptions = {CVAR_SAVE, "cl_capturevideo_lavc_voptions", "", "space separated key=value pairs for video encoder flags"};
static cvar_t cl_capturevideo_lavc_acodec = {CVAR_SAVE, "cl_capturevideo_lavc_acodec", "vorbis", "audio codec to use"};
static cvar_t cl_capturevideo_lavc_aoptions = {CVAR_SAVE, "cl_capturevideo_lavc_aoptions", "", "space separated key=value pairs for video encoder flags"};
#elif DEFAULT_X264
static cvar_t cl_capturevideo_lavc_format = {CVAR_SAVE, "cl_capturevideo_lavc_format", "mp4", "video format to use"};
static cvar_t cl_capturevideo_lavc_formatoptions = {CVAR_SAVE, "cl_capturevideo_lavc_formatoptions", "", "space separated key=value pairs for video format flags"};
static cvar_t cl_capturevideo_lavc_vcodec = {CVAR_SAVE, "cl_capturevideo_lavc_vcodec", "libx264", "video codec to use"};
static cvar_t cl_capturevideo_lavc_voptions = {CVAR_SAVE, "cl_capturevideo_lavc_voptions",
	/* sane */     "crf=23 threads=4 "
		/* faster */   "coder=1 flags=+loop cmp=+chroma partitions=+parti8x8+parti4x4+partp8x8+partb8x8 me_method=hex subq=4 me_range=16 g=250 keyint_min=25 sc_threshold=40 i_qfactor=0.71 b_strategy=1 qcomp=0.6 qmin=10 qmax=51 qdiff=4 bf=3 refs=2 directpred=1 trellis=1 flags2=+bpyramid-mixed_refs+wpred+dct8x8+fastpskip wpredp=1 rc_lookahead=20 "
		/* baseline */ "coder=0 bf=0 flags2=-wpred-dct8x8 wpredp=0",
	"space separated key=value pairs for video encoder flags"};
static cvar_t cl_capturevideo_lavc_acodec = {CVAR_SAVE, "cl_capturevideo_lavc_acodec", "aac", "audio codec to use"};
static cvar_t cl_capturevideo_lavc_aoptions = {CVAR_SAVE, "cl_capturevideo_lavc_aoptions", "", "space separated key=value pairs for video encoder flags"};
#else
static cvar_t cl_capturevideo_lavc_format = {CVAR_SAVE, "cl_capturevideo_lavc_format", "mkv", "video format to use"};
static cvar_t cl_capturevideo_lavc_formatoptions = {CVAR_SAVE, "cl_capturevideo_lavc_formatoptions", "", "space separated key=value pairs for video format flags"};
static cvar_t cl_capturevideo_lavc_vcodec = {CVAR_SAVE, "cl_capturevideo_lavc_vcodec", "ffvhuff", "video codec to use"};
static cvar_t cl_capturevideo_lavc_voptions = {CVAR_SAVE, "cl_capturevideo_lavc_voptions", "", "space separated key=value pairs for video encoder flags"};
static cvar_t cl_capturevideo_lavc_acodec = {CVAR_SAVE, "cl_capturevideo_lavc_acodec", "flac", "audio codec to use"};
static cvar_t cl_capturevideo_lavc_aoptions = {CVAR_SAVE, "cl_capturevideo_lavc_aoptions", "", "space separated key=value pairs for video encoder flags"};
#endif

static int set_avoptions(void *ctx, const void *privclass, void *privctx, const char *str, const char *key_val_sep, const char *pairs_sep, int dry_run)
{
	int good = 0;
	int errorcode = 0;

	if (!privclass)
		privctx = NULL;

	while (*str)
	{
		char *key = qav_get_token(&str, key_val_sep);
		char *val;
		int ret;

		if (*key && strspn(str, key_val_sep)) {
			str++;
			val = qav_get_token(&str, pairs_sep);
		} else {
			if (!dry_run)
				Con_Printf("Missing key or no key/value separator found after key '%s'\n", key);
			qav_free(key);
			if (!errorcode)
				errorcode = AVERROR(EINVAL);
			if(*str)
				++str;
			continue;
		}

		if (!dry_run)
			Con_DPrintf("Setting value '%s' for key '%s'\n", val, key);

		ret = AVERROR(ENOENT);
		if (dry_run) {
			char buf[256];
			const AVOption *opt;
			const char *p = NULL;
			if (privctx)
				p = qav_get_string(privctx, key, &opt, buf, sizeof(buf));
			if (p == NULL)
				p = qav_get_string(ctx, key, &opt, buf, sizeof(buf));
			if (p)
				ret = 0;
		} else {
			if (privctx)
				ret = qav_set_string3(privctx, key, val, 1, NULL);
			if (ret == AVERROR(ENOENT))
				ret = qav_set_string3(ctx, key, val, 1, NULL);
			if (ret == AVERROR(ENOENT))
				Con_Printf("Key '%s' not found.\n", key);
		}

		qav_free(key);
		qav_free(val);

		if (ret < 0) {
			if (!errorcode)
				errorcode = ret;
		} else
			++good;

		if(*str)
			++str;
	}
	return errorcode ? errorcode : good;
}

void SCR_CaptureVideo_Lavc_Init(void)
{
	if(SCR_CaptureVideo_Lavc_OpenLibrary())
	{
		qavcodec_register_all();
		qav_register_all();
		Cvar_RegisterVariable(&cl_capturevideo_lavc_format);
		Cvar_RegisterVariable(&cl_capturevideo_lavc_formatoptions);
		Cvar_RegisterVariable(&cl_capturevideo_lavc_vcodec);
		Cvar_RegisterVariable(&cl_capturevideo_lavc_voptions);
		Cvar_RegisterVariable(&cl_capturevideo_lavc_acodec);
		Cvar_RegisterVariable(&cl_capturevideo_lavc_aoptions);
	}
}

void SCR_CaptureVideo_Lavc_Shutdown(void)
{
	SCR_CaptureVideo_Lavc_CloseLibrary();
}

typedef struct capturevideostate_lavc_formatspecific_s
{
	AVFormatContext *avf;
	int64_t apts;
	int64_t vpts;
	unsigned char *yuv;
	unsigned char *buffer;
	size_t bufsize;
	short *aframe;
	int aframesize;
	int aframepos;
	qboolean pcmhack;
	quint8_t bytebuffer[32768];
	int64_t asavepts;
}
capturevideostate_lavc_formatspecific_t;
#define LOAD_FORMATSPECIFIC_LAVC() capturevideostate_lavc_formatspecific_t *format = (capturevideostate_lavc_formatspecific_t *) cls.capturevideo.formatspecific

static void SCR_CaptureVideo_Lavc_ConvertFrame_BGRA_to_YUV(AVFrame *frame)
{
	LOAD_FORMATSPECIFIC_LAVC();
	int x, y;
	int blockr, blockg, blockb;
	unsigned char *b = cls.capturevideo.outbuffer;
	int w = cls.capturevideo.width;
	int uw = (w+1)/2;
	int h = cls.capturevideo.height;
	int uh = (h+1)/2;
	int inpitch = w*4;

	unsigned char *yuvy = format->yuv;
	unsigned char *yuvu = yuvy + w*h;
	unsigned char *yuvv = yuvu + uw*uh;

	frame->data[0] = yuvy;
	frame->linesize[0] = w;
	frame->data[1] = yuvu;
	frame->linesize[1] = uw;
	frame->data[2] = yuvv;
	frame->linesize[2] = uw;

	for(y = 0; y < h; ++y)
	{
		for(b = cls.capturevideo.outbuffer + (h-1-y)*w*4, x = 0; x < w; ++x)
		{
			blockr = b[2];
			blockg = b[1];
			blockb = b[0];
			yuvy[x + w * y] =
				cls.capturevideo.yuvnormalizetable[0][cls.capturevideo.rgbtoyuvscaletable[0][0][blockr] + cls.capturevideo.rgbtoyuvscaletable[0][1][blockg] + cls.capturevideo.rgbtoyuvscaletable[0][2][blockb]];
			b += 4;
		}

		if((y & 1) == 0)
		{
			for(b = cls.capturevideo.outbuffer + (h-2-y)*w*4, x = 0; x < (w+1)/2; ++x)
			{
				blockr = (b[2] + b[6] + b[inpitch+2] + b[inpitch+6]) >> 2;
				blockg = (b[1] + b[5] + b[inpitch+1] + b[inpitch+5]) >> 2;
				blockb = (b[0] + b[4] + b[inpitch+0] + b[inpitch+4]) >> 2;
				yuvu[x + uw * (y/2)] =
					cls.capturevideo.yuvnormalizetable[1][cls.capturevideo.rgbtoyuvscaletable[1][0][blockr] + cls.capturevideo.rgbtoyuvscaletable[1][1][blockg] + cls.capturevideo.rgbtoyuvscaletable[1][2][blockb] + 128];
				yuvv[x + uw * (y/2)] =
					cls.capturevideo.yuvnormalizetable[2][cls.capturevideo.rgbtoyuvscaletable[2][0][blockr] + cls.capturevideo.rgbtoyuvscaletable[2][1][blockg] + cls.capturevideo.rgbtoyuvscaletable[2][2][blockb] + 128];
				b += 8;
			}
		}
	}
}


static void SCR_CaptureVideo_Lavc_VideoFrames(int num)
{
	LOAD_FORMATSPECIFIC_LAVC();

	// data is in cls.capturevideo.outbuffer as BGRA and has size width*height
	AVCodecContext *avc = format->avf->streams[0]->codec;
	AVFrame frame;
	int size;

	do
	{
		qavcodec_get_frame_defaults(&frame);
		if(num > 0)
		{
			SCR_CaptureVideo_Lavc_ConvertFrame_BGRA_to_YUV(&frame);
			frame.pts = format->vpts;
			size = qavcodec_encode_video(avc, format->buffer, format->bufsize, &frame);
		}
		else
		{
			size = qavcodec_encode_video(avc, format->buffer, format->bufsize, NULL);
		}

		if(size < 0)
			Con_Printf("error encoding\n");
		if(size > 0)
		{
			AVPacket packet;
			qav_init_packet(&packet);
			packet.stream_index = 0;
			packet.data = format->buffer;
			packet.size = size;
			if (avc->coded_frame->key_frame)
				packet.flags |= AV_PKT_FLAG_KEY;
			if (avc->coded_frame->pts != AV_NOPTS_VALUE)
				packet.pts = qav_rescale_q(avc->coded_frame->pts, avc->time_base, format->avf->streams[0]->time_base);
			if(qav_interleaved_write_frame(format->avf, &packet) < 0)
				Con_Printf("error writing\n");
		}
	}
	while(num < 0 && size > 0);

	if(num > 0)
		format->vpts += num;
}

// FIXME find the correct mappings for DP to ffmpeg
typedef int channelmapping_t[8];
static channelmapping_t mapping[8] =
{
	{ 0, -1, -1, -1, -1, -1, -1, -1 }, // mono
	{ 0, 1, -1, -1, -1, -1, -1, -1 }, // stereo
	{ 0, 1, 2, -1, -1, -1, -1, -1 }, // L C R
	{ 0, 1, 2, 3, -1, -1, -1, -1 }, // surround40
	{ 0, 2, 3, 4, 1, -1, -1, -1 }, // FL FC FR RL RR
	{ 0, 2, 3, 4, 1, 5, -1, -1 }, // surround51
	{ 0, 2, 3, 4, 1, 5, 6, -1 }, // (not defined by vorbis spec)
	{ 0, 2, 3, 4, 1, 5, 6, 7 } // surround71 (not defined by vorbis spec)
};

static void SCR_CaptureVideo_Lavc_SoundFrame_Encode(void)
{
	LOAD_FORMATSPECIFIC_LAVC();
	int size;
	AVCodecContext *avc = format->avf->streams[1]->codec;

	if(format->pcmhack)
		size = qavcodec_encode_audio(avc, format->buffer, format->aframesize * format->pcmhack, format->aframe);
	else
		size = qavcodec_encode_audio(avc, format->buffer, format->bufsize, format->aframe);

	if(format->asavepts == AV_NOPTS_VALUE)
		format->asavepts = format->apts;

	if(size < 0)
		Con_Printf("error encoding\n");
	if(size > 0)
	{
		AVPacket packet;
		qav_init_packet(&packet);
		packet.stream_index = 1;
		packet.data = format->buffer;
		packet.size = size;
		if(avc->coded_frame && avc->coded_frame->pts != AV_NOPTS_VALUE)
			packet.pts = qav_rescale_q(avc->coded_frame->pts, avc->time_base, format->avf->streams[1]->time_base);
		else
			packet.pts = format->asavepts;
		format->asavepts = AV_NOPTS_VALUE;
		if(qav_interleaved_write_frame(format->avf, &packet) < 0)
			Con_Printf("error writing\n");
	}

	format->apts += avc->frame_size;
	format->aframepos = 0;
}
static void SCR_CaptureVideo_Lavc_SoundFrame_EncodeEnd(void)
{
	LOAD_FORMATSPECIFIC_LAVC();
	int size;
	AVCodecContext *avc = format->avf->streams[1]->codec;
	do
	{
		size = qavcodec_encode_audio(avc, format->buffer, format->bufsize, NULL);

		if(format->asavepts == AV_NOPTS_VALUE)
			format->asavepts = format->apts;

		if(size < 0)
			Con_Printf("error encoding\n");
		if(size > 0)
		{
			AVPacket packet;
			qav_init_packet(&packet);
			packet.stream_index = format->avf->streams[1]->index;
			packet.data = format->buffer;
			packet.size = size;
			packet.flags |= AV_PKT_FLAG_KEY;
			if(avc->coded_frame && avc->coded_frame->pts != AV_NOPTS_VALUE)
				packet.pts = qav_rescale_q(avc->coded_frame->pts, avc->time_base, format->avf->streams[1]->time_base);
			else
				packet.pts = format->asavepts;
			format->asavepts = AV_NOPTS_VALUE;
			if(qav_interleaved_write_frame(format->avf, &packet) < 0)
				Con_Printf("error writing\n");
		}
	}
	while(size > 0);
}

static void SCR_CaptureVideo_Lavc_SoundFrame(const portable_sampleframe_t *paintbuffer, size_t length)
{
	LOAD_FORMATSPECIFIC_LAVC();

	if(cls.capturevideo.soundrate)
	{
		AVCodecContext *avc = format->avf->streams[1]->codec;
		int i;
		int *map = mapping[bound(1, cls.capturevideo.soundchannels, 8) - 1];
		size_t bufpos = 0;

		if(paintbuffer)
		{
			while(bufpos < length)
			{
				// fill up buffer
				while(bufpos < length && format->aframepos < format->aframesize)
				{
					for(i = 0; i < cls.capturevideo.soundchannels; ++i)
						format->aframe[format->aframepos*cls.capturevideo.soundchannels+map[i]] = paintbuffer[bufpos].sample[i];
					++bufpos;
					++format->aframepos;
				}

				if(format->aframepos >= avc->frame_size)
				{
					SCR_CaptureVideo_Lavc_SoundFrame_Encode();
				}
				else
				{
					// if we get here, frame_size was not hit
					// this means that length has been hit!
					break;
				}
			}
		}
		else
		{
			memset(format->aframe + format->aframepos*cls.capturevideo.soundchannels, 0, sizeof(format->aframe[0]) * (format->aframesize - format->aframepos));
			SCR_CaptureVideo_Lavc_SoundFrame_Encode();
			SCR_CaptureVideo_Lavc_SoundFrame_EncodeEnd();
		}
	}
}

static void SCR_CaptureVideo_Lavc_EndVideo(void)
{
	LOAD_FORMATSPECIFIC_LAVC();

	if(format->buffer)
	{
		SCR_CaptureVideo_Lavc_VideoFrames(-1);
		SCR_CaptureVideo_Lavc_SoundFrame(NULL, 0);
		qav_write_trailer(format->avf);
		{
			unsigned int i;
			for (i = 0; i < format->avf->nb_streams; i++) {
				qavcodec_close(format->avf->streams[i]->codec);
				qav_free(format->avf->streams[i]->codec);
				qav_free(format->avf->streams[i]->info);
				qav_free(format->avf->streams[i]);
			}
			qav_free(format->avf->pb);
			qav_free(format->avf);
		}
		Mem_Free(format->buffer);
	}
	if(format->aframe)
		Mem_Free(format->aframe);
	Mem_Free(format);

	FS_Close(cls.capturevideo.videofile);
	cls.capturevideo.videofile = NULL;
}

static int lavc_write(void *f, quint8_t *buf, int bufsize)
{
	return FS_Write((qfile_t *) f, buf, bufsize);
}

static qint64_t lavc_seek(void *f, qint64_t offset, int whence)
{
	return FS_Seek((qfile_t *) f, offset, whence);
}

void SCR_CaptureVideo_Lavc_BeginVideo(void)
{
	const char *fn;
	cls.capturevideo.format = CAPTUREVIDEOFORMAT_LAVC;
	cls.capturevideo.formatextension = cl_capturevideo_lavc_format.string;
	fn = va("%s.%s", cls.capturevideo.basename, cls.capturevideo.formatextension);
	cls.capturevideo.videofile = FS_OpenRealFile(fn, "wb", false);
	cls.capturevideo.endvideo = SCR_CaptureVideo_Lavc_EndVideo;
	cls.capturevideo.videoframes = SCR_CaptureVideo_Lavc_VideoFrames;
	cls.capturevideo.soundframe = SCR_CaptureVideo_Lavc_SoundFrame;
	cls.capturevideo.formatspecific = Mem_Alloc(tempmempool, sizeof(capturevideostate_lavc_formatspecific_t));
	{
		LOAD_FORMATSPECIFIC_LAVC();
		AVCodec *encoder;
		AVFormatParameters avp;
		int num, denom;

		format->bufsize = FF_MIN_BUFFER_SIZE;

		format->avf = qavformat_alloc_context();
		format->avf->oformat = qav_guess_format(NULL, fn, NULL); // TODO: use a separate cvar for the ffmpeg format name
		if(!format->avf->oformat)
			format->avf->oformat = qav_guess_format(NULL, fn, cls.capturevideo.formatextension);
		if(!format->avf->oformat)
		{
			Con_Printf("format not found\n");
			SCR_CaptureVideo_EndVideo();
			return;
		}

		strlcpy(format->avf->filename, fn, sizeof(format->avf->filename));

		memset(&avp, 0, sizeof(avp));
		if(qav_set_parameters(format->avf, &avp) < 0)
		{
			Con_Printf("format cannot be initialized\n");
			SCR_CaptureVideo_EndVideo();
			return;
		}

		if(set_avoptions(format->avf, format->avf->oformat->priv_class, format->avf->priv_data, cl_capturevideo_lavc_formatoptions.string, "=", " \t", 0) < 0)
			Con_Printf("Failed to set some format options\n");

		format->avf->preload = 0.5 * AV_TIME_BASE;
		format->avf->max_delay = 0.7 * AV_TIME_BASE;

		// we always have video
		{
			AVStream *video_str;
			video_str = qav_new_stream(format->avf, 0);
			video_str->codec->codec_type = AVMEDIA_TYPE_VIDEO;

			encoder = qavcodec_find_encoder_by_name(cl_capturevideo_lavc_vcodec.string);
			if(!encoder)
			{
				Con_Printf("Failed to find encoder\n");
				SCR_CaptureVideo_EndVideo();
				return;
			}
			qavcodec_get_context_defaults3(video_str->codec, encoder);
			video_str->codec->codec_id = encoder->id;

			FindFraction(cls.capturevideo.framerate / cls.capturevideo.framestep, &num, &denom, cls.capturevideo.framerate / cls.capturevideo.framestep * 1001 + 2);
			{
				AVRational r;
				r.num = num;
				r.den = denom;
				if(encoder && encoder->supported_framerates)
					r = encoder->supported_framerates[qav_find_nearest_q_idx(r, encoder->supported_framerates)];
				video_str->codec->time_base.num = r.den;
				video_str->codec->time_base.den = r.num;
				video_str->time_base = video_str->codec->time_base;
				cls.capturevideo.framerate = r.num / (double) r.den;
			}

			video_str->codec->width = cls.capturevideo.width;
			video_str->codec->height = cls.capturevideo.height;
			video_str->codec->pix_fmt = PIX_FMT_YUV420P;
			FindFraction(1 / vid_pixelheight.value, &num, &denom, 1000);
			video_str->sample_aspect_ratio.num = num;
			video_str->sample_aspect_ratio.den = denom;
			video_str->codec->sample_aspect_ratio.num = num;
			video_str->codec->sample_aspect_ratio.den = denom;

			if(format->avf->oformat->flags & AVFMT_GLOBALHEADER)
				video_str->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			video_str->codec->global_quality = 0;
			if(set_avoptions(video_str->codec, encoder->priv_class, video_str->codec->priv_data, cl_capturevideo_lavc_voptions.string, "=", " \t", 0) < 0)
				Con_Printf("Failed to set video options\n");
			if (video_str->codec->global_quality != 0)
				video_str->codec->flags |= CODEC_FLAG_QSCALE;

			if(qavcodec_open(video_str->codec, encoder) < 0)
			{
				Con_Printf("Failed to open encoder\n");
				SCR_CaptureVideo_EndVideo();
				return;
			}

			if (video_str->codec->global_quality != 0)
				video_str->codec->flags |= CODEC_FLAG_QSCALE;

			format->vpts = 0;
			format->bufsize = max(format->bufsize, (size_t) (cls.capturevideo.width * cls.capturevideo.height * 6 + 200));
		}

		if(cls.capturevideo.soundrate)
		{
			AVStream *audio_str;
			audio_str = qav_new_stream(format->avf, 0);
			audio_str->codec->codec_type = AVMEDIA_TYPE_AUDIO;

			encoder = qavcodec_find_encoder_by_name(cl_capturevideo_lavc_acodec.string);
			if(!encoder)
			{
				Con_Printf("Failed to find encoder\n");
				SCR_CaptureVideo_EndVideo();
				return;
			}
			qavcodec_get_context_defaults3(audio_str->codec, encoder);
			audio_str->codec->codec_id = encoder->id;

			audio_str->codec->time_base.num = 1;
			audio_str->codec->time_base.den = cls.capturevideo.soundrate;

			audio_str->codec->sample_rate = cls.capturevideo.soundrate;
			audio_str->codec->channels = cls.capturevideo.soundchannels;
			audio_str->codec->sample_fmt = AV_SAMPLE_FMT_S16;

			audio_str->codec->global_quality = QSCALE_NONE;
			if(set_avoptions(audio_str->codec, encoder->priv_class, audio_str->codec->priv_data, cl_capturevideo_lavc_aoptions.string, "=", " \t", 0) < 0)
				Con_Printf("Failed to set audio options\n");
			if (audio_str->codec->global_quality != QSCALE_NONE)
				audio_str->codec->flags |= CODEC_FLAG_QSCALE;

			if(format->avf->oformat->flags & AVFMT_GLOBALHEADER)
				audio_str->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

			if(qavcodec_open(audio_str->codec, encoder) < 0)
			{
				Con_Printf("Failed to open encoder\n");
				SCR_CaptureVideo_EndVideo();
				return;
			}

			// is it a nasty PCM codec?
			if(audio_str->codec->frame_size <= 1)
			{
				format->pcmhack = qav_get_bits_per_sample(audio_str->codec->codec_id) / 8;
			}

			format->aframesize = audio_str->codec->frame_size;
			if(format->pcmhack)
				format->aframesize = 16384; // use 16k samples for PCM

			format->apts = 0;
			format->aframe = Z_Malloc(format->aframesize * sizeof(*format->aframe) * cls.capturevideo.soundchannels);
			format->aframepos = 0;

			if(format->pcmhack)
				format->bufsize = max(format->bufsize, format->aframesize * format->pcmhack * cls.capturevideo.soundchannels * 2 + 200);
			else
				format->bufsize = max(format->bufsize, format->aframesize * sizeof(*format->aframe) * cls.capturevideo.soundchannels * 2 + 200);
		}

		if(!(format->avf->oformat->flags & AVFMT_NOFILE))
			format->avf->pb = qav_alloc_put_byte(format->bytebuffer, sizeof(format->bytebuffer), 1, cls.capturevideo.videofile, NULL, lavc_write, lavc_seek);

		if(qav_write_header(format->avf) < 0)
		{
			Con_Printf("Failed to write header\n");
			SCR_CaptureVideo_EndVideo();
			return;
		}

		format->buffer = Z_Malloc(format->bufsize);
		format->yuv = Z_Malloc(cls.capturevideo.width * cls.capturevideo.height + ((cls.capturevideo.width + 1) / 2) * ((cls.capturevideo.height + 1) / 2) * 2);

		format->asavepts = AV_NOPTS_VALUE;
	}
}
