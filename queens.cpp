//
//  queens.cpp
//  id2204-Assignment1
//
//  Created by Vaikunth Srinivasan A on 12/04/18.
//  Copyright (c) 2018 Vaikunth Srinivasan A. All rights reserved.
//  Inspired by the n-queens example by Christian Schulte from the Gecode example programs.

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

/**
 * Place n queens on an n times n chessboard such that they do not
 * attack each other.
 */
class Queens : public Script {
private:
    // Position of queens on boards
    const int n;
    BoolVarArray board;
public:
    /// The actual problem
    Queens(const SizeOptions& opt)
    : Script(opt), n(opt.size()), board(*this, opt.size() * opt.size(), 0, 1) {
        // Create a matrix representing the board
        Matrix<BoolVarArgs> matrix(board, n);
        // There must be only one queen per row
        for (int i = 0; i < n; i++)
            linear(*this, matrix.row(i), IRT_EQ, 1, opt.ipl());
        // There must be only one queen per column
        for (int i = 0; i < n; i++)
            linear(*this, matrix.col(i), IRT_EQ, 1, opt.ipl());
        
        // There must be only one queen per diagonal
        // Only the bottom left and right have to be checked as the top left and right have already been checked in a previous iteration.

        for (int x = 0; x < n; x++) {
            for (int y = 0; y < n; y++) {
                BoolVarArgs d1;
                BoolVarArgs d2;
                
                int xx = x, yy = y;
                // Checking the bottom right
                while (xx < n && yy < n)
                    d1 << matrix(xx++, yy++);
                linear(*this, d1, IRT_LQ, 1, opt.ipl());
                
                xx = x, yy = y;
                // Checking the bottom left
                while (xx  >= 0 && yy < n)
                    d2 << matrix(xx--, yy++);
                linear(*this, d2, IRT_LQ, 1, opt.ipl());
            }
        }
        
        // INT_VAL_MAX seems to be the best branching option, the number of nodes is reduced significantly. However, the execution time is increased largely.
        // Random variable can be used in branching. It reduces the size of the search tree and the number of propogations, although the execution time is still large.
        branch(*this, board, INT_VAR_SIZE_MIN(), INT_VAL_MAX());
    }
    
    // Constructor for cloning
    Queens(bool share, Queens& s) : Script(share,s), n(s.n) {
        board.update(*this, share, s.board);
    }
    
    // Perform copying during cloning
    virtual Space*
    copy(bool share) {
        return new Queens(share,*this);
    }
    
    // Print solution
    virtual void
    print(std::ostream& os) const {
        os << "\t";
        for (int i = 0; i < board.size(); i++) {
            os << board[i] << "\t";
            if ((i+1) % n == 0)
                os << std::endl << "\t";
        }
        os << std::endl;
    }
};

/**
 * Main function
 */
int
main(int argc, char* argv[]) {
    SizeOptions opt("Queens");
    opt.iterations(1);
    // Default problem size
    opt.size(5);
    opt.solutions(1);
    
    opt.parse(argc,argv);
    Script::run<Queens,DFS,SizeOptions>(opt);
    return 0;
}
/*


Pros and cons VS "std model"
Pros:
* Lower values per variable
* Lower constraints
Cons:
* Number of variables is higher.
* And the number combinations are a lot higher than they are in the standard model 
*/
