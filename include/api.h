#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class FFmpeg {
public:
    /**
     * Analyzes a video using FFprobe.
     * 
     * @param file the video file to analyze
     * @return JSON output from FFprobe. 
     */
    static json probe(std::string file);
};
