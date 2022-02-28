#include "api.h"

#include <string>
#include <iostream>
#include "ffmpeg.h"

#include "util.h"

using namespace std;

MediaInfo FFmpeg::probe(const char* videoFileName)
{
	AVFormatContext* formatCtx = avformat_alloc_context();
	if (avformat_open_input(&formatCtx, videoFileName, NULL, NULL) != 0) {
		throw "Unable to open video file";
	}
	
	avformat_find_stream_info(formatCtx, NULL);

	auto videoStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (videoStreamIndex < 0) {
		throw "Unable to find video stream";
	}

	auto codecCtx = formatCtx->streams[videoStreamIndex]->codecpar;

	MediaInfo info = {
		videoFileName,
		(double) formatCtx->duration / AV_TIME_BASE,
		codecCtx->width,
		codecCtx->height,
		to_aspect_ratio(codecCtx->width, codecCtx->height)
	};

	avformat_close_input(&formatCtx);
	avformat_free_context(formatCtx);

	return info;
}