#pragma once

#include "ffmpeg.h"
#include "util.h"

/**
 * Rotates a video by the given amount.
 * 
 * @param fmt_ctx the FormatContext representing the video
 * @param deg the amount in degrees to rotate by
 */
void rotate(MediaInfo media, int deg);