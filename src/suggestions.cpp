#include <string>
#include <iostream>
#include "suggestions.h"
#include "api.h"
#include "util.h"

json gen_edits(char* files[], int num_files) {

    json info = json::array();

    list_files(files, num_files);

    // loop through each video
    for (int i = 0; i < num_files; ++i) {
        // add each info object to JSON array
        info.push_back(FFmpeg::probe(files[i]));
    }

    // [TEMP] test high level FFmpeg API
	// void* handle = FFmpeg::create("out.mp4");
	// FFmpeg::addVideoStream(handle, "../tests/big_buck_bunny.mp4");
	// FFmpeg::addVideoFilter(handle, "transpose=1");
    // FFmpeg::addAudioStream(handle, "../tests/big_buck_bunny.mp4");
	// FFmpeg::generate(handle);
	// FFmpeg::close(handle);

    std::cout << "Video info: " << info << std::endl; 

    // TODO change to suggested edits JSON object
    return json::object({});
}
