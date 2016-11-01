/*
	TODO
	#1. SDL Wave play module upgrade
	- copy from sample
	- replace play functions
	
	2. MIDI input and play
	- aquire sample MIDI data (very simple one)
	- aquire Wav data
	- combine multiple channels to one array
	- note length
	- remove test folder
	- It seems like our sound API doesn't have enough number of channels to play all sounds

	3. Train with it

	
*/

#define _CRT_SECURE_NO_WARNINGS

#include "SoundPlayer.h"
#include <iostream>
#include "NotesTickTable.h"

#include "VectorND.h"
#include "NeuralNetwork.h"

#include <Windows.h>
#include <stdio.h>
#include <conio.h>


#pragma comment (lib, "winmm.lib")
#include <windows.h>
#include <mmsystem.h>

#include "MidiFileReader.h"
#include "Options.h"
#include <iomanip>

#define CLAMP(v, min, max)		((v) > (max) ? (max) : ((v) < (min) ? (min) : (v)))

SoundPlayer sound_player;

VectorND<int> my_play, my_play_length;
NeuralNetwork nn_;

int main(int argc, char *argv[])
{
	sound_player.init();
	sound_player.readWavFileSequence("../sound_files/piano88/Piano %03d.wav", 1, 88, false);

	// parse midi file
	MidiFileReader my_midi_file_reader;
	my_midi_file_reader.sound_player_ = &sound_player;
//	my_midi_file_reader.read("../midi_files/Classic/Haendel/Harpsichord Suite n1 Hwv426 1mov.mid", false);
	//my_midi_file_reader.read("Etude_op10_n01.mid", false);
	my_midi_file_reader.read("C:\\github-repository\\AIComposer\\midi_files\\Beethoven\\mond_1_format0.mid", false);
	
	// test play midi data
	//my_midi_file_reader.playEventList();

	const int d_tick = 10;
	NotesTickTable<D> table_(88, (my_midi_file_reader.tick_end_ - my_midi_file_reader.tick_start_) / d_tick + 1);

	// fill notes tick table from play event list
	{
		const std::list<MidiFileReader::Event> &event_list = my_midi_file_reader.event_list_;

		for (auto itr : event_list)
		{
			// Use duration
			//for(int tick = itr.tick_ / d_tick; tick <= (itr.tick_ + itr.duration_)/d_tick; tick ++)
			//	table_.table_[tick](itr.note_) = 1.0;

			table_.table_[itr.tick_/d_tick](itr.note_) = 1.0;
		}
	}

	// test play notes-tick table
	const bool test_play_table = false;

	if(test_play_table)
	{
		VectorND<bool> notes_played(88);

		for (int i = 0; i < notes_played.num_dimension_; i++) notes_played(i) = false;

		for (auto itr : table_.table_)
		{
			for (int n = 0; n < itr.num_dimension_; n++)
			{
				if (itr(n) == 1.0)
				{
					std::cout << dec << n << " ";

					my_midi_file_reader.sound_player_->playSound(std::to_string(n));
				}				

				// use duration
				//if (itr(n) == 1.0)
				//{
				//	std::cout << dec<< n << " ";
				//	
				//	if (notes_played(n) == false)
				//	{
				//		my_midi_file_reader.sound_player_->playSound(std::to_string(n));

				//		notes_played(n) = true;
				//	}
				//}
				//else
				//{
				//	notes_played(n) = false;
				//}
			}

			std::cout << std::endl;

			Sleep(13);
		}

		std::cout << "Test play end" << std::endl;
	}

	//int temp;
	//std::cin >> temp;

	// initialize nn

	const int num_histories = 20;
	const int num_all_notes = 88;

	nn_.initialize(num_histories * num_all_notes, num_all_notes, 1, 1);

	nn_.layer_type_act_[0] = 2;
	nn_.layer_type_act_[1] = 2;
	nn_.layer_type_act_[2] = 2;
	//nn_.layer_type_act_[3] = 2;

	//nn_.weights_[0].setDiagonal();
	//  nn_.weights_[1].setDiagonal();

	nn_.eta_ = 1e-4;
	nn_.alpha_ = 0.5;

	//FILE *my_file = fopen("my_play.txt", "r");

	//int num;
	//fscanf(my_file, "%d", &num);

	//my_play.initialize(num, true);
	//my_play_length.initialize(num, true);

	//std::cout << "Read my play record (Training data)" << std::endl;

	//for (int i = 0; i < num; i++)
	//{
	//	int note;
	//	int length;
	//	fscanf(my_file, "%d %d", &note, &length);

	//	my_play[i] = note;
	//	my_play_length[i] = length;

	//	//myplaysoundnt(rand()%8);    // random play
	//	
	//	// uncomment if you want to listen recorded play
	//	sound_player.playSound(std::to_string(note));
	//	Sleep(length / 1.5);
	//}

	//fclose(my_file);

	//exit(1);
	std::cout << std::endl;

	std::cout << "Start training" << std::endl;

	const bool print_cost = true;

	bool play_during_training = false;

	{
		
		int count = 0;
		int count_all = 0;

		VectorND<D> input;
		input.initialize(nn_.num_input_, true);

		VectorND<D> output;
		VectorND<D> desired;
		desired.initialize(num_all_notes, true);

		for (int tr = 0; tr < 10000; tr++)
		{
			std::cout << "Training set " << tr << std::endl;

			D cost_per_tr = 0.0;

			for (int i = 0; i < table_.table_.size() / 500; i++)
			{
				if (kbhit() != 0)
				{
					char c = getch();

					play_during_training = !play_during_training;

					std::cout << "Play mode = " << play_during_training << std::endl;
				}

				int history_start = i - num_histories;

				// fill input vector
				{int count = 0;
				for (int h = history_start; h < history_start + num_histories; h++) // 7 histories
				{
					if (h < 0) // start part
					{
						for (int n = 0; n < num_all_notes; n++)
						{
							input[count] = 0.0;

							count++;
						}
					}
					else for (int n = 0; n < num_all_notes; n++)
					{
						input[count] = table_.table_[h](n);

						count++;
					}					
				}}

				nn_.setInputVector(input);
				nn_.feedForward();
				
				nn_.copyOutputVector(output, false);
				
				for (int n = 0; n < desired.num_dimension_; n++)
				{
					desired[n] = table_.table_[i][n];

					cost_per_tr += (desired[n] - output[n])*(desired[n] - output[n]);
				}

				nn_.propBackward(desired);

				if(play_during_training) // play
				{
					for (int n = 0; n < num_all_notes; n++)
					{
						if (rand() / (float)RAND_MAX < max(0.0, output(n)))
						{
							input(n + num_all_notes * (num_histories - 1)) = 1.0;

							std::cout << n<<":"<<output(n) << " ";

							if (n > 0) // TODO: we dont have 0 wav
								my_midi_file_reader.sound_player_->playSound(std::to_string(n));
						}
						else
							input(n + num_all_notes * num_histories - 1) = 0.0;
					}

					std::cout << std::endl;

					Sleep(13);
				}
			}			

			cost_per_tr /= table_.table_.size();

			if (print_cost == true) std::cout << cost_per_tr  << std::endl;
		}	
	}
	std::cout << std::endl;
	std::cout << "End of training" << std::endl;

	//free_play = true;

	// Trained NN plays
	{
		VectorND<D> input;
		input.initialize(nn_.num_input_, true);

		VectorND<D> output;

		// initial input
		// fill input vector
		const int history_start = 0;
		{int count = 0;
		for (int h = history_start; h < history_start + num_histories; h++)
		{
			if (h < 0) // start part
			{
				for (int n = 0; n < num_all_notes; n++)
				{
					input[count] = 0.0;

					count++;
				}
			}
			else for (int n = 0; n < num_all_notes; n++)
			{
				input[count] = table_.table_[h](n);

				count++;
			}
		}}

		while(true)
		{
			nn_.setInputVector(input);
			nn_.feedForward();
			nn_.copyOutputVector(output, false);

			// move one history back
			{
				for (int h = 0; h < num_histories - 1; h++)
					for(int n = 0; n < num_all_notes; n ++)
					{
						input(n + num_all_notes * h) = input(n + num_all_notes * (h + 1));
					}
			}

			// try random (all notes)
			{
				for (int n = 0; n < num_all_notes; n++)
				{
					if (rand() / (float)RAND_MAX < max(0.0, output(n)))
					{
						input(n + num_all_notes * (num_histories - 1)) = 1.0;

						std::cout << output(n) << " ";

						if(n > 0) // TODO: we dont have 0 wav
							my_midi_file_reader.sound_player_->playSound(std::to_string(n));
					}
					else
						input(n + num_all_notes * num_histories - 1) = 0.0;
				}

				std::cout << std::endl;
				
			}

			// one hot end
			//{
			//	for (int n = 0; n < num_all_notes; n++)
			//		input(n + num_all_notes * (num_histories - 1)) = 0.0;

			//	const int note = nn_.getIXProbOutput();

			//	input(note + num_all_notes * (num_histories - 1)) = 1.0;

			//	std::cout << note << " ";

			//	if(note > 0) // TODO: we dont have 0 wav
			//		my_midi_file_reader.sound_player_->playSound(std::to_string(note));
			//}

			Sleep(13);
		}

	}

	return 0;
}
