#include "SoundPlayer.h"

#pragma comment (lib, "winmm.lib")
#include <windows.h>
#include <mmsystem.h>
// for Sleep() function.
// TODO: use standard function

void SoundPlayer::readWavFileSequence(const std::string& filename_sequence, const int& start_ix, const int& end_ix, const bool& test_play)
{
	for (int i = start_ix; i <= end_ix; i++)
	{
		char filename[255];
		std::sprintf(filename, filename_sequence.c_str(), i);

		std::cout << "Reading " << filename << " : ";
		if (addWav(std::to_string(i), std::string(filename)) == true)
		{
			std::cout << "OK" << std::endl;

			if (test_play)
			{
				playSound(std::to_string(i));
				Sleep(500);
			}
		}
		else
		{
			std::cout << "Failed. Terminating program." << std::endl;
			exit(1);
		}
	}
}