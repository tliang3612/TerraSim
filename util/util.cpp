#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <sstream>

#include "util.h"
#include <cstdarg>
#include <random>

namespace util {
    float GenerateRandomFloat(float min, float max){
        //using mt19937 algorithmn for randomness
        std::random_device rand;
        std::mt19937 gen(rand());

        std::uniform_real_distribution<float> dist(min, max); // create distribution in range [min, max]

        return dist(gen);
    }

    std::string read_file(const std::string& filePath) {
        std::ifstream shaderFile(filePath);
        if (!shaderFile.is_open()) {
            fatal_error("Failed to open shader file: %s", filePath);
        }

        // using sstream is faster 
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        return shaderStream.str();
    }

    void file_error(const char* pFileName, int line, const char* pFileError) {
        fprintf(stderr, "%s:%d: unable to open file `%s`\n", pFileName, line, pFileError);
        exit(EXIT_FAILURE);
    }

    void fatal_error(const char* format, ...) {
        char msg[1000];
        va_list args;
        va_start(args, format);
        vsnprintf(msg, sizeof(msg), format, args);
        va_end(args);

        fprintf(stderr, "Error at %s\n", msg);
        std::cerr << "Failed to load textures!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

