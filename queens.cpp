
#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>

using namespace Gecode;

class Queens: public Script {
public:
  //The postion of the queens on the boards
  IntVarArray q;
  int n;
    
  //n x n with allowed values 0 & 1
  Queens(const SizeOptions& opt) : Script(opt), q(*this, opt.size() * opt.size(), 0, 1){
    n = opt.size();

    //Matrix rep of the board.
    Matrix<IntVarArray> m(q, opt.size(), opt.size());
      
    // n is the total number of queens (count)
    count(*this, q, 1, IRT_EQ, n);

    //There must only be one queen per row.
    for (int i = 0; i < n; i++) {
      count(*this, m.row(i), 1, IRT_EQ, 1);
    }
    //There must be only one queen per column.
    for (int i = 0; i < n; i++) {
      count(*this, m.col(i), 1, IRT_EQ, 1);
    }
    // There must be only one queen per diagonal.
    IntVarArray diagonal(*this, n);
    std::cout << "To the left side, going down the diagonals" << std::endl;

        for (int i = 0; i < n; i++) {
            std::cout << "New diagonal" << std::endl;

            //Set the whole diagonal array to 0s.
            for (int k = 0; k < n; k++) {
                diagonal[k] = IntVar(*this, 0, 0);
            }

            for (int j = 0; j < n - i; j++) {
                std::cout << "(" << i + j << "," << j << ")" << std::endl;
                diagonal[j] = m(i + j, j);
            }

            count(*this, diagonal, 1, IRT_LQ, 1);
        }

        std::cout << "To the left side, going up the diagonals" << std::endl;

        for (int i = 0; i < n; i++) {
            std::cout << "New diagonal" << std::endl;

            //Set the whole diagonal array to 0s.
            for (int k = 0; k < n; k++) {
                diagonal[k] = IntVar(*this, 0, 0);
            }

            for (int j = 0; j <= i; j++) {
                std::cout << "(" << i - j << "," << j << ")" << std::endl;
                diagonal[j] = m(i - j, j);
            }

            count(*this, diagonal, 1, IRT_LQ, 1);
        }

        std::cout << "To the right side, going down the diagonals" << std::endl;

        for (int i = 0; i < n; i++) {
            std::cout << "New diagonal" << std::endl;

            //Set whole the diagonal array to 0s.
            for (int k = 0; k < n; k++) {
                diagonal[k] = IntVar(*this, 0, 0);
            }

            for (int j = (n - 1); j >= i; j--) {
                std::cout << "(" << (n - 1) + i - j << "," << j << ")" << std::endl;
                diagonal[j] = m((n - 1) + i - j, j);
            }

            count(*this, diagonal, 1, IRT_LQ, 1);
        }

        std::cout << "To the right side, going up the diagonals" << std::endl;
        //Diagonals, right side, going up
        for (int i = 0; i < n; i++) {
            std::cout << "New diagonal" << std::endl;

            //Set the whole diagonal array to 0s.
            for (int k = 0; k < n; k++) {
                diagonal[k] = IntVar(*this, 0, 0);
            }

            for (int j = (n - 1) - i; j < n; j++) {
                std::cout << "(" << i + j - (n - 1) << "," << j << ")" << std::endl;
                diagonal[j] = m(i + j - (n - 1), j);
            }

            count(*this, diagonal, 1, IRT_LQ, 1);
        }

        // Branching over the matrix.
        branch(*this, q, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }
  // Constructor used in cloning
    Queens(bool share, Queens& s) : Script(share,s) {
        q.update(*this, share, s.q);
    }

    // To perform copy during clone
    virtual Space* copy(bool share) {
        return new Queens(share,*this);
    }

    // Solution printer
    virtual void print(std::ostream& os) const {
        std::cout << "" << std::endl << std::endl;

        for (int i = 0; i < n * n; i++) {
            if (i != 0 && i % n == 0) {
                std::cout << std::endl;
            }
            std::cout << q[i] << " ";
        }

        std::cout << std::endl;
      }
  };
  int main(int argc, char* argv[]) {
    SizeOptions opt("Queens");
    opt.iterations(500);
    opt.size(8); //Size parameter size of chess board.

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
