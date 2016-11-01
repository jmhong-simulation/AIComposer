#pragma once

#include <string>
#include <iostream>
#include "MidiFile.h"
#include "SoundPlayer.h"
#include <list>
#include <algorithm>
#include <windows.h>

class MidiFileReader
{
public:
	SoundPlayer *sound_player_ = nullptr;

	struct Event
	{
		int tick_;
		int note_;
		int duration_; // TODO: need to convert from tick to ms
	};
		
	int tick_start_;
	int tick_end_;

	std::list<Event> event_list_;

	void playEventList() const;

	void read(const std::string& filename, const bool& test_play = false);	// TODO: add output parameters	
};
