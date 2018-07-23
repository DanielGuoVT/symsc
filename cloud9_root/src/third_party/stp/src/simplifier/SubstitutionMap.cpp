/*
 * substitutionMap.cpp
 *
 */

#include "SubstitutionMap.h"
#include "simplifier.h"
#include "../AST/ArrayTransformer.h"

namespace BEEV
{

SubstitutionMap::~SubstitutionMap()
{
	delete SolverMap;
	delete nf;
}

// if false. Don't simplify while creating the substitution map.
// This makes it easier to spot how long is spent in the simplifier.
const bool simplify_during_create_subM = false;

//if a is READ(Arr,const) and b is BVCONST then return 1.
//if a is a symbol SYMBOL, return 1.
//if b is READ(Arr,const) and a is BVCONST then return -1
// if b is a symbol return -1.
//
//else return 0 by default
int TermOrder(const ASTNode& a, const ASTNode& b)
{
  const Kind k1 = a.GetKind();
  const Kind k2 = b.GetKind();

  if (k1 == SYMBOL)
  	return 1;

  if (k2 == SYMBOL)
  	return -1;


  //a is of the form READ(Arr,const), and b is const, or
  if ((k1 == READ
       && a[0].GetKind() == SYMBOL
       && a[1].GetKind() == BVCONST
       && (k2 == BVCONST)))
    return 1;

  //b is of the form READ(Arr,const), and a is const, or
  //b is of the form var, and a is const
  if ((k1 == BVCONST)
      && ((k2 == READ
           && b[0].GetKind() == SYMBOL
           && b[1].GetKind() == BVCONST)))
    return -1;

  return 0;
} //End of TermOrder()



//This finds conjuncts which are one of: (= SYMBOL BVCONST), (= BVCONST (READ SYMBOL BVCONST)),
// (IFF SYMBOL TRUE), (IFF SYMBOL FALSE), (IFF SYMBOL SYMBOL), (=SYMBOL SYMBOL)
// or (=SYMBOL BVCONST).
// It tries to remove the conjunct, storing it in the substitutionmap. It replaces it in the
// formula by true.
// The bvsolver puts expressions of the form (= SYMBOL TERM) into the solverMap.

ASTNode SubstitutionMap::CreateSubstitutionMap(const ASTNode& a,  ArrayTransformer*at)
{
  if (!bm->UserFlags.wordlevel_solve_flag)
    return a;

  ASTNode output;
  //if the variable has been solved for, then simply return it
  if (CheckSubstitutionMap(a, output))
    return output;

  if (!alreadyVisited.insert(a.GetNodeNum()).second)
  {
    return a;
  }

  output = a;

  //traverse a and populate the SubstitutionMap
  const Kind k = a.GetKind();
  if (SYMBOL == k && BOOLEAN_TYPE == a.GetType())
    {
      bool updated = UpdateSubstitutionMap(a, ASTTrue);
      output = updated ? ASTTrue : a;
    }
  else if (NOT == k && SYMBOL == a[0].GetKind())
    {
      bool updated = UpdateSubstitutionMap(a[0], ASTFalse);
      output = updated ? ASTTrue : a;
    }
  else if (IFF == k || EQ == k)
    {
	  ASTVec c = a.GetChildren();

	  if (simplify_during_create_subM)
		  SortByArith(c);

	  if (c[0] == c[1])
		  return ASTTrue;

      ASTNode c1;
      if (EQ == k)
    	  c1 = simplify_during_create_subM ? simp->SimplifyTerm(c[1]) : c[1];
      else// (IFF == k )
    	  c1= simplify_during_create_subM ?  simp->SimplifyFormula_NoRemoveWrites(c[1], false) : c[1];

      bool updated = UpdateSubstitutionMap(c[0], c1);
      output = updated ? ASTTrue : a;

      if (updated)
        {
          //fill the arrayname readindices vector if e0 is a
          //READ(Arr,index) and index is a BVCONST
          int to;
          if ((to =TermOrder(c[0],c1))==1 && c[0].GetKind() == READ)
              at->FillUp_ArrReadIndex_Vec(c[0], c1);
          else if (to ==-1 && c1.GetKind() == READ)
                    at->FillUp_ArrReadIndex_Vec(c1,c[0]);
        }
    }
  else if (XOR == k)
  {
	  if (a.Degree() !=2)
	    return output;

	  int to = TermOrder(a[0],a[1]);
	  if (0 == to)
	    {
	      if (a[0].GetKind() == NOT && a[0][0].GetKind() == EQ && a[0][0][0].GetValueWidth() ==1 && a[0][0][1].GetKind() == SYMBOL)
	        {
	          // (XOR (NOT(= (1 v)))  ... )
	          const ASTNode& symbol = a[0][0][1];
	          const ASTNode newN = nf->CreateTerm(ITE, 1, a[1], a[0][0][0], nf->CreateTerm(BVNEG, 1,a[0][0][0]));

                  if (UpdateSolverMap(symbol, newN))
                      output =  ASTTrue;
	        }
	      else if (a[1].GetKind() == NOT && a[1][0].GetKind() == EQ && a[1][0][0].GetValueWidth() ==1 && a[1][0][1].GetKind() == SYMBOL)
                {
                  const ASTNode& symbol = a[1][0][1];
                  const ASTNode newN = nf->CreateTerm(ITE, 1, a[0], a[1][0][0], nf->CreateTerm(BVNEG, 1,a[1][0][0]));

                  if (UpdateSolverMap(symbol, newN))
                      output =  ASTTrue;
                }
	      else if (a[0].GetKind() == EQ && a[0][0].GetValueWidth() ==1 && a[0][1].GetKind() == SYMBOL)
	        {
                  // XOR ((= 1 v) ... )

	          const ASTNode& symbol = a[0][1];
                  const ASTNode newN = nf->CreateTerm(ITE, 1, a[1], nf->CreateTerm(BVNEG, 1,a[0][0]), a[0][0]);

                  if (UpdateSolverMap(symbol, newN))
                      output =  ASTTrue;
	        }
	      else if (a[1].GetKind() == EQ && a[1][0].GetValueWidth() ==1 && a[1][1].GetKind() == SYMBOL)
                {
                  const ASTNode& symbol = a[1][1];
                  const ASTNode newN = nf->CreateTerm(ITE, 1, a[0], nf->CreateTerm(BVNEG, 1,a[1][0]), a[1][0]);

                  if (UpdateSolverMap(symbol, newN))
                      output =  ASTTrue;
                }
	      else
	      return output;
	    }
	  else
	    {
                ASTNode symbol,rhs;
                if (to==1)
                {
                    symbol = a[0];
                    rhs = a[1];
                }
                else
                {
                    symbol = a[1];
                    rhs = a[0];
                }

                assert(symbol.GetKind() == SYMBOL);

                if (UpdateSolverMap(symbol, nf->CreateNode(NOT, rhs)))
                   output =  ASTTrue;
	    }
  }
  else if (AND == k)
    {
	  const ASTVec& c = a.GetChildren();
	  ASTVec o;
      o.reserve(c.size());

      for (ASTVec::const_iterator
             it = c.begin(), itend = c.end();
           it != itend; it++)
        {
          simp->UpdateAlwaysTrueFormSet(*it);
          ASTNode aaa = CreateSubstitutionMap(*it,at);

          if (ASTTrue != aaa)
            {
              if (ASTFalse == aaa)
                return ASTFalse;
              else
                o.push_back(aaa);
            }
        }
      if (o.size() == 0)
        output =  ASTTrue;
      else if (o.size() == 1)
        output = o[0];
      else if (o != c)
    	output = nf->CreateNode(AND, o);
      else
    	output = a;
    }

  return output;
} //end of CreateSubstitutionMap()

// idempotent.
ASTNode SubstitutionMap::applySubstitutionMap(const ASTNode& n)
{
	bm->GetRunTimes()->start(RunTimes::ApplyingSubstitutions);
	ASTNodeMap cache;
	ASTNode result =  replace(n,*SolverMap,cache,nf, false);

	// NB. This is an expensive check. Remove it after it's been idempotent
	// for a while.
        #ifndef NDEBUG
              cache.clear();
              assert( result ==  replace(result,*SolverMap,cache,nf, false));
        #endif

	bm->GetRunTimes()->stop(RunTimes::ApplyingSubstitutions);
	return result;
}

// not always idempotent.
ASTNode SubstitutionMap::applySubstitutionMapUntilArrays(const ASTNode& n)
{
	bm->GetRunTimes()->start(RunTimes::ApplyingSubstitutions);
	ASTNodeMap cache;
	ASTNode result = replace(n,*SolverMap,cache,nf, true);
	bm->GetRunTimes()->stop(RunTimes::ApplyingSubstitutions);
	return result;
}

ASTNode SubstitutionMap::replace(const ASTNode& n, ASTNodeMap& fromTo,
                ASTNodeMap& cache, NodeFactory * nf)
{
    return replace(n,fromTo,cache,nf,false);
}

// NOTE the fromTo map is changed as we traverse downwards.
// We call replace on each of the things in the fromTo map aswell.
// This is in case we have a fromTo map: (x maps to y), (y maps to 5),
// and pass the replace() function the node "x" to replace, then it
// will return 5, rather than y.

// NB: You can't use this to map from "5" to the symbol "x" say.
// It's optimised for the symbol to something case.

ASTNode SubstitutionMap::replace(const ASTNode& n, ASTNodeMap& fromTo,
		ASTNodeMap& cache, NodeFactory * nf, bool stopAtArrays)
{
    const Kind k = n.GetKind();
	if (k == BVCONST || k == TRUE || k == FALSE)
    	return n;

	ASTNodeMap::const_iterator it;

    if ((it = cache.find(n)) != cache.end())
		return it->second;

	if ((it = fromTo.find(n)) != fromTo.end())
	{
		const ASTNode& r = it->second;
		assert(r.GetIndexWidth() == n.GetIndexWidth());
		ASTNode replaced = replace(r, fromTo, cache,nf,stopAtArrays);
		if (replaced != r)
			fromTo[n] = replaced;

		cache.insert(make_pair(n,replaced));
		return replaced;
	}

	// These can't be created like regular nodes are
	if (k == SYMBOL )
		return n;

	const unsigned int indexWidth = n.GetIndexWidth();
	if (stopAtArrays && indexWidth > 0) // is an array.
	{
	   return n;
	}

	const ASTVec& children = n.GetChildren();
	assert(children.size() > 0); // Should have no leaves left here.

	ASTVec new_children;
	new_children.reserve(children.size());

	for (ASTVec::const_iterator it = children.begin(); it != children.end(); it++)
	{
		new_children.push_back(replace(*it, fromTo, cache,nf,stopAtArrays));
	}

	assert(new_children.size() == children.size());

	// This code short-cuts if the children are the same. Nodes with the same children,
	// won't have necessarily given the same node if the simplifyingNodeFactory is enabled
	// now, but wasn't enabled when the node was created. Shortcutting saves lots of time.
	if (new_children == children)
	{
		cache.insert(make_pair(n,n));
		return n;
	}

	ASTNode result;
	const unsigned int valueWidth = n.GetValueWidth();

	if (valueWidth == 0 ) // n.GetType() == BOOLEAN_TYPE
	{
		result = nf->CreateNode(k, new_children);
	}
	else
	{
		// If the index and value width aren't saved, they are reset sometimes (??)
		result = nf->CreateArrayTerm(k,indexWidth, valueWidth,new_children);
	}

	// We may have created something that should be mapped. For instance,
	// if n is READ(A, x), and the fromTo is: {x==0, READ(A,0) == 1}, then
	// by here the result will be READ(A,0). Which needs to be mapped again..
	// I hope that this makes it idempotent.

	if (fromTo.find(result) != fromTo.end())
	  result = replace(result, fromTo, cache,nf,stopAtArrays);

	assert(result.GetValueWidth() == valueWidth);
	assert(result.GetIndexWidth() == indexWidth);

	cache.insert(make_pair(n,result));
	return result;
}

// Adds to the dependency graph that n0 depends on the variables in n1.
// It's not the transitive closure of the dependencies. Just the variables in the expression "n1".
// This is only needed as long as all the substitution rules haven't been written through.
void SubstitutionMap::buildDepends(const ASTNode& n0, const ASTNode& n1)
{
	if (n0.GetKind() != SYMBOL)
		return;

	if (n1.isConstant())
		return;

	vector<Symbols*> av;
	vars.VarSeenInTerm(vars.getSymbol(n1), rhs_visited, rhs, av);

	sort(av.begin(), av.end());
	for (int i =0; i < av.size();i++)
	{
		if (i!=0 && av[i] == av[i-1])
			continue; // Treat it like a set of Symbol* in effect.

		ASTNodeSet* sym = (vars.TermsAlreadySeenMap.find(av[i])->second);
		if(rhsAlreadyAdded.find(sym) != rhsAlreadyAdded.end())
			continue;
		rhsAlreadyAdded.insert(sym);

		//cout << loopCount++ << " ";
		//cout << "initial" << rhs.size() << " Adding: " <<sym->size();
		rhs.insert(sym->begin(), sym->end());
		//cout << "final:" << rhs.size();
		//cout << "added:" << sym << endl;

	}

	assert (dependsOn.find(n0) == dependsOn.end());
	dependsOn.insert(make_pair(n0,vars.getSymbol(n1)));
}

// Take the transitive closure of the varsToCheck. Storing the result in visited.
void SubstitutionMap::loops_helper(const set<ASTNode>& varsToCheck, set<ASTNode>& visited)
{
	set<ASTNode>::const_iterator visitedIt = visited.begin();

	set<ASTNode> toVisit;
	vector<ASTNode> visitedN;

	// for each variable.
	for (set<ASTNode>::const_iterator varIt = varsToCheck.begin(); varIt != varsToCheck.end(); varIt++)
	{
		while (visitedIt != visited.end() && *visitedIt < *varIt )
		        visitedIt++;

		if ((visitedIt != visited.end()) &&  *visitedIt == *varIt)
			continue;

		visitedN.push_back(*varIt);
		DependsType::iterator it;
		if ((it = dependsOn.find(*varIt)) != dependsOn.end())
		{
			Symbols* s= it->second;
			bool destruct;
			ASTNodeSet* varsSeen = vars.SetofVarsSeenInTerm(s,destruct);
			toVisit.insert(varsSeen->begin(), varsSeen->end());

			if (destruct)
				delete varsSeen;
		}
	}

	visited.insert(visitedN.begin(), visitedN.end());

	visitedN.clear();

	if (toVisit.size() !=0)
		loops_helper(toVisit, visited);
}


// If n0 is replaced by n1 in the substitution map. Will it cause a loop?
// i.e. will the dependency graph be an acyclic graph still.
// For example, if we have x = F(y,z,w), it would make the substitutionMap loop
// if there's already z = F(x).
bool SubstitutionMap::loops(const ASTNode& n0, const ASTNode& n1)
	{
	if (n0.GetKind() != SYMBOL)
		return false; // sometimes this function is called with constants on the lhs.

	if (n1.isConstant())
		return false; // constants contain no variables. Can't loop.

	// We are adding an edge FROM n0, so unless there is already an edge TO n0,
	// there is no change it can loop. Unless adding this would add a TO and FROM edge.
	if (rhs.find(n0) == rhs.end())
	{
		return vars.VarSeenInTerm(n0,n1);
	}

	if (n1.GetKind() == SYMBOL && dependsOn.find(n1) == dependsOn.end() )
		return false; // The rhs is a symbol and doesn't appear.

	if (debug_substn)
		cout << loopCount++  << endl;

	bool destruct = true;
	ASTNodeSet* dependN = vars.SetofVarsSeenInTerm(n1,destruct);

	if (debug_substn)
	{
		cout << n0 << " " <<  n1.GetNodeNum(); //<< " Expression size:" << bm->NodeSize(n1,true);
		cout << "Variables in expression: "<< dependN->size() << endl;
	}

	set<ASTNode> depend(dependN->begin(), dependN->end());

	if (destruct)
		delete dependN;

	set<ASTNode> visited;
	loops_helper(depend,visited);

	bool loops = visited.find(n0) != visited.end();

	if (debug_substn)
		cout << "Visited:" << visited.size() << "Loops:" << loops << endl;

	return (loops);
	}

};
