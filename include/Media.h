#pragma once

#include <string>
#include <unordered_map>

#include "ffmpeg.h"
#include "util.h"

#define ENCODE 0
#define DECODE 1

class Media {
public:
    /**
     * Constructs a Media object and initializes member objects.
     */
    Media(std::string filename);

    /**
     * "Opens" a video file by initializing the necessary FFmpeg containers.
     */
    void open_stream(AVFormatContext* &ctx, AVStream* &stream);

    void close_stream(AVFormatContext* &ctx, AVStream* &stream);

    inline void open_decoder(AVCodecContext* &dec_ctx, AVStream* &stream) {
        init_codec(dec_ctx, stream, DECODE);
    }

    inline void open_encoder(AVCodecContext* &enc_ctx, AVStream* &stream) {
        init_codec(enc_ctx, stream, ENCODE);
    }
    
    void add_filter(std::string param, std::string val);
    void remove_filter(std::string param);

    inline bool has_filters() { return params.size() > 0; }
    void filter_frame(AVFrame* &frame);

private:
    void init_graph();
    void reset_filter_graph();

    void init_codec(AVCodecContext* &dec_ctx, AVStream* &stream, uint8_t type);

    const std::string filename;
    const MediaInfo info;

    std::unordered_map<std::string,std::string> params;

    AVFilterGraph* filter_graph;
    AVFilterContext* buffersrc_ctx;
    AVFilterContext* buffersink_ctx;

    AVFilterInOut* inputs;
    AVFilterInOut* outputs;
};