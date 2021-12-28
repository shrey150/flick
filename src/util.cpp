#include <iostream>
#include <cmath>

#include "util.h"

void list_files(char* files[], int num_files) {
    std::cout << "Files (" << num_files << "):" << std::endl;
    
    // list input files
    for (int i = 0; i < num_files; ++i) {
        std::cout << files[i] << std::endl;
    }

    std::cout << std::endl;
}

/**
 * Rounds a double to 2 decimal places.
 * 
 * @param num the number to round
 * @return double a rounded decimal number
 */
double round_off(double num) {
    return round_off(num, 2);
}

/**
 * Rounds a double to the given number of decimal places.
 * 
 * @param num the number to round
 * @param precision the number of places to round to
 * @return double a rounded decimal number
 */
double round_off(double num, unsigned char precision) {
    double pow_10 = std::pow(10.0, precision);
    return round(num * pow_10) / pow_10;
}

/**
 * Returns the aspect ratio as a 2-decimal floating point number.
 * 
 * @param width the width of the video
 * @param height the height of the video
 * @return double the aspect ratio rounded to 2 decimal places
 */
double to_aspect_ratio(int width, int height) {
    return round_off((double) width / height);
}
