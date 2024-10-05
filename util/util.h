#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <assert.h>
#include <time.h>

namespace util {

	std::string read_file(const std::string &filePath);
	char* ReadBinaryFile(const char* pFileName, int& size);
	void WriteBinaryFile(const char* pFilename, const void* pData, int size);
	long long GetCurrentTimeMillis();
	float GenerateRandomFloat(float min, float max);

	void file_error(const char* pFileName, int line, const char* pFileError);
	void fatal_error(const char* format, ...);

}



