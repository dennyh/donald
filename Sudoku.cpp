/*
 *  Authors:
 *    Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *    Christian Schulte, 2008-2018
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software, to deal in the software without restriction,
 *  including without limitation the rights to use, copy, modify, merge,
 *  publish, distribute, sublicense, and/or sell copies of the software,
 *  and to permit persons to whom the software is furnished to do so, subject
 *  to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

namespace {
	extern const int examples[][9][9];
	void print_Sudoku(const int x[][9][9], int n);
}

class Sudoku : public Script {
protected:
	IntVarArray l;
	const int ss = 9; // Sudoku size.
	const int bs = 3; // block size
public:
	// Branching variants
	enum {
		BRANCH_NONE,
		BRANCH_SIZE,
		BRANCH_SIZE_DEGREE,
		BRANCH_SIZE_AFC,
		BRANCH_AFC
	};

	Sudoku(const SizeOptions& opt) : Script(opt), l(*this, 81, 1, 9) {
		// Print unsolved Sudoku.
		std::cout << "Unsolved Sudoku: " << '\n';
		print_Sudoku(examples, opt.size());

		Gecode::Matrix<IntVarArray> m(l, ss, ss);

		// Constraints for rows and columns
		for (int i = 0; i<ss; i++) {
			distinct(*this, m.row(i), opt.ipl());
			distinct(*this, m.col(i), opt.ipl());
		}

		// Constraints for squares
		for (int i = 0; i<ss; i += bs) {
			for (int j = 0; j<ss; j += bs) {
				/*
				Don't understand how slice works. How are slices defined.
				This is the [fc,tc) [fr,tr) being used in the for-body.(9x9 sudoku)
				Every row should defines a block in the soduko.
				C: 0,3 : R: 0,3
				C: 0,3 : R: 3,6
				C: 0,3 : R: 6,9
				C: 3,6 : R: 0,3
				C: 3,6 : R: 3,6
				C: 3,6 : R: 6,9
				C: 6,9 : R: 0,3
				C: 6,9 : R: 3,6
				C: 6,9 : R: 6,9
				*/
				distinct(*this, m.slice(i, i + bs, j, j + bs), opt.ipl());
			}
		}

		// Fill-in predefined fields
		for (int i = 0; i<ss; i++)
			for (int j = 0; j < ss; j++) {
				if (int v = examples[opt.size()][j][i]) // zeros in sudoku evaluates to false
					rel(*this, m(i, j), IRT_EQ, v);
			}
	
		if (opt.branching() == BRANCH_NONE) {
			branch(*this, l, INT_VAR_NONE(), INT_VAL_SPLIT_MIN());
		}
		else if (opt.branching() == BRANCH_SIZE) {
			branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_SPLIT_MIN());
		}
		else if (opt.branching() == BRANCH_SIZE_DEGREE) {
			branch(*this, l, INT_VAR_DEGREE_SIZE_MAX(), INT_VAL_SPLIT_MIN());
		}
		else if (opt.branching() == BRANCH_SIZE_AFC) {
			branch(*this, l, INT_VAR_AFC_SIZE_MAX(opt.decay()), INT_VAL_SPLIT_MIN());
		}
		else if (opt.branching() == BRANCH_AFC) {
			branch(*this, l, INT_VAR_AFC_MAX(opt.decay()), INT_VAL_SPLIT_MIN());
		}
		//branch(*this, l, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
	}

	Sudoku(Sudoku& s) : Script(s) {
		l.update(*this, s.l);
	}
	virtual Space* copy(void) {
		return new Sudoku(*this);
	}
	virtual void print(std::ostream& os) const {
		//os << l << std::endl;
		os << "Solved Sudoku: " << '\n';
		for (int i = 0; i < ss*ss; i++) {
			os << " " << l[i];
			if ((i+1) % ss == 0)
				os << '\n';
		}
	}
};

int main(int argc, char* argv[]) {
	// Define commandline options
	SizeOptions opt("Sudoku");
	opt.size(0); // Decides what sudoku in the array we are working with.
	opt.ipl(IPL_DOM); // propagator strength of distinct. (IPL_DEF, IPL_VAL, IPL_BND and IPL_DOM)
	//DOM and BND seams pretty good.
	opt.solutions(1); // Find first solution.
	opt.branching(Sudoku::BRANCH_SIZE_AFC);
	opt.branching(Sudoku::BRANCH_NONE, "none", "none");
	opt.branching(Sudoku::BRANCH_SIZE, "size", "min size");
	opt.branching(Sudoku::BRANCH_SIZE_DEGREE, "sizedeg", "min size over degree");
	opt.branching(Sudoku::BRANCH_SIZE_AFC, "sizeafc", "min size over afc");
	opt.branching(Sudoku::BRANCH_AFC, "afc", "maximum afc");
	opt.parse(argc,argv);
	// run script
	Script::run<Sudoku,DFS,SizeOptions>(opt);
	return 0;
}

namespace {

	const int examples[][9][9] = {
    {// 0
	{0,0,0, 2,0,5, 0,0,0},
	{0,9,0, 0,0,0, 7,3,0},
	{0,0,2, 0,0,9, 0,6,0},
	
	{2,0,0, 0,0,0, 4,0,9},
	{0,0,0, 0,7,0, 0,0,0},
	{6,0,9, 0,0,0, 0,0,1},
    
	{0,8,0, 4,0,0, 1,0,0},
	{0,6,3, 0,0,0, 0,8,0},
	{0,0,0, 6,0,8, 0,0,0}
    },{// 1
	{3,0,0, 9,0,4, 0,0,1},
	{0,0,2, 0,0,0, 4,0,0},
	{0,6,1, 0,0,0, 7,9,0},

	{6,0,0, 2,4,7, 0,0,5},
	{0,0,0, 0,0,0, 0,0,0},
	{2,0,0, 8,3,6, 0,0,4},
    
	{0,4,6, 0,0,0, 2,3,0},
	{0,0,9, 0,0,0, 6,0,0},
	{5,0,0, 3,0,9, 0,0,8}
    },{// 2
	{0,0,0, 0,1,0, 0,0,0},
	{3,0,1, 4,0,0, 8,6,0},
	{9,0,0, 5,0,0, 2,0,0},
    
	{7,0,0, 1,6,0, 0,0,0},
	{0,2,0, 8,0,5, 0,1,0},
	{0,0,0, 0,9,7, 0,0,4},
    
	{0,0,3, 0,0,4, 0,0,6},
	{0,4,8, 0,0,6, 9,0,7},
	{0,0,0, 0,8,0, 0,0,0}
    },{	// 3 Fiendish puzzle April 21, 2005 Times London
	{0,0,4, 0,0,3, 0,7,0},
	{0,8,0, 0,7,0, 0,0,0},
	{0,7,0, 0,0,8, 2,0,5},
    
	{4,0,0, 0,0,0, 3,1,0},
	{9,0,0, 0,0,0, 0,0,8},
	{0,1,5, 0,0,0, 0,0,4},
    
	{1,0,6, 9,0,0, 0,3,0},
	{0,0,0, 0,2,0, 0,6,0},
	{0,2,0, 4,0,0, 5,0,0}
    },{	// 4 This one requires search
	{0,4,3, 0,8,0, 2,5,0},
	{6,0,0, 0,0,0, 0,0,0},
	{0,0,0, 0,0,1, 0,9,4},
    
	{9,0,0, 0,0,4, 0,7,0},
	{0,0,0, 6,0,8, 0,0,0},
	{0,1,0, 2,0,0, 0,0,3},
    
	{8,2,0, 5,0,0, 0,0,0},
	{0,0,0, 0,0,0, 0,0,5},
	{0,3,4, 0,9,0, 7,1,0}
    },{	// 5 Hard one from http://www.cs.mu.oz.au/671/proj3/node5.html
	{0,0,0, 0,0,3, 0,6,0},
	{0,0,0, 0,0,0, 0,1,0},
	{0,9,7, 5,0,0, 0,8,0},

	{0,0,0, 0,9,0, 2,0,0},
	{0,0,8, 0,7,0, 4,0,0},
	{0,0,3, 0,6,0, 0,0,0},

	{0,1,0, 0,0,2, 8,9,0},
	{0,4,0, 0,0,0, 0,0,0},
	{0,5,0, 1,0,0, 0,0,0}
    },{ // 6 Puzzle 1 from http://www.sudoku.org.uk/bifurcation.htm
	{1,0,0, 9,0,7, 0,0,3},
	{0,8,0, 0,0,0, 0,7,0},
	{0,0,9, 0,0,0, 6,0,0},
	{0,0,7, 2,0,9, 4,0,0},
	{4,1,0, 0,0,0, 0,9,5},
	{0,0,8, 5,0,4, 3,0,0},
	{0,0,3, 0,0,0, 7,0,0},
	{0,5,0, 0,0,0, 0,4,0},
	{2,0,0, 8,0,6, 0,0,9}
    },{ // 7 Puzzle 2 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 3,0,2, 0,0,0},
	{0,5,0, 7,9,8, 0,3,0},
	{0,0,7, 0,0,0, 8,0,0},
	{0,0,8, 6,0,7, 3,0,0},
	{0,7,0, 0,0,0, 0,6,0},
	{0,0,3, 5,0,4, 1,0,0},
	{0,0,5, 0,0,0, 6,0,0},
	{0,2,0, 4,1,9, 0,5,0},
	{0,0,0, 8,0,6, 0,0,0}
    },{ // 8 Puzzle 3 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 8,0,0, 0,0,6},
	{0,0,1, 6,2,0, 4,3,0},
	{4,0,0, 0,7,1, 0,0,2},
	{0,0,7, 2,0,0, 0,8,0},
	{0,0,0, 0,1,0, 0,0,0},
	{0,1,0, 0,0,6, 2,0,0},
	{1,0,0, 7,3,0, 0,0,4},
	{0,2,6, 0,4,8, 1,0,0},
	{3,0,0, 0,0,5, 0,0,0}
    },{ // 9 Puzzle 4 from http://www.sudoku.org.uk/bifurcation.htm
	{3,0,5, 0,0,4, 0,7,0},
	{0,7,0, 0,0,0, 0,0,1},
	{0,4,0, 9,0,0, 0,3,0},
	{4,0,0, 0,5,1, 0,0,6},
	{0,9,0, 0,0,0, 0,4,0},
	{2,0,0, 8,4,0, 0,0,7},
	{0,2,0, 0,0,7, 0,6,0},
	{8,0,0, 0,0,0, 0,9,0},
	{0,6,0, 4,0,0, 2,0,8}
    },{ // 10 Puzzle 5 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 7,0,0, 3,0,0},
	{0,6,0, 0,0,0, 5,7,0},
	{0,7,3, 8,0,0, 4,1,0},
	{0,0,9, 2,8,0, 0,0,0},
	{5,0,0, 0,0,0, 0,0,9},
	{0,0,0, 0,9,3, 6,0,0},
	{0,9,8, 0,0,7, 1,5,0},
	{0,5,4, 0,0,0, 0,6,0},
	{0,0,1, 0,0,9, 0,0,0}
    },{ // 11 Puzzle 6 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 6,0,0, 0,0,4},
	{0,3,0, 0,9,0, 0,2,0},
	{0,6,0, 8,0,0, 7,0,0},
	{0,0,5, 0,6,0, 0,0,1},
	{6,7,0, 3,0,1, 0,5,8},
	{9,0,0, 0,5,0, 4,0,0},
	{0,0,6, 0,0,3, 0,9,0},
	{0,1,0, 0,8,0, 0,6,0},
	{2,0,0, 0,0,6, 0,0,0}
    },{ // 12 Puzzle 7 from http://www.sudoku.org.uk/bifurcation.htm
	{8,0,0, 0,0,1, 0,4,0},
	{2,0,6, 0,9,0, 0,1,0},
	{0,0,9, 0,0,6, 0,8,0},
	{1,2,4, 0,0,0, 0,0,9},
	{0,0,0, 0,0,0, 0,0,0},
	{9,0,0, 0,0,0, 8,2,4},
	{0,5,0, 4,0,0, 1,0,0},
	{0,8,0, 0,7,0, 2,0,5},
	{0,9,0, 5,0,0, 0,0,7}
    },{ // 13 Puzzle 8 from http://www.sudoku.org.uk/bifurcation.htm
	{6,5,2, 0,4,8, 0,0,7},
	{0,7,0, 2,0,5, 4,0,0},
	{0,0,0, 0,0,0, 0,0,0},
	{0,6,4, 1,0,0, 0,7,0},
	{0,0,0, 0,8,0, 0,0,0},
	{0,8,0, 0,0,4, 5,6,0},
	{0,0,0, 0,0,0, 0,0,0},
	{0,0,8, 6,0,7, 0,2,0},
	{2,0,0, 8,9,0, 7,5,1}
    },{ // 14 Puzzle 9 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,6, 0,0,2, 0,0,9},
	{1,0,0, 5,0,0, 0,2,0},
	{0,4,7, 3,0,6, 0,0,1},
	{0,0,0, 0,0,8, 0,4,0},
	{0,3,0, 0,0,0, 0,7,0},
	{0,1,0, 6,0,0, 0,0,0},
	{4,0,0, 8,0,3, 2,1,0},
	{0,6,0, 0,0,1, 0,0,4},
	{3,0,0, 4,0,0, 9,0,0}
    },{ // 15 Puzzle 10 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,4, 0,5,0, 9,0,0},
	{0,0,0, 0,7,0, 0,0,6},
	{3,7,0, 0,0,0, 0,0,2},
	{0,0,9, 5,0,0, 0,8,0},
	{0,0,1, 2,0,4, 3,0,0},
	{0,6,0, 0,0,9, 2,0,0},
	{2,0,0, 0,0,0, 0,9,3},
	{1,0,0, 0,4,0, 0,0,0},
	{0,0,6, 0,2,0, 7,0,0}
    },{ // 16 Puzzle 11 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 0,3,0, 7,9,0},
	{3,0,0, 0,0,0, 0,0,5},
	{0,0,0, 4,0,7, 3,0,6},
	{0,5,3, 0,9,4, 0,7,0},
	{0,0,0, 0,7,0, 0,0,0},
	{0,1,0, 8,2,0, 6,4,0},
	{7,0,1, 9,0,8, 0,0,0},
	{8,0,0, 0,0,0, 0,0,1},
	{0,9,4, 0,1,0, 0,0,0}
    },{ // 17 From http://www.sudoku.org.uk/discus/messages/29/51.html?1131034031
	{2,5,8, 1,0,4, 0,3,7},
	{9,3,6, 8,2,7, 5,1,4},
	{4,7,1, 5,3,0, 2,8,0},
	{7,1,5, 2,0,3, 0,4,0},
	{8,4,9, 6,7,5, 3,2,1},
	{3,6,2, 4,1,0, 0,7,5},
	{1,2,4, 9,0,0, 7,5,3},
	{5,9,3, 7,4,2, 1,6,8},
	{6,8,7, 3,5,1, 4,9,2}
	}
	};

	// Print Soduku
	// Takes a 3d-Array of 9x9 sudokus and prints number n sudoku.
	void print_Sudoku(const int x[][9][9], int n) {
		for (int i = 0; i < 9; i++){
			for (int j = 0; j < 9; j++)
				if (x[n][i][j] != 0)
					std::cout << " " << x[n][i][j];
				else
					std::cout << " " << "-";
			std::cout << '\n';
		}
	}

}