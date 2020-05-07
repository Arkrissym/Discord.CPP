#pragma once

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

namespace DiscordCPP {
	class AudioSource {
	public:
		DLL_EXPORT AudioSource() {};
		DLL_EXPORT virtual ~AudioSource() {};

		/**	@param[in]	pcm_data	an array to save the raw audio data
			@param[in]	length		length of pcm_data
			@return 	bool		wether the opreation was succesfull or not
		*/
		DLL_EXPORT virtual bool read(char* pcm_data, const int length) = 0;
	};

}
