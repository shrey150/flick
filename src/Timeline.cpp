#include "Timeline.h"
#include "ffmpeg.h"

#include <string>

void Timeline::add(Media m) {
    clips.push_back(m);
}

void Timeline::export_video(std::string output_filename) {
    // create output video context (automatically sets oformat field for muxing)
    AVFormatContext* o_fmt_ctx;
    avformat_alloc_output_context2(&o_fmt_ctx, NULL, NULL, output_filename.c_str());

    // init first video file's context & stream
    AVFormatContext* i_fmt_ctx = avformat_alloc_context();
    AVStream* i_video_stream;
    clips[0].open_stream(i_fmt_ctx, i_video_stream);

    // create output video stream with same params as first input video file
    AVStream* o_video_stream = avformat_new_stream(o_fmt_ctx, 0);
    avcodec_parameters_copy(o_video_stream->codecpar, i_video_stream->codecpar);

    // open the output video file & set it to write
    avio_open(&o_fmt_ctx->pb, output_filename.c_str(), AVIO_FLAG_WRITE);

    // close the first video file's context & stream
    clips[0].close_stream(i_fmt_ctx, i_video_stream);

    // write output video metadata to file
    avformat_write_header(o_fmt_ctx, NULL);

    // presentation time stamp, decoding time stamp
    int64_t last_dts = 0, last_pts = 0;

    // loop through each media file
    for (Media m: clips) {
        // initialize video stream
        AVFormatContext* i_fmt_ctx = avformat_alloc_context();
        AVStream* i_video_stream;
        m.open_stream(i_fmt_ctx, i_video_stream);

        // presentation time stamp, decoding time stamp
        int64_t pts, dts;

        // mutable structs for fetching and decoding data
        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();

        // only initialize encoder/decoder if there are filters to apply
        AVCodecContext *enc_ctx, *dec_ctx;
        if (m.has_filters()) {
            dec_ctx = avcodec_alloc_context3(NULL);
            m.open_decoder(dec_ctx, i_video_stream);

            enc_ctx = avcodec_alloc_context3(NULL);
            m.open_encoder(enc_ctx, i_video_stream);
        }

        // read each packet (compressed frame) from container
        while (av_read_frame(i_fmt_ctx, packet) >= 0) {

            // decode & re-encode packet if filters need to be applied
            if (m.has_filters()) {

                // send packet to decoder
                avcodec_send_packet(dec_ctx, packet);
            
                // fetch uncompressed frame from decoder
                int ret = avcodec_receive_frame(dec_ctx, frame);

                // stop decoding if no more frames are present
                if (ret == AVERROR_EOF) {
                    break;
                }
                else if (ret < 0) {
                    throw "Error decoding video";
                }
                
                // pass frame through filter
                m.filter_frame(frame);

                // send packet to encoder
                avcodec_send_frame(enc_ctx, frame);

                // fetch compressed frame from encoder
                ret = avcodec_receive_packet(enc_ctx, packet);

                // stop encoding if no more frames are present
                if (ret == AVERROR_EOF) {
                    break;
                }
                else if (ret < 0) {
                    throw "Error decoding video";
                }
            }

            packet->flags |= AV_PKT_FLAG_KEY;
            pts = packet->pts;
            packet->pts += last_pts;
            dts = packet->dts;
            packet->dts += last_dts;
            packet->stream_index = 0;

            // write compressed packet to file
            av_interleaved_write_frame(o_fmt_ctx, packet);
        }

        last_dts += dts;
        last_pts += pts;

        m.close_stream(i_fmt_ctx, i_video_stream);
    }

    // write all remaining packets to output file & close
    av_write_trailer(o_fmt_ctx);
    avio_closep(&o_fmt_ctx->pb);

    // clean up all contexts & structs
    avformat_free_context(o_fmt_ctx);
}