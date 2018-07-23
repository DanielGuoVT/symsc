/*
 * utils.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: sjguo
 */

#include "klee/Utils.h"
#include <iomanip>
#include <sstream>
#include <string>

#include "llvm/Function.h"

using namespace llvm;
using namespace std;

namespace klee {

std::string Utils::to_const_bitvec(unsigned i) {
	std::stringstream stream;
	// Ensure that the constant is padded with zeros equal to the number of hex
	// digits in an unsigned (numbers of bytes * 2)
	unsigned num_nibbles = sizeof(unsigned) * 2;
	stream << "#x" << std::setfill('0') << std::setw(num_nibbles) << std::hex << i;

	std::string ret(stream.str());

	return ret;
}

bool Utils::skipFunction(const Function *f) {
	if (!f || !(f->hasName())) {
		return true;
	}
	std::string name = f->getName().str();
	if (name.substr(0, sizeof("PROGA_body__")) == "PROGA_body__") {
		return false;
	}
	if (name.substr(0, sizeof("FB_G4LTL_body__")) == "FB_G4LTL_body__") {
		return false;
	}
	return true;
}
}

