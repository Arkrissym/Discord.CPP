#include "FileAudioSource.h"

DiscordCPP::FileAudioSource::FileAudioSource(const std::string& filename) {
    _file.open(filename, std::ios_base::binary);
}

DiscordCPP::FileAudioSource::~FileAudioSource() {
    _file.close();
}

bool DiscordCPP::FileAudioSource::read(char* pcm_data, const int length) {
    _file.read((char*)pcm_data, length);

    if (_file.gcount() == 0)
        return false;

    return true;
}
