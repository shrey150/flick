#include "Media.h"
#include "api.h"

Media::Media(std::string filename):
	filename(filename),
	info(FFmpeg::probe(filename.c_str()))
{}

void Media::open_stream(AVFormatContext* &fmt_ctx, AVStream* &stream) {
	// initialize container context
	if (avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL) < 0) {
		throw "Unable to open video file";
	}
	
	// initialize stream info inside container context
	if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
		throw "Unable to find stream information";
	}

	// get the index of the video stream in container
	auto video_stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
	if (video_stream_idx < 0) {
		throw "Unable to find video stream";
	}

	// initialize object to the appropriate video stream
	stream = fmt_ctx->streams[video_stream_idx];
}

void Media::close_stream(AVFormatContext* &fmt_ctx, AVStream* &stream) {
	avformat_close_input(&fmt_ctx);
	stream = nullptr;
}

void Media::init_codec(AVCodecContext* &codec_ctx, AVStream* &stream, uint8_t type) {
	// copy video stream's codec parameters to codec context
	avcodec_parameters_to_context(codec_ctx, stream->codecpar);

	// fetch the encoder/decoder matching the video stream's codec
	const AVCodec* codec = type == DECODE
		? avcodec_find_decoder(stream->codecpar->codec_id)
		: avcodec_find_encoder(stream->codecpar->codec_id);

	// open encoder/decoder and initialize its context
	if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
		throw "Unable to open video " + std::string(type == DECODE ? "decoder" : "encoder");
	}
}

void Media::init_graph() {
	// clear existing filter graph
	reset_filter_graph();

	// don't generate graph if there are no filters
	if (params.size() == 0) {
		filter_graph = nullptr;
		return;
	}

	// initialize input (src) and output (sink) filters
	const AVFilter* buffersrc = avfilter_get_by_name("buffersrc");
	const AVFilter* buffersink = avfilter_get_by_name("buffersink");

	// initialize filter args
	char args[512];
	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		info.width, info.height, info.format,
		info.time_base.num, info.time_base.den,
		info.aspect_ratio_raw.num, info.aspect_ratio_raw.den
	);

	// initialize filter graph
	filter_graph = avfilter_graph_alloc();

	// initialize buffer source (input filter)
	if (avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
		args, NULL, filter_graph) < 0) {
		throw "Unable to create buffer source (input filter)";
	}

	// configure buffer sink output parameters
	// TODO deprecated, use buffer_ref
	AVBufferSinkParams* buffersink_params = av_buffersink_params_alloc();
	enum AVPixelFormat pix_fmts[] = { (AVPixelFormat) info.format };
	buffersink_params->pixel_fmts = pix_fmts;

	// initialize buffer sink (output filter)
	if (avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
		NULL, buffersink_params, filter_graph) < 0) {
		throw "Unable to create buffer sink (output filter)";
	}

	// NOTE: the following seems counterintuitive, but only the input filter
	// will have an output, and only the output filter will have an input
	//
	// initialize linked list of inputs and outputs (only one of each)
	AVFilterInOut* inputs = avfilter_inout_alloc();
	AVFilterInOut* outputs = avfilter_inout_alloc();

	outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

	// parse filter graph description
	// TODO eliminate dependance on filter description, generate IO ourselves
	if (avfilter_graph_parse_ptr(filter_graph, "scale=78:24", &inputs, &outputs, NULL) < 0) {
		reset_filter_graph();
		throw "Failed to parse filter graph description";
	}

	// configure and validate filter graph
	if (avfilter_graph_config(filter_graph, NULL) < 0) {
		reset_filter_graph();
		throw "Failed to configure filter graph";
	}
}

// TODO generate IO ourselves instead of clearing filter graph and restarting
void Media::add_filter(std::string param, std::string val) {
	params[param] = val;
	init_graph();
}

void Media::remove_filter(std::string param) {
	params.erase(param);
	init_graph();
}

void Media::reset_filter_graph() {
	avfilter_graph_free(&filter_graph);
	avfilter_inout_free(&inputs);
	avfilter_inout_free(&outputs);
}

void Media::filter_frame(AVFrame* &frame) {
	// push frame into filter graph
	if (av_buffersrc_add_frame(buffersrc_ctx, frame) < 0) {
		av_frame_unref(frame);
		av_frame_free(&frame);
		throw "Error pushing frame into filter graph";
	}

	// pull frame out of filter graph
	if (av_buffersink_get_frame(buffersink_ctx, frame) < 0) {
		av_frame_unref(frame);
		av_frame_free(&frame);
		throw "Error pulling frame out of filter graph";
	}
}