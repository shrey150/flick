#pragma once

#include <string>
#include <iostream>
#include <cmath>
#include <exception>
#include <string>
#include <vector>

#include "ffmpeg.h"

/**
 * Stores information about a media file.
 */
struct MediaInfo {
    std::string name;
    double duration;
    int width;
    int height;
    double aspect_ratio;
    int format;     // AVPixelFormat (video) or AVSampleFormat (audio)
                    // TODO assuming video for all Media objects right now
    AVRational time_base;
    AVRational aspect_ratio_raw;
};

/**
 * Stores information about a single edit applied to a file.
 * 
 * TODO Flesh out EditInfo.type when edits are finalized.
 *      Maybe template this struct so that it can take in a type of edit
 *      (or make an Edit struct/class that different edits inherit from)
 */
struct EditInfo {
    MediaInfo media;
    std::string type;
    int time_start;
    int time_end;
};

inline void list_files(char* files[], int num_files) {
    std::cout << "Files (" << num_files << "):" << std::endl;
    
    // list input files
    for (int i = 0; i < num_files; ++i) {
        std::cout << files[i] << std::endl;
    }

    std::cout << std::endl;
}

inline void list_media_info(std::vector<MediaInfo> info) {
    
    for (MediaInfo m: info) {
        std::cout << "Name:\t\t"          << m.name         << std::endl;
        std::cout << "Duration:\t"      << m.duration     << std::endl;
        std::cout << "Width:\t\t"         << m.width        << std::endl;
        std::cout << "Height:\t\t"        << m.height       << std::endl;
        std::cout << "Aspect ratio:\t"  << m.aspect_ratio << std::endl;
        std::cout << std::endl;
    }

}

/**
 * Rounds a double to the given number of decimal places.
 * 
 * @param num the number to round
 * @param precision the number of places to round to
 * @return double a rounded decimal number
 */
inline double round_off(double num, unsigned char precision) {
    double pow_10 = std::pow(10.0, precision);
    return round(num * pow_10) / pow_10;
}

/**
 * Rounds a double to 2 decimal places.
 * 
 * @param num the number to round
 * @return double a rounded decimal number
 */
inline double round_off(double num) {
    return round_off(num, 2);
}

/**
 * Returns the aspect ratio as a 2-decimal floating point number.
 * 
 * @param width the width of the video
 * @param height the height of the video
 * @return double the aspect ratio rounded to 2 decimal places
 */
inline double to_aspect_ratio(int width, int height) {
    return round_off((double) width / height);
}