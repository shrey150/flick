#include "edit.h"
#include <string>

// TODO: refactor for Media class
void rotate(MediaInfo media, int deg) {

    AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (avformat_open_input(&fmt_ctx, media.name.c_str(), NULL, NULL) != 0) {
		throw "Unable to open video file";
	}
	avformat_find_stream_info(fmt_ctx, NULL);

    av_dict_set(&fmt_ctx->metadata, "rotate", std::to_string(deg).c_str(), AV_DICT_DONT_STRDUP_VAL);
}