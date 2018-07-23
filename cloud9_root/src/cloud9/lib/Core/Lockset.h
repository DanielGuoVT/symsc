/*
 * Lockset.h
 *
 *  Created on: Feb 17, 2015
 *      Author: sjguo
 */

#ifndef LOCKSET_H_
#define LOCKSET_H_

#include <set>
#include <string>
#include <map>

using namespace std;

namespace klee {

class Lockset {
public:
	typedef set<int>::iterator iterator;
	set<int> locks;

public:
	Lockset() {
		locks.clear();
	}
	Lockset(Lockset &set);
	~Lockset() {
	}

	void insert(int lid);
	void remove(int lid);
	bool mutual_exclusive(Lockset &lkst);

	inline bool has_member(int lid) {
		return locks.find(lid) != locks.end();
	}

	Lockset intersect(Lockset);
	Lockset intersect(Lockset*);
	string toString();

	bool empty() {
		return locks.empty();
	}

	int get_lock() {
		return *locks.begin();
	}

	Lockset& operator=(Lockset);
};

class Locksets {
public:
	typedef std::map<int, Lockset*>::iterator iterator;

public:
	Locksets();
	~Locksets();
	Locksets(Locksets &copy);

	void add_thread(int tid);
	void remove_thread(int tid);

	Lockset* get_lockset(int tid);
	void acquire(int tid, int mid);
	void release(int tid, int mid);

	Locksets & operator =(Locksets& sets);
public:
	std::map<int, Lockset*> sets;
};

}
#endif /* LOCKSET_H_ */
