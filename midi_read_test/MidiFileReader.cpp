#include "MidiFileReader.h"

void MidiFileReader::playEventList() const
{
	if (sound_player_ == nullptr)
	{
		std::cout << "Sound player is not set" << std::endl;
		exit(-1);
	}

	const int TRQ_scaler = 13;	//TODO: determine from MIDI file.

	int prev_tick = event_list_.begin()->tick_;

	auto itr = event_list_.begin();
	int current_tick = 0;
	const int tick_dt = 10;

	while (itr != event_list_.end())
	{
		int num_notes_to_play = 0; // hormony

		while ((*itr).tick_ == current_tick)
		{
			std::cout << dec << (*itr).tick_ << " " << num_notes_to_play << " "
				<< (*itr).note_ << " " << (*itr).duration_ << std::endl;

			sound_player_->playSound(std::to_string((*itr).note_));

			itr++;
			num_notes_to_play++;
		}

		current_tick += tick_dt;

		//Sleep(TRQ_scaler * (*itr).duration_); // current implementation doesn't make use of duration
		Sleep(TRQ_scaler);
	}

	//for (auto itr : event_list_)
	//{
	//	std::cout << std::dec << itr.tick_ << " "<< itr.note_<< std::endl;

	//	Sleep((itr.tick_ - prev_tick) * TRQ_scaler);	
	//	//Sleep(1000);

	//	prev_tick = itr.tick_;

	//	sound_player_->playSound(std::to_string(itr.note_));
	//}
}

//TODO: don't test play here.
//      make a play data array and play it after reading.
void MidiFileReader::read(const std::string& filename, const bool& test_play)	// TODO: add output parameters
{
	MidiFile midifile;
	midifile.read(filename.c_str());

	const int num_tracks = midifile.getTrackCount();
	std::cout << "Ticks Per Quater Note = " << midifile.getTicksPerQuarterNote() << std::endl;

	if (num_tracks > 1)
	{
		std::cout << "Number of Tracks = " << num_tracks << std::endl;
	}

	const bool print = false;

	for (int track = 0; track < num_tracks; track++)
		//for (int track = 1; track < num_tracks; track++) //TODO: track 0 and 1 are duplicated ?
	{
		std::cout << "Start Reading Track " << track << std::endl;

		for (int event = 0; event < midifile[track].size(); event++)
		{
			if (print) std::cout << std::dec << midifile[track][event].tick;
			if (print) std::cout << '\t' << std::hex;

			if (midifile[track][event].isNoteOn()) // 144 = 0x90
			{
				const int note = midifile[track][event].getKeyNumber() - 20;

				/*if (!(note >= 1 && note <= 88))
				{
				std::cout << "Invalid note " << dec << note << ' ';
				continue;
				}*/

				if (print) std::cout << dec << note << "-start ";

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

			else if (midifile[track][event].isNoteOff())
			{
				const int note = midifile[track][event].getKeyNumber() - 20;

				// find latest play event of this note
				for (auto itr = event_list_.rbegin(); itr != event_list_.rend(); ++itr)
				{
					if ((*itr).note_ == note)
					{
						(*itr).duration_ = midifile[track][event].tick - (*itr).tick_;

						if (print) std::cout << dec << (*itr).duration_ << "-end ";
						break;
					}
				}
			}

			if (print) std::cout << std::endl;
		}

	}

	//std::sort(event_list_.begin(), event_list_.end(), [](Event a, Event b) {
	//	return b.tick_ < a.tick_;
	//});
	event_list_.sort( [](Event a, Event b) {
							return b.tick_ > a.tick_;
					});

	tick_start_ = 0;
	tick_end_ = 0;

	for (auto itr : event_list_)
	{
		tick_start_ = min(tick_start_, itr.tick_);
		tick_end_ = max(tick_end_, itr.tick_ + itr.duration_);
	}

	std::cout << "Tick range" << tick_start_ << " " << tick_end_ << std::endl;
}