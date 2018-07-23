/*
 * ClockVector.cpp
 *
 *  Created on: Feb 17, 2015
 *      Author: sjguo
 */

#include "ClockVector.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <cassert>

using namespace std;
using namespace klee;

//////////////////////////////////////////////////////////////////////////
//
//  the implementation of ClockVector
//
//////////////////////////////////////////////////////////////////////////

ClockVector::ClockVector() :
		width(0) {
	timestamps.clear();
}

ClockVector::ClockVector(ClockVector& cv) :
		width(cv.width) {
	timestamps = cv.timestamps;
}

ClockVector::ClockVector(int num_threads) :
		width(num_threads) {
	this->resize(num_threads);
}

ClockVector::~ClockVector() {
}

void ClockVector::resize(int size, int init_val) {
	width = size;
	timestamps.resize(size, init_val);
}

/**
 *  increase the size of the clock vector by sz
 *  @param sz  the size that is going to increase
 *  @param init_val the initial value
 *
 */
void ClockVector::increase_size(int sz, int init_val) {
	assert( sz > 0);
	assert( (int)timestamps.size() == width);

	width += sz;
	timestamps.resize(width, init_val);
}

void ClockVector::clear() {
	timestamps.resize(width, -1);
}

int& ClockVector::operator[](int pos) {
	assert( pos >= 0 && pos < width);
	assert( (int)timestamps.size() == width);
	return timestamps[pos];
}

ClockVector * ClockVector::duplicate() {
	ClockVector * dupcopy;

	dupcopy = new ClockVector();
	dupcopy->width = this->width;
	dupcopy->timestamps = this->timestamps;
	return dupcopy;
}

string ClockVector::toString() {
	stringstream ss;
	int i;

	assert( width == (int)timestamps.size());

	ss << "[";
	for (i = 0; i < width; i++) {
		ss << setw(2) << timestamps[i];
		if (i < width - 1)
			ss << ", ";
	}
	ss << "]";

	return ss.str();
}

void ClockVector::merge(ClockVector & another_vec) {
	int i;
	assert( width == (int)timestamps.size());
	assert( (int)another_vec.size() == width);
	for (i = 0; i < width; i++) {
		timestamps[i] = ::max(timestamps[i], another_vec[i]);
	}
}

/**
 *
 */
bool ClockVector::must_happen_before(ClockVector & another_vec) {
	int i, max_width;
	vector<int> stamps1, stamps2;

	assert( width == (int)timestamps.size());

	stamps1 = timestamps;
	stamps2 = another_vec.timestamps;

	max_width = ::max(width, (int) stamps2.size());

	stamps1.resize(max_width, -1);
	stamps2.resize(max_width, -1);

	for (i = 0; i < max_width; i++) {
		if (stamps1[i] > stamps2[i])
			return false;
	}
	return true;
}

bool ClockVector::is_concurrent_with(ClockVector & another_vec) {
	if (must_happen_before(another_vec) || another_vec.must_happen_before(*this))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

ClockVectorList::ClockVectorList() {
	vecs.clear();
}

ClockVectorList::ClockVectorList(ClockVectorList &cvs) {
	iterator it;
	ClockVector * clock_vec = NULL;

	for (it = vecs.begin(); it != vecs.end(); it++) {
		clock_vec = *it;

	}
}

ClockVectorList::~ClockVectorList() {
	iterator it;
	ClockVector * vec;
	for (it = vecs.begin(); it != vecs.end(); it++) {
		vec = *it;
		delete vec;
	}
	vecs.clear();
}

ClockVectorList & ClockVectorList::operator=(ClockVectorList & cvlst) {
	iterator it;
	ClockVector * clock_vector;

	vecs.clear();
	for (it = cvlst.begin(); it != cvlst.end(); it++) {
		clock_vector = *it;
		vecs.push_back(clock_vector->duplicate());
	}

	return *this;

}

ClockVector * ClockVectorList::get_clock_vector(int tid) {
	assert( tid >= 0);
	assert( tid < (int)vecs.size());
	return vecs[tid];
}

void ClockVectorList::add_the_first_thread() {
	ClockVector * clock_vector;
	clock_vector = new ClockVector(1);
	clock_vector->timestamps[0] = 0;
	vecs.push_back(clock_vector);
}

void ClockVectorList::add_thread(int thread_id, int parent) {
	ClockVector * child_vector = NULL, *parent_vector = NULL;

	assert(thread_id > parent);
	parent_vector = vecs[parent];
	assert(parent_vector != NULL);

	parent_vector->increase_size(1);
	child_vector = parent_vector->duplicate();
	child_vector->timestamps[thread_id] = 0;
	assert((int)vecs.size() == thread_id);
	vecs.push_back(child_vector);
}
