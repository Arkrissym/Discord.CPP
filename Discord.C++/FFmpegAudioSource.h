#pragma once
#include <boost/process.hpp>
#include <memory>
#include <string>

#include "AudioSource.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {
class FFmpegAudioSource : public AudioSource {
   private:
    std::shared_ptr<boost::asio::io_context> _io_context;
    std::shared_ptr<boost::asio::readable_pipe> _pipe;
    struct Process {
        boost::process::v2::process process;
    };
    std::shared_ptr<Process> _process;

   public:
    /**	Creates an AudioSource using FFmpeg. This requires ffmpeg to be in your PATH.
        @param[in]	input                   The input for ffmpeg.
        @param[in]	before_options          [Optional] Options to place before input.
        @param[in]	options                 [Optional] Options to place after input.
        @return         FFmpegAudioSource       playable AudioSource
    */
    DLL_EXPORT explicit FFmpegAudioSource(const std::string& input,
                                          const std::string& before_options = "",
                                          const std::string& options = "");
    DLL_EXPORT ~FFmpegAudioSource() override;

    DLL_EXPORT bool read(char* pcm_data, const int length) override;
};
}  // namespace DiscordCPP
