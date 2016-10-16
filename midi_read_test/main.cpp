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

#include "VectorND.h"
#include "NeuralNetwork.h"

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
	my_midi_file_reader.read("../midi_files/Classic/Haendel/Harpsichord Suite n1 Hwv426 1mov.mid", false);

	// test play midi data
	my_midi_file_reader.playEventList();

	int temp;
	std::cin >> temp;

	// initialize nn
	nn_.initialize(7 * (8 + 1), 8 + 1, 1, 1);    // 8 keys + 1 length(0 to 7), input 7 histories

	nn_.layer_type_act_[0] = 2;
	nn_.layer_type_act_[1] = 2;
	nn_.layer_type_act_[2] = 2;
	//nn_.layer_type_act_[3] = 2;

	//nn_.weights_[0].setDiagonal();
	//  nn_.weights_[1].setDiagonal();

	nn_.eta_ = 1e-4;
	nn_.alpha_ = 0.5;

	FILE *my_file = fopen("my_play.txt", "r");

	int num;
	fscanf(my_file, "%d", &num);

	my_play.initialize(num, true);
	my_play_length.initialize(num, true);

	std::cout << "Read my play record (Training data)" << std::endl;

	for (int i = 0; i < num; i++)
	{
		int note;
		int length;
		fscanf(my_file, "%d %d", &note, &length);

		my_play[i] = note;
		my_play_length[i] = length;

		//myplaysoundnt(rand()%8);    // random play
		
		// uncomment if you want to listen recorded play
		sound_player.playSound(std::to_string(note));
		Sleep(length / 1.5);
	}

	fclose(my_file);

	//exit(1);
	bool free_play = false;
	std::cout << std::endl;
	std::cout << "Start training" << std::endl;

	for (int set = 0; set < 0; set++)
	{
		int count = 0;
		int count_all = 0;

		for (int tr = 0; tr < 0; tr++)
		{
			for (int i = 0; i < my_play.num_dimension_ - 7; i++)
			{
				VectorND<D> input;
				input.initialize(nn_.num_input_, true);

				int history_start = i;

				{int count = 0;
				for (int h = history_start; h < history_start + 7; h++) // 7 histories
				{
					input[count + my_play[h]] = 1.0;
					input[9] = (double)my_play_length[h] / 2000.0;

					count += 9;// 8 keys
				}}

				nn_.setInputVector(input);
				nn_.feedForward();

				VectorND<D> output;
				nn_.copyOutputVector(output, false);

				const int out_note = nn_.getIXProbOutput(0, 7);

				VectorND<D> desired;
				desired.initialize(8 + 1, true);

				desired[my_play[history_start + 7]] = 1.0;
				desired[8] = (double)my_play_length[history_start + 7] / 2000.0;

				if (out_note == my_play[history_start + 7]) count++;
				count_all++;

				if (free_play == false)
					nn_.propBackward(desired);
				else
				{
					my_play[history_start + 7] = out_note;
					my_play_length[history_start + 7] = output[8];

					sound_player.playSound(std::to_string(out_note));

					Sleep(output[8] * 2000.0);
				}
			}
			if (free_play == true)
				std::cout << "Fully updated" << std::endl;
		}

		std::cout << (double)count / (double)count_all * 100.0 << " % " << std::endl;

		if ((double)count / (double)count_all > 0.0) break; // takes more time to achieve higher rate
	}
	std::cout << std::endl;
	std::cout << "End of training" << std::endl;

	free_play = true;

	while (1)
	{

		for (int i = 0; i < 7; i++)
		{
			sound_player.playSound(std::to_string(my_play[i]));
			Sleep(my_play_length[i]);
		}

		for (int i = 0; i < my_play.num_dimension_ - 7; i++)
		{
			VectorND<D> input;
			input.initialize(nn_.num_input_, true);

			int history_start = i;

			{int count = 0;
			for (int h = history_start; h < history_start + 7; h++) // 7 histories
			{
				input[count + my_play[h]] = 1.0;
				input[9] = (double)my_play_length[h] / 2000.0;

				count += 9;// 8 keys
			}}

			nn_.setInputVector(input);
			nn_.feedForward();

			VectorND<D> output;
			nn_.copyOutputVector(output, false);

			const int out_note = nn_.getIXProbOutput(0, 7);

			VectorND<D> desired;
			desired.initialize(8 + 1, true);

			desired[my_play[history_start + 7]] = 1.0;
			desired[8] = (double)my_play_length[history_start + 7] / 2000.0;

			if (free_play == false)
				nn_.propBackward(desired);
			else
			{
				my_play[history_start + 7] = out_note;
				my_play_length[history_start + 7] = output[8];

				sound_player.playSound(std::to_string(out_note));
//				Sleep(CLAMP(output[8] * 2000.0 / 1.5, 500, 2000));
			}
		}

	}

	return 0;
}
