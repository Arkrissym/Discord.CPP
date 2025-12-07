#include "FFmpegAudioSource.h"

#include <boost/asio.hpp>
#include <cstring>
#include <sstream>
#include <vector>

DiscordCPP::FFmpegAudioSource::FFmpegAudioSource(const std::string& input,
                                                 const std::string& before_options,
                                                 const std::string& options) {
    using Process = DiscordCPP::FFmpegAudioSource::FFmpegAudioSource::Process;

    const auto exe = boost::process::environment::find_executable("ffmpeg");

    _io_context = std::make_shared<boost::asio::io_context>();
    _pipe = std::make_shared<boost::asio::readable_pipe>(*_io_context);
    std::vector<std::string> params;
    std::istringstream iss(before_options);
    std::string param;
    while (getline(iss, param, ' ')) {
        params.push_back(param);
    }
    for (auto param : std::vector<std::string>{"-i", input, "-loglevel", "warning", "-f", "s16le", "-ac", "2", "-ar", "48000", "pipe:1"}) {
        params.push_back(param);
    }
    iss = std::istringstream(options);
    while (getline(iss, param, ' ')) {
        params.push_back(param);
    }
    _process = std::make_shared<Process>(Process{boost::process::v2::process(*_io_context, exe,
                                                                             params,
                                                                             boost::process::v2::process_stdio{nullptr, *_pipe, stderr})});
}

DiscordCPP::FFmpegAudioSource::~FFmpegAudioSource() {
    if (_process->process.running()) {
        _process->process.terminate();
    }
    _process->process.wait();
}

bool DiscordCPP::FFmpegAudioSource::read(char* pcm_data, const int length) {
    boost::system::error_code ec;
    boost::asio::streambuf read_buffer;
    boost::asio::read(*_pipe, read_buffer, boost::asio::transfer_exactly(length), ec);

    if (ec.failed()) {
        return false;
    }

    std::memcpy(pcm_data, read_buffer.data().data(), length);

    return true;
}
