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
            std::cerr << "Error probing info for " << files[i] << std::endl;
            exit(1);
        }
    }

    std::cout << "Video info: " << std::endl;
    list_media_info(info);
    std::cout << std::endl;

    // TODO change to suggested edits object
    return {};
}
