#pragma once

#include <vector>
#include "VectorND.h"

template<class T>
class NotesTickTable
{
public:
	std::vector<VectorND<T> > table_;

	NotesTickTable(const int& num_notes, const int& num_ticks)
	{
		table_.resize(num_ticks);
		
		for (int i = 0; i < table_.size(); i ++)
		{
			table_[i].initialize(num_notes, true);
		}
	}
};

