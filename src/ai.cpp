#include <string>
#include <iostream>
#include "ai.h"
#include "api.h"

json gen_edits(char* files[], int length) {

    std::cout << "Files (" << length << "):" << std::endl;
    
    // list input files
    for (int i = 0; i < length; ++i) {
        std::cout << files[i] << std::endl;
    }

    std::cout << std::endl;

    // [TEMP] test high level FFmpeg API
	void* handle = FFmpeg::create("out.mp4");
	FFmpeg::addVideoStream(handle, "../tests/big_buck_bunny.mp4");
	FFmpeg::addVideoFilter(handle, "transpose=1");
    FFmpeg::addAudioStream(handle, "../tests/big_buck_bunny.mp4");
	FFmpeg::generate(handle);
	FFmpeg::close(handle);

    return {
        {"test", "value"}
    };
}
