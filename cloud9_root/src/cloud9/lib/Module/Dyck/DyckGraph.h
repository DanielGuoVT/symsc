/*
 * DyckGraph.h
 *
 *  Created on: Jun 16, 2014
 *      Author: sjguo
 */

#ifndef DYCKGRAPH_H_
#define DYCKGRAPH_H_

#include "DyckVertex.h"
#include <tr1/unordered_map>
#include <stack>
#include <set>

using namespace std;

namespace klee {

/// This class models a dyck-cfl language as a graph, which does not contain the barred edges.
/// See details in http://dl.acm.org/citation.cfm?id=2491956.2462159&coll=DL&dl=ACM&CFID=379446910&CFTOKEN=65130716 .
class DyckGraph {
private:
	set<DyckVertex*> vertices;
	set<DyckVertex*> reps;

	tr1::unordered_map<void *, DyckVertex*> val_ver_map;
public:

	/// The number of vertices in the graph.
	unsigned int numVertices();

	/// The number of equivalent sets.
	/// Please use it after you call void qirunAlgorithm().
	unsigned int numEquivalentClasses();

	/// Add a vertex into the graph
	bool addVertex(DyckVertex* ver);

	/// Get the set of vertices in the graph.
	set<DyckVertex*>& getVertices();

	/// Get the representative of each equivalent set.
	/// From each representative, you can find its equivalent set.
	/// Use it after you call void qirunAlgorithm().
	set<DyckVertex*>& getRepresentatives();

	/// You are not recommended to use the function when the graph is big,
	/// because it is time-consuming.
	void printAsDot(const char * filename) const;

	/// Combine x's rep and y's rep.
	void combine(DyckVertex* x, DyckVertex* y);

	// The graph contains a path between v1 and v2,
	// and the path does not contain the "label"
	//bool havePathsWithoutLabel(DyckVertex* v1, DyckVertex* v2, void* label);

	/// if value is NULL, a new vertex will be always returned with false.
	/// if value's vertex has been initialized, it will be returned with true;
	/// otherwise, it will be initialized and returned with false;
	/// If a new vertex is initialized, it will be added into the graph.
	pair<DyckVertex*, bool> retrieveDyckVertex(void * value, const char* name = NULL);

	/// The algorithm proposed by Qirun Zhang.
	/// Find the paper here: http://dl.acm.org/citation.cfm?id=2491956.2462159&coll=DL&dl=ACM&CFID=379446910&CFTOKEN=65130716 .
	/// Note that if there are two edges: a->b and a->c, b and c will be put into the same equivelant class.
	/// If the function does nothing, return true, otherwise return false.
	virtual bool qirunAlgorithm();

private:
	void removeFromWorkList(multimap<DyckVertex*, void*>& list, DyckVertex* v, void* l);

	bool containsInWorkList(multimap<DyckVertex*, void*>& list, DyckVertex* v, void* l);
};

}

#endif /* DYCKGRAPH_H_ */
