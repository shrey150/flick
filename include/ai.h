#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Generates edit suggestions given a set of videos.
 * 
 * @return a JSON object listing potential edits.
 * @param files the video files to add 
 */
json gen_edits(char* files[], int length);
