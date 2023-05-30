#include "api.h"

#include <string>
#include <iostream>
#include "ffmpeg.h"

#include "util.h"

using namespace std;

MediaInfo FFmpeg::probe(const char* video_filename)
{
	AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (avformat_open_input(&fmt_ctx, video_filename, NULL, NULL) != 0) {
		throw "Unable to open video file";
	}
	
	avformat_find_stream_info(fmt_ctx, NULL);

	auto video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (video_stream_idx < 0) {
		throw "Unable to find video stream";
	}

	auto codec_ctx = fmt_ctx->streams[video_stream_idx];
	auto codec_params = codec_ctx->codecpar;

	MediaInfo info = {
		video_filename,
		(double) fmt_ctx->duration / AV_TIME_BASE,
		codec_params->width,
		codec_params->height,
		to_aspect_ratio(codec_params->width, codec_params->height),
		codec_params->format,
		codec_ctx->time_base,
		codec_ctx->sample_aspect_ratio
	};

	AVDictionaryEntry *tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX);
	char* metadata;
	av_dict_get_string(fmt_ctx->metadata, &metadata, ':', ';');

	avformat_close_input(&fmt_ctx);
	avformat_free_context(fmt_ctx);

	return info;
}