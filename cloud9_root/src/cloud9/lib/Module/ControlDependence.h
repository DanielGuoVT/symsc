/** 
 * Author: Markus Kusano
 *
 * Implementation of control dependence algorithm from:
 *
 * ``The Program Dependence Graph and Its Use
 * in Optimization'' Ferrante et al. 1987
 *
 * This code currently does not implement region nodes as described in the paper.
 *
 * This class requires the PostDominatorTree.
 *
 * The entry point for this analysis is the function getControlDepenence().
 * This function is passed an llvm::Function and the result of running the
 * PostDominatorTree analysis on this function. This function then updates
 * internal data structures with control depenendencies based on the analysis.
 */
#pragma once

#include "llvm/Analysis/PostDominators.h"

#include <map>
#include <set>
#include <string>

using namespace llvm;

using std::vector;

namespace klee {

class ControlDependence {
  public:

    // An edge in the CFG. This is an edge from tail to head (tail->head).
    //
    // This implies that head is a successor of tail and tail is a predecessor
    // of head. Head refers to the head of the arrow.
    struct CFGEdge {
      public:
        // ctor: sets tail and head to NULL
        CFGEdge();

        BasicBlock *tail;
        BasicBlock *head;
    };

    // PDT is assumed to be the PostDominatorTree of the Function F. This will
    // update internal data structures with the control dependency information.
    void getControlDependencies(Function &F, PostDominatorTree &PDT);

    // Map from BasicBlock to a set of BasicBlocks.
    //
    // The returned set from the map query contains all the basicblocks control
    // dependent on A
    //
    // This can be used to ask the question: "What is control dependent on the
    // basicblock A?"
    typedef std::map<BasicBlock *, std::set<BasicBlock *> > depmap;
    depmap controlDeps_;

    // Map from BasicBLock to a set of BasicBLocks
    //
    // This is the reverse of the map above (depmap)
    //
    // The returned set from a map query of BasicBlock A contains all the
    // BasicBlocks which A is dependent on.
    //
    // This can be used to ask the question: "Which blocks control the
    // execution of a basicblock?"
    depmap reverseControlDeps_;

    // Dump the contents of controlDeps_ to a .dot file with the given name. If
    // name is empty then the name will be "controldeps.dot"
    void toDot(std::string name) const;

    // Dump the contents of the passed map to a .dot file with the given name
    void toDot(std::string name, depmap map) const;

  private:
    // Returns the set S as described in Ferrante et al. (see
    // ControlDependence.cpp for a description of the algorithm)
    //
    // This is the set S of CFG edges (A->B) where B does not post dominate A.
    vector<CFGEdge> getNonPDomEdges(Function &F,
        const PostDominatorTree &PDT) const ;

    // Updates class internal control dependency information with the set S
    // (see getNonPDomEdges()) and the PostDominatorTree.
    //
    // For each edge (A->B) in S, this finds the least common ancestor in the
    // post-dom tree L. L can have two cases:
    //
    //
    // Case 1. L = parent of A. All nodes in the post-dominator tree on the path
    // from L to B, including B but not L, should be made control dependent on A.
    //
    // Case 2. L = A. All nodes in the post-dominator tree on the path from A to B,
    // including A and B, should be made control dependent on A. (This case
    // captures loop dependence.)
    void updateControlDependencies(const vector<CFGEdge> &S,
        PostDominatorTree &PDT);


    // Inserts a properly formatted .dot (graphviz) node into the raw_fd_ostream
    void insertDotNode(raw_fd_ostream &out, BasicBlock *node) const;

    // Inserts an edge from A to B (A->B) in dot syntax in the passed raw_fd_ostream
    void insertDotEdge(raw_fd_ostream &out, BasicBlock *A, BasicBlock *B) const;
};
}
