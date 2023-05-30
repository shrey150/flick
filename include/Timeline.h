#pragma once

#include <vector>
#include <string>

#include "Media.h"

class Timeline {
public:
    void add(Media m);
    void export_video(std::string output_filename);

private:
    std::vector<Media> clips;
};