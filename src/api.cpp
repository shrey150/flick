#include <iostream>
#include "api.h"
#include <nlohmann/json.hpp>

#if defined(_WIN32)
    #define popen _popen
    #define pclose _pclose
#endif

using json = nlohmann::json;

json FFmpeg::probe(std::string file) {
    // system  ("ffprobe -v error \
    //             -select_streams v:0 \
    //             -show_entries stream=width,height,sample_aspect_ratio,display_aspect_ratio \
    //             -of json=c=1 " + file
    //         );
    return {};
}
