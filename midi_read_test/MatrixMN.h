/////////////////////////////////////////////////////////////////////////////
// Authored by Jeong-Mo Hong for CSE4060 course at Dongguk University CSE  //
// jeongmo.hong@gmail.com                                                  //
// Do whatever you want license.                                           //
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VectorND.h"
#include <fstream>

template<class T>
class MatrixMN
{
public:
    int num_rows_;  // m_
    int num_cols_;  // n_
    T   *values_;

    MatrixMN()
        : values_(nullptr), num_rows_(0), num_cols_(0)
    {}

	void initialize(const int& _m, const int& _n, const bool init = true);

	void multiply(const VectorND<T>& vector, VectorND<T>& result) const;
    void multiplyTransposed(const VectorND<T>& vector, VectorND<T>& result) const;

	int get1DIndex(const int& row, const int& column) const;
	T&  getValue(const int& row, const int& column) const;

	void cout();

	void read(std::ifstream& is)
	{
		int num_rows_input, num_cols_input;

		is.read((char*)&num_rows_input, sizeof(num_rows_input));
		is.read((char*)&num_cols_input, sizeof(num_cols_input));

		initialize(num_rows_input, num_cols_input, false);

		for (int i = 0; i < num_rows_input * num_cols_input; i++)
			is.read((char*)&values_[i], sizeof(T));
	}

	void write(std::ofstream& os) const
	{
		os.write((char*)&num_rows_, sizeof(num_rows_));
		os.write((char*)&num_cols_, sizeof(num_cols_));

		for (int i = 0; i < num_rows_ * num_cols_; i++)
			os.write((char*)&values_[i], sizeof(T));
	}
};
