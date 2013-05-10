/*
 * thinmatrix.h
 *
 * This file is designed to be header only (as in, it's all templated and you don't need to
 * link to any particular library for it)
 *
 *  Created on: May 1, 2013
 *      Author: dsakagi
 */

#ifndef THINMATRIX_H_
#define THINMATRIX_H_

#include <iostream>
#include <fstream>
#include <algorithm>

namespace ThinMatrixScope {
    template <typename T>
    struct UnaryConstantOp {
        UnaryConstantOp(const T & _val) : val(_val) { ; }
        ~UnaryConstantOp() { ; }

        const T operator()(const T & x) { return val; }
        T val;
    };

    template <typename T>
    struct UnaryScaleOp {
    	UnaryScaleOp(const T & _val) : scalar(_val) { ; }
    	~UnaryScaleOp() { ; }

    	const T operator()(const T & x) { return x * scalar; }
    	T scalar;
    };

    template <typename T>
    struct UnaryAddOp {
    	UnaryAddOp(const T & _val) : scalar(_val) { ; }
    	~UnaryAddOp() { ; }

    	const T operator()(const T & x) { return x + scalar; }
    	T scalar;
    };
    template <typename T>
    struct UnaryDivOp {
    	UnaryDivOp(const T & _val) : scalar(_val) { ; }
    	~UnaryDivOp() { ; }

    	const T operator()(const T & x) { return x / scalar; }
    	T scalar;
    };
}  // namespace ThinMatrixScope

template <typename T>
class ThinMatrix {
public:
	typedef float* iterator;
	typedef const float * const_iterator;

	size_t rows;
	size_t cols;
	T * data;

	ThinMatrix(size_t rows, size_t cols) : rows(rows), cols(cols), data(NULL) {
		data = new T[rows*cols];
	}

	ThinMatrix() : rows(0), cols(0), data(NULL) { ; }

	ThinMatrix(size_t rows, size_t cols, const T * _data) :
		rows(rows), cols(cols), data(NULL)
	{
		data = new T[rows*cols];
		std::copy(_data, _data+(rows*cols), data);
	}
	~ThinMatrix() {
		if (data)
			delete[] data;
        data = NULL;
	}

    ThinMatrix(const ThinMatrix<T> & other) :
        rows(other.rows),
        cols(other.cols),
        data(NULL)
    {
        data = new T[rows*cols];
        std::copy(other.cbegin(), other.cend(), this->data);
    }

    ThinMatrix & operator=(const ThinMatrix<T> & other) {
        if (this == &other) return *this;
        if (rows != other.rows || cols != other.cols) {
            if (data) delete[] data;
            data = new T[rows*cols];
        }
        std::copy(other.cbegin(), other.cend(), this->data);
        return *this;
    }

	size_t datasize(void) const { return rows * cols* sizeof(T); }
    size_t size() const { return rows * cols; }

	float *begin(void)  { return data; }
	const float *cbegin(void) const { return (const float*) data; }
	float *end(void)   { return data + rows*cols; }
	const float *cend(void) const {return (const float *) data + rows*cols; }

    /* Get an iterator to the begin/end of a column.  Since this is column
     * major layout, we get the iterator for free by just using the pointer
     */
    float *row_begin(size_t row)  { return data + cols*row; }
    float *row_end(size_t row)    { return data + cols*(row+1); }
    const float *row_cbegin(size_t row) const {return data + cols*row; }
    const float *row_cend(size_t row)   const {return data + cols*(row+1); }

    void write(std::ostream & out) const {
		out.write((const char *)&rows, sizeof(rows));
		out.write((const char *)&cols, sizeof(cols));
		out.write((const char *)this->cbegin(), this->datasize());
    }

	void save(const std::string & outname) const {
        std::ofstream out(outname.c_str());
        this->write(out);
	}

    void read(std::istream & in) {
		in.read((char*)&rows, sizeof(rows));
		in.read((char*)&cols, sizeof(cols));
		if (data) delete[] data;
		data = new T[rows * cols];
		in.read((char*)data, this->datasize());
    }

	void load(const std::string & in_name) {
        std::ifstream in(in_name.c_str());
        this->read(in);
	}

    const ThinMatrix<T> trans(void) const;
    const ThinMatrix<T> SumCols(void) const;
    void SumCols(ThinMatrix<T> & out) const;
    const ThinMatrix<T> SumRows(void) const;
    void SumRows(ThinMatrix<T> & out) const;
    void setValue(T val);

    static const ThinMatrix<T> LoadFromFile(std::string filename) {
        ThinMatrix<T> tm;
        tm.load(filename);
        return tm;
    }

    T& operator()(size_t row, size_t col) {
    	//TODO: Bounds checks
    	return data[row*cols + col];
    }

    ThinMatrix<T> & operator*=(const T & scalar) {
    	std::transform(this->begin(),
    				   this->end(),
    				   this->begin(),
    				   ThinMatrixScope::UnaryScaleOp<T>(scalar));
    	return *this;
    }
    ThinMatrix<T> & operator/=(const T & scalar) {
    	std::transform(this->begin(),
    				   this->end(),
    				   this->begin(),
    				   ThinMatrixScope::UnaryDivOp<T>(scalar));
        	return *this;
    }
    ThinMatrix<T> & operator+=(const T & scalar) {
    std::transform(this->begin(),
    			   this->end(),
    			   this->begin(),
    			   ThinMatrixScope::UnaryAddOp<T>(scalar));
        	return *this;
    }
    ThinMatrix<T> & operator-=(const T & scalar) {
    	std::transform(this->begin(),
    				   this->end(),
    				   this->begin(),
    				   ThinMatrixScope::UnaryScaleOp<T>(-scalar));
        	return *this;
    }

    const ThinMatrix<T> operator*(const T & scalar) const {
    	return ThinMatrix<T>(*this) *= scalar;
    }
    const ThinMatrix<T> operator/(const T & scalar) const {
    	return ThinMatrix<T>(*this) /= scalar;
    }
    const ThinMatrix<T> operator+(const T & scalar) const {
    	return ThinMatrix<T>(*this) += scalar;
    }
    const ThinMatrix<T> operator-(const T & scalar) const {
    	return ThinMatrix<T>(*this) -= scalar;
    }

};

template <typename T>
const ThinMatrix<T> ThinMatrix<T>::trans() const {
	ThinMatrix<T> other(this->cols, this->rows);
	for (size_t i = 0; i < this->rows; i++) {
		for (size_t j = 0; j < this->cols; j++) {
			size_t srcOffset = i*cols + j;
			size_t dstOffset = j*rows + i;
			other.data[dstOffset] = this->data[srcOffset];
		}
	}
	return other;
}

template <typename T>
void ThinMatrix<T>::SumCols(ThinMatrix<T> & out) const {
    //TODO: Size checking
    out.setValue(0.0f);
    T * writePtr = out.begin();
    const T * readPtr = this->cbegin();
    size_t idx = 0;
    while (idx < this->size()) {
        *writePtr += *readPtr++;
        idx++;
        if (0 == idx % this->cols) writePtr++;
    }
}

template <typename T>
const ThinMatrix<T> ThinMatrix<T>::SumCols(void) const {
    ThinMatrix<T> out(this->rows, 1);
    this->SumCols(out);
    return out;
}

template <typename T>
void ThinMatrix<T>::SumRows(ThinMatrix<T> & out) const {
    //TODO: Size checking
    out.setValue(0.0f);
    T * writePtr = out.begin();
    const T * readPtr = this->cbegin();
    size_t idx = 0;
    while (idx < this->size()) {
        *writePtr++ += *readPtr++;
        idx++;
        if (0 == idx % this->cols) writePtr = out.begin();
    }
}

template <typename T>
const ThinMatrix<T> ThinMatrix<T>::SumRows(void) const {
    ThinMatrix<T> out(1, this->cols);
    this->SumRows(out);
    return out;
}

template <typename T>
void ThinMatrix<T>::setValue(T val) {
   std::transform(this->begin(), this->end(), this->begin(), ThinMatrixScope::UnaryConstantOp<T>(val) );
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ThinMatrix<T> & tm) {
	os << "[";
	size_t idx = 0;
	const T*  ptr = tm.cbegin();
	while (idx < tm.size()) {
		os << *ptr++;
		idx++;
		if (idx % tm.size() == 0) {
			os << "]";
		}
		else if (idx % tm.cols == 0) {
			os << ";";
		} else
			os << ", ";
	}
	return os;
}


#endif /* THINMATRIX_H_ */
