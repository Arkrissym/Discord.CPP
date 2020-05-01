#pragma once
#include "AudioSource.h"
#include <fstream>

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {
	class FileAudioSource : public AudioSource {
	private:
		std::ifstream _file;
	public:
		DLL_EXPORT FileAudioSource(const std::string& filename);
		DLL_EXPORT ~FileAudioSource();

		DLL_EXPORT bool read(char* pcm_data, const int length);
	};

}
