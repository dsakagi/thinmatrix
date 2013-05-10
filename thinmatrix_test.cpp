/*
 * thinmatrix_test.cpp
 *
 * This only exists to test the thinmatrix functionality and ensure
 * it matches a reference implementation
 *
 *  Created on: May 1, 2013
 *      Author: dsakagi
 */


#include "thinmatrix.h"


// The reference implementation is boost
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
namespace ublas = boost::numeric::ublas;
typedef ublas::matrix<float, ublas::row_major> boostmat;
typedef ublas::unbounded_array<float> uba;

/*
 *  We'll use boost's random because they are so convenient
 *  This is just for the reference testing - there is no
 *  boost used inside thinmatrix (but only because NSight's indexer
 *  dies really quickly if you try to use boost headers)
 *
 *  I would use c++11, but I want to be sure that anything
 *  using the old standard could use it too
 */
#include <boost/random.hpp>
typedef boost::mt19937 twister_t;
typedef boost::uniform_01<float> unidist_t;
typedef boost::variate_generator<twister_t, unidist_t> gen_t;



int main(int argc, char **argv) {

	twister_t twister(time(0));
	unidist_t unidist;
	gen_t generator(twister, unidist);

	ThinMatrix<float> tm(10,12);
	std::generate(tm.begin(), tm.end(), generator);

	std::cout << tm << std::endl;

	boostmat bm(10,12);
	std::copy(tm.begin(), tm.end(), bm.data().begin());
	std::cout << bm << std::endl;

	ThinMatrix<float> tmt = tm.trans();
	boostmat bmt = ublas::trans(bm);

	std::cout << tmt << std::endl;
	std::cout << bmt << std::endl;

	tmt.save(std::string("test_output.tmf"));
	ThinMatrix<float> tmz;
	tmz.load(std::string("test_output.tmf"));

	std::cout << tmt << std::endl;
	std::cout << tmz << std::endl;

	ThinMatrix<float> rowsum = tm.SumRows() ;
	std::cout << "Row sum: " << rowsum << std::endl;
	ThinMatrix<float> colsum = tm.SumCols() ;
	std::cout << "Col sum: " << colsum << std::endl;


	return 0;
}
