#include <string>
#include <iostream>
#include "ai.h"

json gen_edits(char* files[], int length) {

    std::cout << "Files (" << length << "):" << std::endl;
    
    // list input files
    for (int i = 0; i < length; ++i) {
        std::cout << files[i] << std::endl;
    }
    
    std::cout << std::endl;

    return {
        {"test", "value"}
    };
}
