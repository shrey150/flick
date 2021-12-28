#pragma once

class FFmpeg {
public:
    static void* create(const char* outputFileName);

    static void* probe(const char* fileName);

    static void addVideoStream(void* handle, const char* videoFileName);
    static void addAudioStream(void* handle, const char* audioFileName);

    static void addVideoFilter(void* handle, const char* filterString);
    static void addAudioFilter(void* handle, const char* filterString);

    static void generate(void* handle);

    static bool isError(void* handle);
    static const char* getError(void* handle);

    static void close(void* handle);
};
