/**
 * Author: Markus Kusano
 *
 * See ControlDependence.h for more information
 */
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "ControlDependence.h"
#include <vector>

using namespace llvm;

namespace klee {

// Enable debugging to stderr
//#define MK_DEBUG

// From Ferrante et al. the algorithm for obtaining control dependency
// information is:
//
// Let S consist of all edges (A,B) in the control flow graph (A->B) such that
// B is not an ancestor of A in the post-dominator tree (ie B does not
// post-dominate A).
//
// Each pair (A,B) in S is then examined. Let L denote the least common
// ancestor of A and B in the post-dominator tree. L can be two things (see the
// paper for proofs)
//
// Case 1. L = parent of A. All nodes in the post-dominator tree on the path
// from L to B, including B but not L, should be made control dependent on A. 
//
// Case 2. L = A. All nodes in the post-dominator tree on the path from A to B,
// including A and B, should be made control dependent on A. (This case
// captures loop dependence.)
//
// This can be done by simply traversing backwards from B in the post-dominator
// tree until we reach A's parent (if it exists). We mark every node as control
// dependent on A.

void ControlDependence::getControlDependencies(Function &F, PostDominatorTree &PDT) {
	// All edges in the CFG (A->B) such that B does not post-dominate A
	vector<CFGEdge> S;

	S = getNonPDomEdges(F, PDT);

#ifdef MK_DEBUG
	errs() << "[MK DEBUG] Size of set S: " << S.size() << '\n';
#endif

	updateControlDependencies(S, PDT);

	// Generate a helper map which is the reverse of the control dependence graph
	for (depmap::iterator mi = controlDeps_.begin(), me = controlDeps_.end(); mi != me; ++mi) {
		std::set<BasicBlock *> curSet;
		curSet = mi->second;
		BasicBlock *A = mi->first;

		// controlDeps is a graph with edges (A, B) where B is the set of
		// BasicBlocks control dependent on A. To do the reverse, we insert an edge
		// to A for each BasicBlock in B.
		for (std::set<BasicBlock *>::iterator si = curSet.begin(), se = curSet.end(); si != se; ++si) {
			BasicBlock *curBlock = *si;
			reverseControlDeps_[curBlock].insert(A);
		} // for (std::set<BasicBlock *>::iterator
	} // for (depmap::iterator mi = controlDeps_.begin())
}

vector<ControlDependence::CFGEdge> ControlDependence::getNonPDomEdges(Function &F, const PostDominatorTree &PDT) const {

#ifdef MK_DEBUG
	errs() << "[MK DEBUG] in ControlDependence::getNonPDomEdges()\n";
#endif
	std::vector<CFGEdge> S;
	for (Function::iterator BBi = F.begin(), BBe = F.end(); BBi != BBe; ++BBi) {
		BasicBlock *A = &(*BBi);
#ifdef MK_DEBUG
		errs() << "[MK DEBUG] BasicBlock A: " << *A;
#endif
		// Get the edge A->B, ie get the successors of A
		for (succ_iterator Si = succ_begin(A), Se = succ_end(A); Si != Se; ++Si) {
			BasicBlock *B = *Si;
#ifdef MK_DEBUG
			errs() << "[MK DEBUG] BasicBlock B: " << *B;
#endif
			// properlyDominates - Returns true iff A dominates B and A != B.
			// See the base class file Dominators.h for some more documentation than
			// what is given in PostDominators.h
			//
			// Note: Ferrante et al. refer to dominance/post-dominance a little bit
			// differently than others. They do not consider A to dominate B if A ==
			// B. This is referred to as strictly dominates, or properly dominantes
			// in LLVM.
			if (!PDT.properlyDominates(B, A)) {
#ifdef MK_DEBUG
				errs() << "[MK DEBUG] B does not post-dominate A\n";
#endif
				// B does not post-dominate A in the edge (A->B), this is the criteria
				// for being in the set S
				CFGEdge e;
				e.head = B;
				e.tail = A; // head refers to the head of the arrow
				S.push_back(e);
			}
		}
#ifdef MK_DEBUG
		errs() << '\n';
#endif
	}

	return S;
}

void ControlDependence::updateControlDependencies(const vector<ControlDependence::CFGEdge> &S, PostDominatorTree &PDT) {
#ifdef MK_DEBUG
	errs() << "[MK DEBUG] in ControlDependence::updatecontroldependencies\n";
#endif
	BasicBlock *L;

	for (vector<CFGEdge>::size_type i = 0; i < S.size(); ++i) {
		CFGEdge curEdge;
		curEdge = S[i];

		BasicBlock *A;
		BasicBlock *B;

		A = curEdge.tail;
		B = curEdge.head; // (A->B)

#ifdef MK_DEBUG
		errs() << "[MK DEBUG] Examining blocks:\n" << *A << '\n' << *B;
#endif

		// Again, from Dominator.h documentation (rather than PostDominators.h):
		// findNearestCommonDominator - Find nearest common dominator basic block
		// for basic block A and B. If there is no such block then return NULL.
		L = PDT.findNearestCommonDominator(A, B);

		// FindNearestCommonDominator could return NULL. From the paper, the least
		// common dominator between A and B is either A
		// or the parent of A. I assume that when NULL is the nearest common
		// dominator that the common dominator is the "root" node, ie the EXIT
		// node of the entire program. Thus, this collapses to the case where L
		// is the parent of A (see comment below)

#ifdef MK_DEBUG
		if (L != NULL) {
			errs() << "[MK DEBUG] common post-dominator: " << *L << '\n';
		}
		else {
			errs() << "[MK DEBUG] common post-dominator: NULL\n";
		}
#endif

		DomTreeNode *domNodeA;
		domNodeA = PDT.getNode(A);

		DomTreeNode *parentA;
		parentA = domNodeA->getIDom(); // could be NULL?

		BasicBlock *parentABlock;

		if (parentA != NULL) {
			parentABlock = parentA->getBlock();
#ifdef MK_DEBUG
			errs() << "[MK DEBUG] post-dom parent of A: " << parentABlock << '\n';
#endif
		} else {
			parentABlock = NULL;
#ifdef MK_DEBUG
			errs() << "[MK DEBUG] post-dom parent of A: NULL\n";
#endif
		}

		// set of blocks dependent on A
		std::set<BasicBlock *> &depSet = controlDeps_[A];

		// Case 1. L = parent of A. All nodes in the post-dominator tree on the path
		// from L to B, including B but not L, should be made control dependent on A.
		// Case 2. L = A. All nodes in the post-dominator tree on the path from A to B,
		// including A and B, should be made control dependent on A. (This case
		// captures loop dependence.)
		//
		// Both cases are captured by traversing up from B until we reach A's
		// parent, marking every node dependent on A (including B) except A's
		// parent
		if (L == parentABlock || L == A) {
			// iterate up post-dom tree from B
			DomTreeNode *curNode;
			curNode = PDT.getNode(B);
			BasicBlock *curBlock;
			curBlock = curNode->getBlock();

			// on the first iteration we will insert B into the depSet which is what
			// we want
			while (curBlock != L) {
				assert(curBlock != L);
				depSet.insert(curBlock);
				curNode = curNode->getIDom();
				if (curNode == NULL) {
					// L can potentially be NULL
					curBlock = NULL;
				} else {
					curBlock = curNode->getBlock();
				}
			} // while (curBlock != L)
		} // if (L == parentABlock || L == A)
		else {
			llvm_unreachable("L is neither A nor the parent of A");
		}
	} // end for(vector<>)

#if 0
	DomTreeNode *domNodeB;
	domNodeB = PDT.getNode(B);
	DomTreeNode *curNode;
	curNode = domNodeB;
	// For an edge (A->B) we are building up the nodes that are control
	// dependent on A. We only need to do one query of the map to get the set
	// of nodes dependent on A, add more nodes to it, and then re-insert it
	// into the map
	std::set<BasicBlock *> &depSet = controlDeps_[A];
	// Operator[] should return a reference to the value referred to by the key
	// A. If there is no value then it should insert one and use the default
	// ctor (emtpy set).
	//depSet = controlDeps_[A];
	while (curNode != parentA) {
#ifdef MK_DEBUG
		errs() << "[DEBUG] Iterating up dom tree\n";
#endif
		// Mark each node visited on our way to the parent of A, but not A's
		// parent, as control dependent on A
		depSet.insert(curNode->getBlock());
		// Update cur
		curNode = curNode->getIDom();
	} // end while (cur != parentA)
	  // Because std::map::operator[] returns a reference to the set then I
	  // believe we do not need to do any insertions
#ifdef MK_DEBUG
	errs() << "[DEBUG] size of depSet: " << depSet.size() << '\n';
	errs() << "[DEBUG] size of map value: " << controlDeps_[A].size() << '\n';
	assert(depSet.size() == controlDeps_[A].size() && "map size mis-match");
#endif
#endif
}

void ControlDependence::toDot(std::string name) const {
	if (name.empty()) {
#ifdef MK_DEBUG
		errs() << "[DEBUG] dot file name is empty\n";
#endif
		name = "controldeps.dot";
	}

	// The set of nodes that have already been inserted into the dot file. This
	// is used so we don't define the same node twice in the file
	std::set<BasicBlock *> insertedNodes;

	// attempt to open output stream
	// std::ofstream out;
	//out.open(name, std::ios::out);

	std::string errInfo;
	std::string filename;
	filename = "cdg.";
	filename += name;
	filename += ".dot";

	//raw_fd_ostream out(filename.c_str(), errInfo);
	//raw_fd_ostream *out = new raw_fd_ostream(filename.c_str(), errInfo, sys::fs::OpenFlags::F_Text);
	raw_fd_ostream *out = new raw_fd_ostream(filename.c_str(), errInfo, sys::fs::file_type::character_file);

	// check for errors
	if (!errInfo.empty()) {
		errs() << "[Warning] Error opening output file: " << errInfo << '\n';
		return;
	}

	// header for dot file
	(*out) << "digraph \"CDG for " << name << " module\" {\n";

	// add label to graph
	(*out) << "label=\"CDG for \'" << name << "\' function \";\n";

#ifdef MK_DEBUG
	errs() << "[DEBUG] making dot file controlDeps_.size(): " << controlDeps_.size() << '\n';
#endif

	// create an edge for each dependency
	for (ControlDependence::depmap::const_iterator i = controlDeps_.begin(), ei = controlDeps_.end(); i != ei; ++i) {
		BasicBlock *tail;
		tail = i->first;

		// ensure that the tail node has been inserted
		if (insertedNodes.find(tail) == insertedNodes.end()) {
			insertedNodes.insert(tail);
			insertDotNode(*out, tail);
		}

		std::set<BasicBlock *> depSet;
		depSet = i->second;

		for (std::set<BasicBlock *>::const_iterator j = depSet.begin(), ej = depSet.end(); j != ej; ++j) {
			// In a CDG, Y is a descendent of X iff Y is control dependent on X
			// everything in the depset in control dependent up tail. So in this case
			// depset is Y and tail is X. Edges should go from tail -> depset[j]

			// insert the node if necessary
			if (insertedNodes.find(*j) == insertedNodes.end()) {
				// We've never encountered this basicblock before so create a node with
				// it in the file
				insertedNodes.insert(*j);
				insertDotNode(*out, *j);
			}

			// Insert the edge: TODO: I'm not sure if there is a chance the same edge
			// will be added twice
			insertDotEdge(*out, tail, *j);
		}
	}

	// closing brace
	(*out) << "}";

	// close the file
	out->close();
}

void ControlDependence::toDot(std::string name, depmap map) const {
	if (name.empty()) {
#ifdef MK_DEBUG
		errs() << "[DEBUG] dot file name is empty\n";
#endif
		name = "controldeps.dot";
	}

	// The set of nodes that have already been inserted into the dot file. This
	// is used so we don't define the same node twice in the file
	std::set<BasicBlock *> insertedNodes;

	// attempt to open output stream
	// std::ofstream out;
	//out.open(name, std::ios::out);

	std::string errInfo;
	std::string filename;
	filename = "cdg.";
	filename += name;
	filename += ".dot";

	//raw_fd_ostream out(filename.c_str(), errInfo);
//	raw_fd_ostream *out = new raw_fd_ostream(filename.c_str(), errInfo, sys::fs::OpenFlags::F_Text);
	raw_fd_ostream *out = new raw_fd_ostream(filename.c_str(), errInfo);

	// check for errors
	if (!errInfo.empty()) {
		errs() << "[Warning] Error opening output file: " << errInfo << '\n';
		return;
	}

	// header for dot file
	(*out) << "digraph \"CDG for " << name << " module\" {\n";

	// add label to graph
	(*out) << "label=\"CDG for \'" << name << "\' function \";\n";

#ifdef MK_DEBUG
	errs() << "[DEBUG] making dot file: map.size(): " << map.size() << '\n';
#endif

	// create an edge for each dependency
	for (ControlDependence::depmap::const_iterator i = map.begin(), ei = map.end(); i != ei; ++i) {
		BasicBlock *tail;
		tail = i->first;

		// ensure that the tail node has been inserted
		if (insertedNodes.find(tail) == insertedNodes.end()) {
			insertedNodes.insert(tail);
			insertDotNode(*out, tail);
		}

		std::set<BasicBlock *> depSet;
		depSet = i->second;

		for (std::set<BasicBlock *>::const_iterator j = depSet.begin(), ej = depSet.end(); j != ej; ++j) {
			// In a CDG, Y is a descendent of X iff Y is control dependent on X
			// everything in the depset in control dependent up tail. So in this case
			// depset is Y and tail is X. Edges should go from tail -> depset[j]

			// insert the node if necessary
			if (insertedNodes.find(*j) == insertedNodes.end()) {
				// We've never encountered this basicblock before so create a node with
				// it in the file
				insertedNodes.insert(*j);
				insertDotNode(*out, *j);
			}

			// Insert the edge: TODO: I'm not sure if there is a chance the same edge
			// will be added twice
			insertDotEdge(*out, tail, *j);
		}
	}

	// closing brace
	(*out) << "}";

	// close the file
	out->close();
}

void ControlDependence::insertDotNode(raw_fd_ostream &out, BasicBlock *node) const {
	assert(node != NULL && "NULL BasicBlock passed to insertDotNode");
	// Use the pointer value to make each node name unique. The label for the
	// node is the contents of the basicblock
	out << "Node" << node << " [shape=record, label=\"";

	// change newlines to \l for left alrignment in graphviz node
	std::string bbstr;
	raw_string_ostream bbo(bbstr);
	bbo << *node;

	// This is kind of a hack: only display the basicblock text up to the colon
	bbstr = bbstr.substr(0, bbstr.find(':'));

	std::size_t hit;
	hit = bbstr.find('\n');

	while (hit != std::string::npos) {
#ifdef MK_DEBUG
		errs() << "[DEBUG} \\n found at pos: " << hit << '\n';
#endif
		bbstr.erase(hit, 1);
		bbstr.insert(hit, "\\l");
		hit = bbstr.find('\n');
	}

	// label body to dot file
	out << bbstr;

	// Close the label and the node
	out << "\"];\n";
}

void ControlDependence::insertDotEdge(raw_fd_ostream &out, BasicBlock *A, BasicBlock *B) const {
	// Nodes are named by their address
	out << "Node" << A << "->" << "Node" << B << '\n';
}

ControlDependence::CFGEdge::CFGEdge() {
	tail = NULL;
	head = NULL;
}

}
