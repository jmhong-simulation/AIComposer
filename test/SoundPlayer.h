#pragma once

/*
	TODO
	- Mix_Music should play MP3 and more, but not working in some reason.
	- move DEFINEs to variable (still don't understand details)
	- This example is mostly from http://lazyfoo.net/SDL_tutorials/lesson11/
*/

//Using SDL, SDL_image, SDL_mixer, standard IO, and strings
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <map>

class SoundPlayer
{
public:
	std::map<std::string, Mix_Chunk*> sound_library_;

	~SoundPlayer()
	{
		for (auto itr : sound_library_)
			delete itr.second;

		Mix_CloseAudio();
	}

	void init()
	{
		if (SDL_Init(SDL_INIT_AUDIO) < 0) exit(1);

		// Setup audio mode
		// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_11.html
		//Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024);
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);


		// can't read mp3 in some reason
		//Mix_Music *mus = Mix_LoadMUS("D:\\github-repository\\SDLMixerTest\\test\\happy.mp3");
		//Mix_PlayMusic(mus, 0); //Music loop=1

		//if (!mus)
		//{
		//	std::cout << "music error" << std::endl;
		//}

		/* scratch from example codes
		
		Mix_Music *mus, *mus2;  // Background Music 
		Mix_Chunk *wav, *wav2;  // For Sounds

		//Not working in some reason
		mus = Mix_LoadMUS("D:/github-repository/SDLMixerTest/test/happy.mp3");

		if (!mus)
		{
			std::cout << "music error" << std::endl;
		}

		mus2 = Mix_LoadMUS("D:\\github-repository\\FMODTest\\test\\sound_files\\31.mp3");
		wav = Mix_LoadWAV("D:\\github-repository\\FMODTest\\test\\sound_files\\29.wav");
		wav2 = Mix_LoadWAV("D:\\github-repository\\FMODTest\\test\\sound_files\\31.wav");



		//Mix_LoadMUS https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_55.html
		//Mix_PlayMusic https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_57.html

		Mix_PlayMusic(mus, 0); //Music loop=1
		Mix_PlayMusic(mus2, 2); //Music loop=tw
		Mix_PlayChannel(-1, wav, 0);
		Mix_PlayChannel(-1, wav2, 0);
*/
	}

	void addWav(const std::string& key_name, std::string& file_name)
	{
		Mix_Chunk *new_wave = Mix_LoadWAV(file_name.c_str());

		if (!new_wave)
		{
			std::cout << "Failed to load " << file_name << std::endl;
			exit(1);
		}

		sound_library_[key_name] = new_wave;
	}

	void playSound(const std::string& key_name)
	{
		if (sound_library_.count(key_name) == 0)
		{
			std::cout << "Cannot find " << key_name << std::endl;
			//exit(1);
		}

		Mix_PlayChannel(-1, sound_library_.find(key_name)->second, 0);
	}
};

//void errorv(char *str, va_list ap)
//{
//	vfprintf(stderr, str, ap);
//	fprintf(stderr, ": %s.\n", SDL_GetError());
//}
//
//void cleanExit(char *str, ...)
//{
//	va_list ap;
//	va_start(ap, str);
//	errorv(str, ap);
//	va_end(ap);
//	Mix_CloseAudio();
//	SDL_Quit();
//	exit(1);
//}