#include "FFmpegAudioSource.h"

#include <boost/process.hpp>

#include "Exceptions.h"

DiscordCPP::FFmpegAudioSource::FFmpegAudioSource(
    const std::string& input, const std::string& before_options,
    const std::string& options) {
    std::string command = "ffmpeg " + before_options + " -i " + input + " " +
                          options +
                          " -loglevel warning -f s16le -ac 2 -ar 48000 pipe:1";

    _pipe =
        std::make_shared<boost::process::ipstream>(boost::process::ipstream());
    _process = std::make_shared<boost::process::child>(
        boost::process::child(command, boost::process::std_out > *_pipe,
                              boost::process::std_err > stderr));
}

DiscordCPP::FFmpegAudioSource::~FFmpegAudioSource() {
    if (_process->running()) {
        _process->terminate();
    }
    _process->wait();
}

bool DiscordCPP::FFmpegAudioSource::read(char* pcm_data, const int length) {
    _pipe->read((char*)pcm_data, length);

    if (_pipe->gcount() == 0) return false;

    return true;
}
