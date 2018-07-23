#ifndef DIFFICULTYSCORE_H_
#define DIFFICULTYSCORE_H_

#include "../AST/AST.h"
#include "../AST/AST.h"
#include "../AST/ASTKind.h"

// estimate how difficult that input is to solve based on some simple rules.

namespace BEEV
{
struct DifficultyScore
{
private:
         // maps from nodeNumber to the previously calculated difficulty score..
        map<int, int> cache;

	static bool isLikeDivision(const Kind& k)
	{
		return k == BVMULT || k == BVDIV || k == BVMOD || k == SBVDIV || k == SBVREM || k == SBVMOD;
	}

	int visit(const ASTNode& b, ASTNodeSet& visited)
	{
		if (b.isAtom())
		  return 0;

		ASTNodeSet::iterator it;

		if ((it = visited.find(b)) != visited.end())
			return 0; // already included.

		visited.insert(b);

		const Kind k =b.GetKind();

		// These scores are approximately the number of AIG nodes created when
		// no input values are known.
		int score= 0;
		if (k == BVMULT)
		    score = (5 * b.GetValueWidth() * b.GetValueWidth() );
		else if (k == BVMOD)
		  score = (15 * b.GetValueWidth() * b.GetValueWidth() );
		else if (isLikeDivision(k))
		  score = (20 * b.GetValueWidth() * b.GetValueWidth() );
		else if (k == BVCONCAT || k == BVEXTRACT || k == NOT)
		{} // no harder.
		else if (k == EQ || k == BVGE || k == BVGT|| k == BVSGE || k == BVSGT )
		{
			// without getting the width of the child it'd always be 2.
			score = std::max(b[0].GetValueWidth(),1u) * (b.Degree());
		}
                else if (k == BVSUB)
                  {
                    // We convert subtract to a + (-b), we want the difficulty scores to be same.
                    score = std::max(b[0].GetValueWidth(),1u) * 3;
                  }
		else
		{
			score = std::max(b.GetValueWidth(),1u) * (b.Degree());
		}

		const ASTVec& c = b.GetChildren();
		ASTVec::const_iterator itC = c.begin();
		ASTVec::const_iterator itendC = c.end();
		for (; itC != itendC; itC++)
		{
			score += visit(*itC, visited);
		}

		return score;
	}

public:
	int score(const ASTNode& top)
	{
		if (cache.find(top.GetNodeNum()) != cache.end())
	          return cache.find(top.GetNodeNum())->second;

	        ASTNodeSet visited;
		int result = visit(top,visited);
		cache.insert(make_pair(top.GetNodeNum(), result));
		return result;
	}
};
};

#endif /* DIFFICULTYSCORE_H_ */



