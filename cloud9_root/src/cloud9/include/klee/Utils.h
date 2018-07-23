/*
 * utils.h
 *
 *  Created on: Feb 12, 2015
 *      Author: sjguo
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>

#include "llvm/Function.h"

using namespace llvm;
using namespace std;

namespace klee {

struct Utils {
	// Return a SMT-LIB2 const bitvector form of the passed unsigned
	static std::string to_const_bitvec(unsigned i);

	static bool skipFunction(const Function* f);
};

}

#endif /* UTILS_H_ */
