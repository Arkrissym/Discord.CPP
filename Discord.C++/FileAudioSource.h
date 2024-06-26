#pragma once
#include <fstream>

#include "AudioSource.h"

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
    /** @param[in]  filename        the name of the file to use as AudioSource
                    @return     FileAudioSource playable AudioSource
    */
    DLL_EXPORT explicit FileAudioSource(const std::string& filename);
    DLL_EXPORT ~FileAudioSource() override;

    DLL_EXPORT bool read(char* pcm_data, const int length) override;
};

}  // namespace DiscordCPP
