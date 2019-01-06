#ifndef MATHTOOLKIT_NPMATRIX_H
#define MATHTOOLKIT_NPMATRIX_H

#include "thirdparty.h"
#include <NVector.h>

/**
 * @defgroup NAlgebra
 * @{
 * @class   NPMatrix
 * @date    04/05/2018
 * @author  samiBendou
 * @brief   A `NPMatrix<T>` inherits from `NVector<T>`. It's a representation of matrices of arbitrary
 *          size in a template field `T` (reals, complex, modular arithemics...).
 *
 * @details The matrix components are stored in a linear form using the index transformation \f$ k = p i + j \f$.
 *          The underlying `std::vector<T>` is represented as `t[p * i + j]`.
 *          The underlying `NVector<T>` is \f$ (A_{00}, A_{01}, ..., A_{0(P - 1)}, A_{10}, ..., A_{1(P - 1)}, ..., A_{(N-1)0}, ...) \f$.
 *
 *          Featuring algebraical operations such as matrix product, linear map, Gauss Jordan elimination.
 *          setters & getters, swappers and classic matrix generators such as ones, zeros...
 *
 *          The \f$ LU \f$ decomposition is stored as a property if the matrix is inversible. It is auto-updated only when needed.
 *          It allow to reduce complexity to get inverse or determinant.
 *
 *          All along this page we will use the following definitions :
 *             - `a`/`b`/`m`: An arbitrary given NPMatrix. The components are noted are noted \f$ A_{ij} \f$.
 *                            By default, \f$ A \f$ denote `this` matrix.
 *             - `R`        : Row of a matrix, also noted \f$ row_{i}(A) \f$
 *             - `C`        : Col of a matrix, also noted \f$ col_{i}(A) \f$
 *             - `n `       : Number of rows
 *             - `p `       : Number of Columns
 *             - `i `       : Row Index between \f$ 0 \leq i < n \f$
 *             - `j `       : Column Index \f$ 0 \leq j < p \f$
 *             - `k`, `l`   : either compound index or underlying `std::vector` array index.

 */


using namespace std;


template<typename T>
class NPMatrix : public NVector<T> {

    enum Parts {
        Row, Col
    };

public:
    // CONSTRUCTION

    /**
     *
     * @brief Construct a \f$ n \times p \f$ matrix initialized with `NVector(ul_t dim)` constructor.
     */
    explicit NPMatrix(ul_t n = 0, ul_t p = 0) : NPMatrix(NVector<T>(n * pIfNotNull(n, p)), n, pIfNotNull(n, p)) {}

    /**
     * @param data bi dimensional `std::vector` source.
     * @brief Construct a matrix using a bi-dimensional 'std::vector'. \f$ Aij \f$ represent `data[i][j]`.
     * @details All the `data[i]` must have the same length. They represent the rows of the matrix.
     */
    NPMatrix(const vector<vector<T> > &data) : NPMatrix(NVector<T>(data.size() * data[0].size()), data.size(),
                                                        data[0].size()) {
        copy(data);
    };

    /**
     *
     * @param m `NPMatrix` source.
     * @brief Construct a matrix by using `copy(m)` method.
     */
    NPMatrix(const NPMatrix<T> &m) : NPMatrix(NVector<T>(0), 0, 0) {
        copy(m);
    };

    /**
     * @param list bi dimensional `initializer_list` source.
     * @brief Construct a \f$ n \times p \f$ matrix using a bi-dimensional 'initializer_list'.
     * @details All the sub lists must have the same length.
     * `list` is converted to a bi dimensional `std::vector` before construction.
     */
    NPMatrix(initializer_list<initializer_list<T>> list) : NPMatrix(vector<vector<T>>(list.begin(), list.end())) {}

    /**
     * @param u `NVector` source of size \f$ q \f$
     * @param n Number of rows formed by u
     * @brief Construct a \f$ n \f$ rows matrix using a `std::vector` of size `1 * vector.dim()`.
     * @details The constructed matrix is of size \f$ n \times p =  q \f$ with \f$ p = q / n \f$. Resulting `p` is computed
     * using integer division so if reminder is not null a part of `u` will be truncated.
     */
    explicit NPMatrix(const NVector<T> &u, ul_t n = 1) : NPMatrix(u, n, u.dim() / n) {}

    /**
     *
     * @brief Construct a \f$ n \times p \f$ matrix using a `std::vector<NVector<T>>`.
     * All the vectors must have the same dimension. They represent the rows of the matrix.
     * @details The `NVector` array is converted to a bi dimensional `std::vector` before construction.
     */

    explicit NPMatrix(const vector<NVector<T> > &vectors) : NPMatrix(
            vector<vector<T>>(vectors.begin(), vectors.end())) {}

    ~NPMatrix() { lupClear(); }


    // SERIALIZATION

    /**
     * @brief Create a string representing the matrix.
     *
     * @details The returned string has the following form :
     *
     *  ```cpp
     *  "
     *  (A(0,0), A(0,1), ..., A(0,(P-1)))
     *  (A(1,0), A(1,1), ..., A(1,(P-1)))
     *  (A(i,0), ..., A(i,j, Ai(P-1)))
     *  (A((N-1),0), ..., ..., ...)
     *  "
     *  ```
     *
     * Each \f$ A_{ij} \f$ is formatted using the default `<<` operator of the type `T`.
     *
     * @return Returns a string representing the matrix.
     */
    std::string str() const override;

    // CHARACTERIZATION

    /**
     *
     * @return True if \f$ n = p \f$.
     */
    inline bool isSquare() const {
        bool cond = _j2 - _j1 == _i2 - _i1;
        setDefaultBrowseIndices();
        return cond;
    }

    bool isUpper() const;

    bool isLower() const;

    bool isDiagonal() const;

    // GETTERS

    /**
     * @name Getters
     * @{
     */

    /**
     *
     * @brief Number of \f$ n \f$ rows
     */
    inline ul_t n() const {
        auto res = _i2 - _i1 + 1;
        setDefaultBrowseIndices();
        return res;
    }

    /**
     *
     * @brief Number of \f$ p \f$ columns
     */
    inline ul_t p() const {
        auto res = _j2 - _j1 + 1;
        setDefaultBrowseIndices();
        return res;
    }

    /**
     *
     * @brief \f$ i \f$ row \f$row_i(A) \f$ of the matrix as a `NVector<T>`.
     */
    NVector<T> row(ul_t i) const;

    /**
     *
     * @brief \f$ j \f$ col  \f$col_j(A) \f$  the matrix as a `NVector<T>`.
     */
    NVector<T> col(ul_t j) const;

    /**
     *
     * @param i1 First row to be taken.
     * @param i2 Last row to be taken \f$ i_1 \lt i_2 \f$.
     * @brief Create an array containing the rows of the matrix.
     *
     * @details The behavior of `rows()` is the following :
     *          - `rows()` returns all the rows.
     *          - `rows(i1)` returns the rows \f$ [R_{i1}, R_{(i1+1)},..., R_{(n-1)}] \f$
     *          - `rows(i1, i2)` returns the rows \f$ [R_{i1}, R_{(i1+1)},..., R_{i2}] \f$
     *
     * @return Return an array containing the rows of the matrix as `Nvector`.
     */
    std::vector<NVector<T> > rows(ul_t i1 = 0, ul_t i2 = MAX_SIZE) const;

    /**
     *
     * @param j1 First column to be taken.
     * @param j2 Last column to be taken \f$ j_1 \lt j_2 \f$.
     * @brief Create an array containing the colomuns of the matrix in the form of `std::vector<NVector<T>>`.
     *
     * @details The behavior of `cols()` is the analog to `rows()`.
     *
     * @return Return an array containing the column of the matrix as `Nvector`.
     */
    std::vector<NVector<T> > cols(ul_t j1 = 0, ul_t j2 = MAX_SIZE) const;

    /**
     * @brief Create a new matrix containing upper part of this matrix.
     * @return upper part of this matrix. The lower part contains `0`.
     */
    NPMatrix<T> upper() const;

    /**
     * @brief Create a new matrix containing lower part of this matrix.
     * @return lower part of this matrix. The upper part contains `0`.
     */
    NPMatrix<T> lower() const;

    /**
     *
     * @brief \f$ L \f$ matrix of \f$ LU \f$ decomposition of the matrix.
     */
    NPMatrix<T> lupL() const;

    /**
     *
     * @brief \f$ U \f$ matrix of \f$ LU \f$ decomposition of the matrix.
     */
    NPMatrix<T> lupU() const;

    /** @} */

    /**
     * @name Setters
     * @brief Setters of a `NPMatrix`.
     * @details All the setters return a reference to `*this` in order to allow cascade calls eg `setRow().setCol()`.
     * Setters cannot be used with `operator()(ul_t, ul_t, ul_t, ul_t)`.
     * @return Setters returns a reference to `*this`.
     * @{
     */

    // SETTERS

    /**
     * @param u source `NVector`.
     * @param i1 index of row to set.
     * @brief Set row with given vector.
     * @details The dimension of the vector must be inferior or equal to the number of columns.
     */
    NPMatrix<T> &setRow(const NVector<T> &u, ul_t i1);

    /**
     * @param u source `NVector`.
     * @param j1 index of column to set.
     * @brief Set column with given vector.
     * @details Same behavior as `setRow()`.
     */
    NPMatrix<T> &setCol(const NVector<T> &u, ul_t j1);

    /**
     *
     * @param vectors   Rows to set on the matrix. `std::vector` of `NVector<T>`.
     *
     * @param i1        First row to set.
     * @brief           Replace the components of the matrix with the array of vectors.
     *
     * @details         The input `vectors` must verify the following conditions :
     *                  - The length of each `NVector<T>` must be inferior or equal to the number of cols.
     *                  - The total size of vectors must be inferior or equal to the number of rows.
     *
     *                  If `i1 + vectors.size()` is greater than `n` Then the algorithm truncate the
     *                  array of `NVector<T>`.
     *
     *                  If the size of `vectors` is \f$ n \times q \f$ than the `setRows(vectors)` will return :
     *
     *                  \f[ \begin{bmatrix}
     *                      v_{00}      & ... & ... & v_{0(p-1)} \\
     *                      v_{10}      & ... & ... & v_{1(p-1)} \\
     *                      v_{(q-1)0}  & ... & ... & v_{(q-1)(p-1)} \\
     *                      A_{q0}      & ... & ... & A_{q(p-1)} \\
     *                      ...         & ... & ... & ... \\
     *                      A_{(n-1)0}  & ... & ... & A_{(n-1)(j1 - 1)} \\
     *                      \end{bmatrix}
     *                  \f]
     *                  Where \f$ v_{ij} \f$ represent `vectors[i](j)`.
     */
    NPMatrix<T> &setRows(const std::vector<NVector<T> > &vectors, ul_t i1 = 0);

    /**
     *
     * @param vectors   Rows to set on the matrix. `std::vector` of `NVector<T>`.
     *
     * @param i1        Start index to set row.
     * @brief           Replace the components of the matrix with the array of vectors.
     *
     * @details         The behavior of `setCols()` is analog to `setRows()`.
     */
    NPMatrix<T> &setCols(const std::vector<NVector<T> > &vectors, ul_t j1 = 0);

    /** @} */

    // MANIPULATORS

    /**
     * @name Manipulators
     * @brief Manipulators such as swap, shift...
     * @details Manipulators allow cascade calls. The behavior is the same as Setters.
     * @{
     */

    // SWAP

    /**
     *
     * @param i1 first row indices to swap
     * @param j1 first col indices to swap
     * @param i2 second row indices to swap
     * @param j2 second col indices to swap
     * @brief Swap \f$ A_{i_1j_1} \f$ and \f$ A_{i_2j_2} \f$.
     */
    inline NPMatrix<T> &swap(ul_t i1, ul_t j1, ul_t i2, ul_t j2) {
        assert(isValidIndex(i1, j1) && isValidIndex(i2, j2));

        NVector<T>::swap(vectorIndex(i1, j1), vectorIndex(i2, j2));
        lupClear();
        return *this;
    }

    /**
     *
     * @param i1 first row indices to swap
     * @param i2 second row indices to swap
     * @brief Swap \f$ row_{i_1}(A) \f$ and \f$ row_{i_2}(A) \f$.
     */
    inline NPMatrix<T> &swapRow(ul_t i1, ul_t i2) { return swap(Row, i1, i2); }

    /**
     *
     * @param j1 first col indices to swap
     * @param j2 second col indices to swap
     * @brief Swap \f$ col_{j_1}(A) \f$ and \f$ col_{j_2}(A) \f$.
     */
    inline NPMatrix<T> &swapCol(ul_t j1, ul_t j2) { return swap(Col, j1, j2); }


    // SHIFT

    /**
     *
     * @param i index of row to shift
     * @param iterations number of times to shift.
     * @brief shift a \f$ i^{th} \f$ row iterations times.
     * @details If `iterations` is positive, shift is powered to the left, else to the right.
     * For example `shiftRow(0, 2)` will set the matrix to :
     *                  \f[ \begin{bmatrix}
     *                      A_{20}      & A_{30}        & ... & A_{(n-1)0} & A_{00}    & A_{10} \\
     *                      ...         & ...           & ... & ...        & ...       & ... \\
     *                      ...         & ...           & ... & ...        & ...       & ... \\
     *                      A_{(n-1)0}  & A_{(n-1)1}    & ... & ...        & ...       & A_{(n-1)(p-1)}\\
     *                      \end{bmatrix}
     *                  \f]
     *
     */
    inline NPMatrix<T> &shiftRow(ul_t i, long iterations = 1) { return shift(Row, i, iterations); }

    /**
     *
     * @param i index of column to shift
     * @param iterations number of times to shift.
     * @brief shift a \f$ j^{th} \f$ column iterations times.
     * @details The behavior is analog to `shiftRow()`. If `iterations` is positive,
     * shift is powered to the up, else to the bottom.
     */
    inline NPMatrix<T> &shiftCol(ul_t j, long iterations = 1) { return shift(Col, j, iterations); }

    /** @} */

    // ALGEBRA

    /**
     * @name Algebra
     * @brief Matrix algebraical operations.
     * @details Algebraical operations consists on method specific to matrices algebra such as inversion, determinant...
     *
     * @{
     */

    /**
     * @brief Transposed matrix.
     * @return Returns the value of transposed matrix.
     */
    NPMatrix<T> transposed() const;

    /**
     *
     * @brief Trace of this matrix \f$ A_{00} + A_{11} + ... + A_{(n-1)(n-1)} \f$
     * @return The value of the trace.
     */
    T trace() const;

    /**
     * @param m Matrix to concatenate.
     * @brief Concatenate matrix `m` to `this` matrix.
     * @details The shifted matrix is the matrix obtained after concatenation of \f$ A \f$
     * and \f$ M \f$ columns. \f$ A \f$ and \f$ M \f$ must have the same number of rows.
     * @return Returns the shifted matrix \f$ [ A | M ] \f$.
     */
    NPMatrix<T> shifted(const NPMatrix<T> &m) const;

    /**
     * @brief Apply Gauss Jordan elimination on matrix to calculate inverse without using \f$ LU \f$ decomposition.
     * @details To perform inverse computation, shift the matrix you want to invert than apply this function. If
     * the matrix is inversible, than the inverse of the matrix is on the right part of the matrix.
     * @return Reference to `*this` the shifted matrix.
     */
    NPMatrix<T> &reduce();

    /**
     * @brief determinant of this matrix det(A). Using the \f$ LU \f$ decomposition \f$ O(n) \f$.
     * @return Value of determinant.
     */
    T det() const;

    /** @} */

    /**
     *
     * @name Algebraical Operators
     * @brief Commons algebraical operations for matrix
     * @details All the algebraical operations are implemented the same way as they are on `NVector`. New algebraical
     * operations are documented here.
     *
     * @{
     */

    // OPERATORS

    // ALGEBRAICAL OPERATORS

    inline friend NPMatrix<T> operator+(NPMatrix<T> a, const NPMatrix<T> &b) {
        a += b;
        return a;
    }

    inline friend NPMatrix<T> operator-(NPMatrix<T> a, const NPMatrix<T> &b) {
        a -= b;
        return a;
    }

    inline friend NPMatrix<T> operator-(NPMatrix<T> m) {
        m.opp();
        return m;
    }

    inline friend NPMatrix<T> operator*(T s, NPMatrix<T> m) {
        m *= s;
        return m;
    }

    inline friend NPMatrix<T> operator*(NPMatrix<T> m, T s) { return s * m; }

    /**
     * @brief Usual matrix multiplication
     * @details The matrices must have the length. Natural \f$ O(n^3) \f$ matrix product is used.
     * @return \f$ A \times B \f$ product.
     */

    inline friend NPMatrix<T> operator*(NPMatrix<T> a, const NPMatrix<T> &b) {
        a *= (&a != &b ? b : a);
        return a;
    }

    /**
     * @brief Usual matrix vector product (linear mapping).
     * @details The number of rows of m must be equal to the dimension of v.
     * Natural \f$ O(n^2) \f$ linear mapping is used.
     * @return \f$ M v \f$ product.
     */
    inline friend NVector<T> operator*(const NPMatrix<T> &m, NVector<T> v) {
        m.vectorProduct(v);
        return v;
    }

    inline friend NPMatrix<T> operator/(NPMatrix<T> m, T s) {
        m /= s;
        return m;
    }

    /**
     *
     * @param m matrix to exponentiate.
     * @param exp exponent.
     * @brief Exponantiate matrix \f$ M \f$ using given exponent.
     * @details Fast exponentiation implementation.
     * If \f$ exp < 0 \f$ we calculate the power of the inverse matrix using `inv()` method.
     * @return \f$ M^{exp} \f$ exponentiated matrix.
     */
    inline friend NPMatrix<T> operator^(NPMatrix<T> m, long exp) {
        m ^= exp;
        return m;
    }

    /**
     * @param m matrix of the equation system.
     * @param v second member of the equation system.
     * @brief Solve the linear system formed by \f$ M \f$ and \f$ v \f$.
     * @details The linear system is \f$ MX = v \f$ where \f$ X \f$ is unknown.
     * This algorithm uses \f$ LU \f$ decomposition.
     * @return Vector solution of the system \f$ X \f$.
     */
    inline friend NVector<T> operator%(const NPMatrix<T> &m, NVector<T> v) {
        v %= m;
        return v;
    }


    // SCALAR PRODUCT BASED OPERATIONS

    friend T operator|(const NPMatrix<T> &a, const NPMatrix<T> &b) {
        NVector<T> sub_a{a(a._i1, a._j1, a._i2, a._j2)}, sub_b{b(b._i1, b._j1, b._i2, b._j2)};
        auto res = sub_a | sub_b;

        a.setDefaultBrowseIndices();
        b.setDefaultBrowseIndices();
        return res;
    }

    friend T operator!(const NPMatrix<T> &m) { return sqrt(m | m); }

    friend T operator/(const NPMatrix<T> &a, const NPMatrix<T> &b) { return !(a - b); }

    // COMPOUND OPERATORS

    inline NPMatrix<T> &operator+=(const NPMatrix<T> &m) { return add(m); }

    inline NPMatrix<T> &operator-=(const NPMatrix<T> &m) { return sub(m); }

    inline NPMatrix<T> &operator*=(const NPMatrix<T> &m) {
        matrixProduct(m);
        setDefaultBrowseIndices();
        m.setDefaultBrowseIndices();
        return *this;
    }

    inline NPMatrix<T> &operator*=(T s) override { return prod(s); }

    inline friend NVector<T> &operator*=(NVector<T> &u, const NPMatrix<T> &m) { return m.vectorProduct(u); }

    inline NPMatrix<T> &operator/=(T s) override { return div(s); }

    inline NPMatrix<T> &operator^=(long exp) { return pow(exp); }

    inline friend NVector<T> &operator%=(NVector<T> &u, const NPMatrix<T> &m) { return m.solve(u); }

    /** @} */

    /**
     * @name Function Operator
     * @brief Access to components and sub-matrix
     * @details C++ function operator are here used to give intuitive access to the matrix.
     *
     * @{
     */

    /**
     * @brief Bi-dimensional access operator
     * @details Access operator equivalent to `[i][j]` on by dimensional arrays.
     * Operator can be used to read/write values.
     * @return component ij of matrix.
     * @{
     */
    inline T &operator()(ul_t i, ul_t j) {
        assert(isValidIndex(i, j));
        return (*this)[vectorIndex(i, j)];
    }

    inline T operator()(ul_t i, ul_t j) const {
        assert(isValidIndex(i, j));
        return (*this).at(vectorIndex(i, j));
    }
    /** @} */

    /**
     *
     * @param i1 start index of rows
     * @param j1 start index of cols
     * @param i2 end index \f$ i2 \gt i1 \f$ of rows
     * @param j2 end index \f$ j2 \gt j1 \f$ of cols
     * @brief Manipulate sub-matrix
     * @details This operator is similar to `NVector<T>::operator() (ul_t, ul_t)`. It allows operations on a restricted
     * range of the matrix.
     * @return a sub matrix  :
     *         \f[ \begin{bmatrix}
     *             A_{i_1j_1}   & ... & A_{i_2j_1} \\
     *             ...          & ... & ... \\
     *             A_{i_2j_1}   & ... & A_{i_2j_2} \\
     *             \end{bmatrix}
     *         \f]
     *
     */
    inline NPMatrix<T> operator()(ul_t i1, ul_t j1, ul_t i2, ul_t j2) const { return subMatrix(i1, j1, i2, j2); }

    /**
     *
     * @param i1 start index of rows
     * @param j1 start index of cols
     * @param i2 end index \f$ i2 \gt i1 \f$ of rows
     * @param j2 end index \f$ j2 \gt j1 \f$ of cols
     * @brief Manipulate sub-matrix
     * @details This operator is similar to previous `operator()` except that it modify browse indices `_k1` and `_k2`
     * in order to modify efficiently non `const` lhs using this operator.
     * @return reference to `*this`.
     */
    NPMatrix<T> &operator()(ul_t i1, ul_t j1, ul_t i2, ul_t j2);

    /** @} */

    // AFFECTATION

    inline NPMatrix<T> &operator=(const NPMatrix<T> &m) {
        return copy(m);
    }

    // COMPARAISON OPERATORS

    friend bool operator==(const NPMatrix<T> &a, const NPMatrix<T> &b) {
        bool res = a(a._i1, a._j1, a._i2, a._j2).isEqual(b(b._i1, b._j1, b._i2, b._j2));

        a.setDefaultBrowseIndices();
        b.setDefaultBrowseIndices();
        return res;
    }

    inline friend bool operator!=(const NPMatrix<T> &a, const NPMatrix<T> &b) { return !(a == b); }

    // STATIC FUNCTIONS

    /**
     *
     * @brief \f$ n \times p \f$ matrix filled with `0`.
     */
    inline static NPMatrix<T> zeros(ul_t n, ul_t p = 0) {
        return NPMatrix<T>(NVector<T>::zeros(n * pIfNotNull(n, p)), n);
    }

    /**
     *
     * @brief \f$ n \times p \f$ matrix filled with `1`.
     */
    inline static NPMatrix<T> ones(ul_t n, ul_t p = 0) {
        return NPMatrix<T>(NVector<T>::ones(n * pIfNotNull(n, p)), n);
    }

    /**
     * @param i Row where to put 1.
     * @param j Col where to put 1.
     * @brief \f$ n \times p \f$ canonical matrix
     * @details canonical matrices \f$ E_{ij} \f$ of \f$ M_{np}(K) \f$
     * which contains `1` in position \f$ ij \f$ and `0` elsewhere.
     * This matrix is eviqualent to \f$ \delta_{ij} \f$ symbol.
     */
    inline static NPMatrix<T> canonical(ul_t i, ul_t j, ul_t n, ul_t p = 0) {
        return NPMatrix<T>(NVector<T>::canonical(p * i + j, n * pIfNotNull(n, p)), n);
    }

    /**
     *
     * @param n Size of the matrix
     * @brief \f$ n^{th} \f$ order identity matrix
     */
    static NPMatrix<T> eye(ul_t n);

    /**
     *
     * @param data values of diagonal `[` \f$ d_0, d_1, ..., d_{(n-1)} \f$`]`.
     * @param n size of the matrix.
     * @brief diagonal \f$ n^{th} \f$ order diagonal matrix filled with data array.
     */
    static NPMatrix<T> diag(const std::vector<T> &data, ul_t n);


    /**
     * @param n size of the matrix
     * @param s scalar value
     * @brief a scalar \f$ n^{th} \f$ order matrix with `s` value
     * @details Scalar matrices are a diagonal matrix filled a unique value.
     */
    inline static NPMatrix<T> scalar(T s, ul_t n) { return s * NPMatrix<T>::eye(n); }

    /**
     *
     * @param data values of diagonals such as.
     *
     * @brief Fills a square matrix matrix by giving diagonal data
     * @details This method is a generalization of `diag()` method. The matrix looks like :
     *
     *         \f[ \begin{bmatrix}
     *             d_{(mid)0}   & d_{(mid+1)0}  & ... & d_{(2 mid)0} \\
     *             d_{(mid-1)0} & d_{(mid)1}    & ... & d_{(2 mid-1)1} \\
     *             ...          & ...           & ... & ...\\
     *             d_{(0)0}     & d_{(1)1}      & ... & d_{(mid)(n-1)} \\
     *             \end{bmatrix}
     *         \f]
     *
     *          The input data must be ordered such as `d[0]` has size equal to `1`, `d[1]` to `2`, ...,
     *          `d[middle]` to `n`, d[middle+1]` to `n-1`, ..., `d[end]` to `0`.
     *
     *          The data must be entered such as :
     *
     *          - `data[l]` is the values of coefficients of the l-th diagonal from the left/up.
     *          - `data[middle]` is the values of coefficients on the diagonal.
     *
     * @return a n-diagonal matrix filled with data bi-dimensional array.
     */
    static NPMatrix<T> ndiag(const std::vector<NVector<T> > &data);


    /**
     *
     * @param scalars array of scalars values to fill diagonals `[` \f$ s_0, s_1, ..., s_q \f$ `]`.
     * @param n size of the matrix.
     * @brief Generalization of scalar matrix with multiple diagonal
     * @details The behavior is a restriction of `ndiag()` method, the matrix is filled using diagonals containing
     * the same value all along. If `values.size() = 2`, the matrix is tri-diagonal.
     * Center diagonal is filled with s1 and the other diagonal are filled with s0.
     * @return  a n-scalar Matrix filled with given `scalars`.
     */
    static NPMatrix<T> nscalar(const std::vector<T> &scalars, ul_t n);

protected:

    explicit NPMatrix(const NVector<T> &u, ul_t n, ul_t p, ul_t i1 = 0, ul_t j1 = 0, ul_t i2 = 0, ul_t j2 = 0);

    // MANIPULATORS

    NPMatrix<T> &swap(Parts element, ul_t k1, ul_t k2);

    NPMatrix<T> &shift(Parts element, ul_t k, long iterations);

    // MAX/MIN

    ul_t maxAbsIndex(Parts element, ul_t k, ul_t r) const;

    // MAX / MIN

    inline ul_t maxAbsIndexRow(ul_t i, ul_t r = 0) const { return maxAbsIndex(Row, i, r); }

    inline ul_t maxAbsIndexCol(ul_t j, ul_t r = 0) const { return maxAbsIndex(Col, j, r); }

    // ALGEBRAICAL OPERATIONS

    NVector<T> &vectorProduct(NVector<T> &u) const;

    NPMatrix<T> &matrixProduct(const NPMatrix<T> &m);

    inline NPMatrix<T> &add(const NPMatrix<T> &m) { return forEach(m, [](T &x, const T &y) { x += y; }); }

    inline NPMatrix<T> &sub(const NPMatrix<T> &m) { return forEach(m, [](T &x, const T &y) { x -= y; }); }

    inline NPMatrix<T> &opp() override { return prod(-1); }

    inline NPMatrix<T> &prod(T s) override { return forEach(s, [](T &x, T s) { x *= s; }); }

    inline NPMatrix<T> &div(T s) override { return forEach(s, [](T &x, T s) { x /= s; }); }

    NPMatrix<T> &pow(long n);

    void rPow(long n);

    NPMatrix<T> &inv();

    NVector<T> &solve(NVector<T> &u) const;

    // LUP MANAGEMENT

    void lupCopy() const;

    void lupUpdate() const;

    void lupClear() const;

    // MUTABLE VARIABLES MANAGEMENT

    inline NPMatrix<T> &clean() const {
        setDefaultBrowseIndices();
        lupClear();
        return const_cast<NPMatrix<T> &>(*this);
    }

    inline NPMatrix<T> &cleanBoth(const NPMatrix<T> &m) const {
        setDefaultBrowseIndices();
        m.setDefaultBrowseIndices();
        lupClear();
        return const_cast<NPMatrix<T> &>(*this);
    }

    // CHARACTERIZATION

    inline bool isValidRowIndex(ul_t i) const { return i < _n; }

    inline bool isValidColIndex(ul_t j) const { return j < _p; }

    inline bool isValidIndex(ul_t i, ul_t j) const { return (isValidRowIndex(i) && isValidColIndex(j)); }

    inline bool isBetweenI12(ul_t i) const { return i >= _i1 && i <= _i2; }

    inline bool isBetweenJ12(ul_t j) const { return j >= _j1 && j <= _j2; }

    inline static ul_t pIfNotNull(ul_t n, ul_t p) { return p > 0 ? p : n; }

    inline bool matchSizeForProduct(const NVector<T> &u) const { return u.dim() - 1 == _j2 - _j1; }

    inline bool matchSizeForProduct(const NPMatrix<T> &m) const { return m._i2 - m._i1 == _j2 - _j1; }

    inline bool hasSameSize(const NPMatrix<T> &m) const {
        return m._i2 - m._i1 == _i2 - _i1 && m._j2 - m._j1 == _j2 - _j1;
    }

    // SUB MATRIX INDICES MANAGEMENT

    bool hasDefaultBrowseIndices() const override;

    void setDefaultBrowseIndices() const override;

    // AFFECTATION

    NPMatrix<T> &copy(const NPMatrix<T> &m);

    NPMatrix<T> &copy(const vector<vector<T>> &data);

    // INDEX GETTERS

    inline ul_t vectorIndex(ul_t i, ul_t j) const { return _p * i + j; }

    inline ul_t getRowFromVectorIndex(ul_t k) const {
        assert(k < _n * _p);
        return k / _p;
    }

    inline ul_t getColFromVectorIndex(ul_t k) const {
        assert(k < _n * _p);
        return k % _p;
    }

    // SUB-MATRICES

    NPMatrix<T> subMatrix(ul_t i1 = 0, ul_t j1 = MAX_SIZE,
                          ul_t i2 = 0, ul_t j2 = MAX_SIZE) const;

    NPMatrix<T> &setSubMatrix(const NPMatrix<T> &m);

    // MANIPULATORS

    NPMatrix<T> &forEach(const NPMatrix<T> &m, const std::function<void(T &, const T &)> &binary_op);

    NPMatrix<T> &forEach(T s, const function<void(T &, T)> &binary_op) override;

    // SIZE

    ul_t _n{};

    ul_t _p{};

    // SUB MATRICES INDICES INDICES

    mutable ul_t _i1{};

    mutable ul_t _j1{};

    mutable ul_t _i2{};

    mutable ul_t _j2{};

    // LU STORAGE

    /**
     * @brief Matrix \f$ A = LU \f$ where \f$ PA = LU \f$ = this.
     * @details `_a` points to the \f$ A \f$ NMatrix or to `nullptr` if the matrix don't have \f$ LU \f$ decomposition.
     */
    mutable NPMatrix<T> *_a;

    /**
     * @brief permutation vector \f$ P \f$ such as \f$ PA = LU \f$.
     * @details Represented as `unsigned long` array.
     */
    mutable std::vector<ul_t> *_perm{};
};


typedef NPMatrix<double> mat_t;
typedef NPMatrix<char> mat_char_t;
typedef NPMatrix<AESByte> mat_aes_t;
typedef NPMatrix<Pixel> mat_pix_t;

/** @} */

#endif //MATHTOOLKIT_NPMATRIX_H