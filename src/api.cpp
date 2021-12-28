#include "api.h"

#include <string>
#include <iostream>
#include <stdio.h>
#if defined(__linux__) || defined(__APPLE__)
#include <experimental/filesystem>
#else
#include <io.h>
#include <filesystem>
#endif
#include <fstream>
#include <sstream>

#include "ffmpegcpp.h"
#include "ffmpeg.h"

#include "util.h"

using namespace std;
using namespace ffmpegcpp;

template<class CodecType, class FrameSinkType>
struct StreamContext
{
	CodecType* codec = nullptr;

	string sourceFileName;
	Demuxer* demuxer = nullptr;

	bool hasFilter = false;
	string filterString;
	Filter* filter = nullptr;

	FrameSinkType* encoder = nullptr;

	void CleanUp()
	{
		if (encoder != nullptr) delete encoder;
		if (filter != nullptr) delete filter;
		if (codec != nullptr) delete codec;
		// the demuxer can be shared amongst different contexts, so we don't delete them here
	}
};

struct Context
{
	string outputFileName;
	Muxer* muxer = nullptr;

	StreamContext<VideoCodec, VideoFrameSink> videoContext;
	StreamContext<AudioCodec, AudioFrameSink> audioContext;

	vector<Demuxer*> uniqueDemuxers;

	bool errored = false;
	char* error;
};

void SetError(Context* ctx, string error)
{
	ctx->errored = true;
	string err = string(error);
	ctx->error = new char[err.length() + 1];
	strcpy(ctx->error, err.c_str());
}

void CleanUp(Context* ctx)
{
	if (ctx->muxer != nullptr) delete ctx->muxer;

	ctx->videoContext.CleanUp();
	ctx->audioContext.CleanUp();

	if (ctx->error != nullptr) delete ctx->error;

	for (unsigned int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
	{
		delete ctx->uniqueDemuxers[i];
	}

	delete ctx;
}

Demuxer* GetExistingDemuxer(Context* ctx, const char* fileName)
{
	for (unsigned int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
	{
		if (string(ctx->uniqueDemuxers[i]->GetFileName()) == string(fileName)) return ctx->uniqueDemuxers[i];
	}
	return nullptr;
}

void* FFmpeg::create(const char* outputFileName)
{
	Context* ctx = new Context();
	try
	{
		// create the output muxer but don't add any streams yet
		ctx->outputFileName = string(outputFileName);
		ctx->muxer = new Muxer(ctx->outputFileName.c_str());
		return ctx;
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to create output file " + ctx->outputFileName + ": " + string(e.what())));
		return nullptr;
	}
}

json FFmpeg::probe(const char* videoFileName)
{
	AVFormatContext* formatCtx = avformat_alloc_context();
	avformat_open_input(&formatCtx, videoFileName, NULL, NULL);
	avformat_find_stream_info(formatCtx, NULL);

	auto videoStreamIndex = av_find_best_stream(formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	if (videoStreamIndex < 0){
		throw new FFmpegException("Unable to find video stream");
	}

	auto codecCtx = formatCtx->streams[videoStreamIndex]->codecpar;

	json video_info = {
		{"name", videoFileName},
		{"duration", (double) formatCtx->duration / AV_TIME_BASE},
		{"width", codecCtx->width},
		{"height", codecCtx->height},
		{"aspect_ratio", to_aspect_ratio(codecCtx->width, codecCtx->height)}
	};

	avformat_close_input(&formatCtx);
	avformat_free_context(formatCtx);

	return video_info;
}

void FFmpeg::addVideoStream(void* handle, const char* videoFileName)
{
	Context* ctx = (Context*)handle;
	try
	{
		// create the demuxer or re-use the previous one
		ctx->videoContext.sourceFileName = videoFileName;
		ctx->videoContext.demuxer = GetExistingDemuxer(ctx, videoFileName);
		if (ctx->videoContext.demuxer == nullptr)
		{
			ctx->videoContext.demuxer = new Demuxer(ctx->videoContext.sourceFileName.c_str());
			ctx->uniqueDemuxers.push_back(ctx->videoContext.demuxer);
		}

		// create the encoder
		ctx->videoContext.codec = new VideoCodec(ctx->muxer->GetDefaultVideoFormat()->id);
		ctx->videoContext.encoder = new VideoEncoder(ctx->videoContext.codec, ctx->muxer);
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to add video stream " + string(videoFileName) + ": " + string(e.what())));
	}
}

void FFmpeg::addAudioStream(void* handle, const char* audioFileName)
{
	Context* ctx = (Context*)handle;
	try
	{
		// create the demuxer or re-use the previous one
		ctx->audioContext.sourceFileName = audioFileName;
		ctx->audioContext.demuxer = GetExistingDemuxer(ctx, audioFileName);
		if (ctx->audioContext.demuxer == nullptr)
		{
			ctx->audioContext.demuxer = new Demuxer(ctx->audioContext.sourceFileName.c_str());
			ctx->uniqueDemuxers.push_back(ctx->audioContext.demuxer);
		}

		// create the encoder
		ctx->audioContext.codec = new AudioCodec(ctx->muxer->GetDefaultAudioFormat()->id);
		ctx->audioContext.encoder = new AudioEncoder(ctx->audioContext.codec, ctx->muxer);
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to add audio stream " + string(audioFileName) + ": " + string(e.what())));
	}
}

void FFmpeg::addVideoFilter(void* handle, const char* filterString)
{
	Context* ctx = (Context*)handle;
	ctx->videoContext.filterString = filterString;
	ctx->videoContext.hasFilter = true;
}

void FFmpeg::addAudioFilter(void* handle, const char* filterString)
{
	Context* ctx = (Context*)handle;
	ctx->audioContext.filterString = filterString;
	ctx->audioContext.hasFilter = true;
}

void FFmpeg::generate(void* handle)
{
	Context* ctx = (Context*)handle;
	try
	{
		// create a filter if necessary
		FrameSink* videoFrameSink = ctx->videoContext.encoder;
		FrameSink* audioFrameSink = ctx->audioContext.encoder;
		if (ctx->videoContext.hasFilter)
		{
			ctx->videoContext.filter = new Filter(ctx->videoContext.filterString.c_str(), videoFrameSink);
			videoFrameSink = ctx->videoContext.filter;
		}
		if (ctx->audioContext.hasFilter)
		{
			ctx->audioContext.filter = new Filter(ctx->audioContext.filterString.c_str(), audioFrameSink);
			audioFrameSink = ctx->audioContext.filter;
		}

		// connect the input and output streams
		if (ctx->videoContext.demuxer != nullptr)
		{
			ctx->videoContext.demuxer->DecodeBestVideoStream(videoFrameSink);
		}
		if (ctx->audioContext.demuxer != nullptr)
		{
			ctx->audioContext.demuxer->DecodeBestAudioStream(audioFrameSink);
		}

		// now go over all the streams and process all frames
		for (unsigned int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
		{
			ctx->uniqueDemuxers[i]->PreparePipeline();
		}

		// finally, we can start writing data to our pipelines. Open the floodgates
		// to start reading frames from the input, decoding them, optionally filtering them,
		// encoding them and writing them to the final container.
		// This can be interweaved if you want to.
		for (unsigned int i = 0; i < ctx->uniqueDemuxers.size(); ++i)
		{
			Demuxer* demuxer = ctx->uniqueDemuxers[i];
			while (!demuxer->IsDone()) demuxer->Step();
		}

		// close the muxer and save the file to disk
		ctx->muxer->Close();
	}
	catch (FFmpegException e)
	{
		SetError(ctx, string("Failed to generate output file: " + string(e.what())));
	}
}

bool FFmpeg::isError(void* handle)
{
	return ((Context*)handle)->errored;
}

const char* FFmpeg::getError(void* handle)
{
	Context* ctx = (Context*)handle;
	return ctx->error;
}

void FFmpeg::close(void* handle)
{
	CleanUp((Context*)handle);
}