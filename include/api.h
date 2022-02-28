#pragma once

#include "util.h"

class FFmpeg {
public:
    static MediaInfo probe(const char* videoFileName);
};
