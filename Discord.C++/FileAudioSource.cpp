#include "FileAudioSource.h"

using namespace std;

/**	@param[in]	filename		the name of the file to use as AudioSource
	@return		FileAudioSource	playable AudioSource
*/
DiscordCPP::FileAudioSource::FileAudioSource(const std::string& filename) {
	_file.open(filename, ios_base::binary);
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
