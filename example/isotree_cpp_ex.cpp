#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include "isotree.hpp"

/* To compile this example from within the example/ folder, use:
g++ -o test isotree_cpp_ex.cpp $(ls ../src | grep ^[^R] | grep cpp | perl \
   -pe 's/^(\w)/..\/src\/\1/') -I../src -std=c++11 -O2
   Then run with './test'
*/

int which_max(std::vector<double> &v)
{
    auto loc_max_el = std::max_element(v.begin(), v.end());
    return std::distance(v.begin(), loc_max_el);
}


int main()
{
    /* Random data from a standard normal distribution
       (100 points generated randomly, plus 1 outlier added manually)
       Library assumes it is passed as a single-dimensional pointer,
       following column-major order (like Fortran) */
    int nrow = 101;
    int ncol = 2;
    std::vector<double> X( nrow * ncol );
    std::default_random_engine rng(1);
    std::normal_distribution<double> rnorm(0, 1);
    #define get_ix(row, col) (row + col*nrow)
    for (int col = 0; col < ncol; col++)
        for (int row = 0; row < 100; row++)
            X[get_ix(row, col)] = rnorm(rng);

    /* Now add obvious outlier point (3,3) */
    X[get_ix(100, 0)] = 3.;
    X[get_ix(100, 1)] = 3.;

    /* Fit a small isolation forest model
       (see 'fit_model.cpp' for the documentation) */
    ExtIsoForest iso;
    fit_iforest(NULL, &iso,
                X.data(),  ncol,
                NULL,    0,    NULL,
                NULL, NULL, NULL,
                2, 3, Normal,
                NULL, false, false,
                nrow, nrow, 500, 0,
                true, true,
                false, NULL,
                NULL, false,
                NULL, false,
                0., 0.,
                0.,  0.,
                0., Impute,
                SubSet, Smallest,
                false, NULL, 0,
                Higher, Inverse, false,
                1, 1);

    /* Check which row has the highest outlier score
       (see file 'predict.cpp' for the documentation) */
    std::vector<double> outlier_scores(nrow);
    predict_iforest(X.data(), NULL,
                    NULL, NULL, NULL,
                    NULL, NULL, NULL,
                    nrow, 1, true,
                    NULL, &iso,
                    outlier_scores.data(), NULL);

    int row_highest = which_max(outlier_scores);
    std::cout << "Point with highest outlier score: [";
    std::cout << X[get_ix(row_highest, 0)] << ", ";
    std::cout << X[get_ix(row_highest, 1)] << "]" << std::endl;

    return EXIT_SUCCESS;
}