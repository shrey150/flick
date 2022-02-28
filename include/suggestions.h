#pragma once

#include "util.h"

/**
 * Generates edit suggestions given a set of videos.
 * 
 * @return a list of EditInfo listing potential edits.
 * @param files the video files to add 
 */
std::vector<EditInfo> gen_edits(char* files[], int length);
