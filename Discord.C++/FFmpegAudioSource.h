#pragma once
#include "AudioSource.h"

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace boost::process {
	class child;
	template<class CharT, class Traits>
	class basic_ipstream;
	typedef basic_ipstream<char, std::char_traits<char>> ipstream;
}

namespace DiscordCPP {
	class FFmpegAudioSource : public AudioSource {
	private:
		std::shared_ptr <boost::process::child> _process;
		std::shared_ptr <boost::process::ipstream> _pipe;
	public:
		/**	Creates an AudioSource using FFmpeg. This requires ffmpeg to be in your PATH.
			@param[in]	input				The input for ffmpeg.
			@param[in]	before_options		[Optional] Options to place before input.
			@param[in]	options				[Optional] Options to place after input.
			@return		FFmpegAudioSource	playable AudioSource
		*/
		DLL_EXPORT FFmpegAudioSource(const std::string& input, const std::string& before_options = "", const std::string& options = "");
		DLL_EXPORT ~FFmpegAudioSource();

		DLL_EXPORT bool read(char* pcm_data, const int length);
	};
}
