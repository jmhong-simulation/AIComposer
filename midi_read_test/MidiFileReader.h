#pragma once

#include <string>
#include <iostream>
#include "MidiFile.h"
#include "SoundPlayer.h"
#include <list>
#include <algorithm>

class MidiFileReader
{
public:
	SoundPlayer *sound_player_ = nullptr;

	struct Event
	{
		int tick_;
		int note_;
		int duration_; // ms?
	};
	
	std::list<Event> event_list_;

	void playEventList()
	{
		if (sound_player_ == nullptr)
		{
			std::cout << "Sound player is not set" << std::endl;
			exit(-1);
		}

		const int TRQ_scaler = 2;

		int prev_tick = event_list_.begin()->tick_;

		for (auto itr : event_list_)
		{
			std::cout << std::dec << itr.tick_ << " "<< itr.note_<< std::endl;

			Sleep((itr.tick_ - prev_tick) * TRQ_scaler);	
			//Sleep(1000);

			prev_tick = itr.tick_;

			sound_player_->playSound(std::to_string(itr.note_));
		}
	}

	//TODO: don't test play here.
	//      make a play data array and play it after reading.
	void read(const std::string& filename, const bool& test_play = false)	// TODO: add output parameters
	{
		MidiFile midifile;
		midifile.read(filename.c_str());

		const int num_tracks = midifile.getTrackCount();
		std::cout << "Ticks Per Quater Note = " << midifile.getTicksPerQuarterNote() << std::endl;

		if (num_tracks > 1)
		{
			std::cout << "Number of Tracks = " << num_tracks << std::endl;
		}

		for (int track = 0; track < num_tracks; track++)
		//for (int track = 1; track < num_tracks; track++) //TODO: track 0 and 1 are duplicated ?
		{
			std::cout << "Start Reading Track " << track << std::endl;

			for (int event = 0; event < midifile[track].size(); event++)
			{
				std::cout << std::dec << midifile[track][event].tick;
				std::cout << '\t' << std::hex;

				if ((int)midifile[track][event][0] == 144) // 144 = 0x90
				{
					const int note = midifile[track][event][1] - 20;

					/*if (!(note >= 1 && note <= 88))
					{
						std::cout << "Invalid note " << dec << note << ' ';
						continue;
					}*/

					std::cout << dec << note << ' ';

					if (sound_player_ != nullptr && test_play == true)
					{
						sound_player_->playSound(std::to_string(note));

						Sleep(100);
					}

					Event new_event;
					new_event.tick_ = midifile[track][event].tick;
					new_event.note_ = note;

					event_list_.push_back(new_event);
				}

				std::cout << std::endl;
			}
		}

		//std::sort(event_list_.begin(), event_list_.end(), [](Event a, Event b) {
		//	return b.tick_ < a.tick_;
		//});
		event_list_.sort([](Event a, Event b) {
				return b.tick_ > a.tick_;
			});
	}
};
