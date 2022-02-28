#include <string>
#include <iostream>
#include <vector>
#include "suggestions.h"
#include "api.h"
#include "util.h"

std::vector<EditInfo> gen_edits(char* files[], int num_files) {

    std::vector<MediaInfo> info;

    list_files(files, num_files);

    // loop through each video
    for (int i = 0; i < num_files; ++i) {

        // add each info object to vector
        try {
            info.push_back(FFmpeg::probe(files[i]));
        }
        catch (...) {
            std::cerr << "Error reading info for " << files[i] << std::endl;
            exit(1);
        }
    }

    // [TEMP] test high level FFmpeg API
	// void* handle = FFmpeg::create("out.mp4");
	// FFmpeg::addVideoStream(handle, "../tests/big_buck_bunny.mp4");
	// FFmpeg::addVideoFilter(handle, "transpose=1");
    // FFmpeg::addAudioStream(handle, "../tests/big_buck_bunny.mp4");
	// FFmpeg::generate(handle);
	// FFmpeg::close(handle);

    std::cout << "Video info: " << std::endl;
    list_media_info(info);

    // TODO change to suggested edits object
    return {};
}
