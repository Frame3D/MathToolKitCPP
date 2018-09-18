//
// Created by Sami Dahoux on 04/05/2018.
//

#include "NPMatrix.h"

using namespace std;

NPMatrix::NPMatrix(unsigned long n, unsigned long p) :
        ENVector(n * ((p > 0) ? p : n)), _n(n), _p((p > 0) ? p : n) {}

NPMatrix::NPMatrix(const ENVector &vector) :
        ENVector(vector), _n(1), _p(vector.dim()) {}

NPMatrix::NPMatrix(const ENVector &vector, unsigned long n, unsigned long p) :
        ENVector(vector), _n(n), _p((p > 0) ? p : n) {
    assert(vector.dim() == _n * _p);
}

NPMatrix::NPMatrix(const NPMatrix &matrix) :
        ENVector(matrix), _n(matrix._n), _p(matrix._p) {}

NPMatrix::NPMatrix(const vector< vector<double> >& data) :
        ENVector((data.size() * data[0].size())), _n(data.size()),  _p(data[0].size()) {

    for (int k = 1; k < data.size(); ++k) {
        assert(data[k].size() == data[0].size());
    }

    if(!(*this).empty()) {
        for (unsigned long i = 0; i < _n; ++i) {
            for (unsigned long j = 0; j < _p; ++j) {
                (*this)(i, j) = data[i][j];
            }
        }
    }
}

NPMatrix::NPMatrix(const vector<ENVector> &vectors) :
        ENVector((vectors.size() * vectors[0].dim())), _n(vectors.size()), _p(vectors[0].dim()) {

    for (int k = 1; k < vectors.size(); ++k) {
        assert(vectors[k].dim() == vectors[0].dim());
    }

    for (unsigned long i = 0; i < _n; ++i) {
        setRow(vectors[i], i);
    }
}


string NPMatrix::str() const {
    string str = "\n";
    char buffer[6];
    for (unsigned long i = 0; i < _n; ++i) {
        str.append("|");
        for (unsigned long j = 0; j < _p; ++j) {
            sprintf(buffer, "%.2e", abs((*this)(i, j)));
            str.append(((*this)(i, j) >= 0 ? "  " : " -"));
            str.append(buffer);
        }
        str.append("  |\n");
    }

    return str;
}


// CHARACTERIZATION


bool NPMatrix::isValidRowIndex(unsigned long i) const {
    return i < _n;
}

bool NPMatrix::isValidColIndex(unsigned long j) const {
    return j < _p;
}

bool NPMatrix::isValidIndex(unsigned long i, unsigned long j) const {
    return isValidRowIndex(i) && isValidColIndex(j);
}

bool NPMatrix::isSquare() const {
    return _n == _p;
}


// GETTERS


unsigned long NPMatrix::n() const {
    return _n;
}

unsigned long NPMatrix::p() const {
    return _p;
}

ENVector NPMatrix::row(unsigned long i) const {
    vector<double> data(_p);
    for (unsigned long k = 0; k < _p; ++k) {
        data[k] = (*this)[getVectorIndex(i, k)];
    }
    return ENVector(data);
}

ENVector NPMatrix::col(unsigned long j) const {
    vector<double> data(_n);
    for (unsigned long k = 0; k < _n; ++k) {
        data[k] = (*this)[getVectorIndex(k, j)];
    }
    return ENVector(data);
}

vector<ENVector> NPMatrix::rows(unsigned long i1, unsigned long i2) const {
    unsigned long start = (isValidRowIndex(i1)) ? i1 : 0;
    unsigned long end   = (isValidRowIndex(i2)) ? i2 : _n - 1;

    vector<ENVector> rows(end - start + 1);
    for (unsigned long i = start; i <= end; ++i) {
        rows[i - start] = row(i);
    }
    return rows;
}

vector<ENVector> NPMatrix::cols(unsigned long j1, unsigned long j2) const {
    unsigned long start = (isValidRowIndex(j1)) ? j1 : 0;
    unsigned long end   = (isValidRowIndex(j2)) ? j2 : _p - 1;

    vector<ENVector> cols(end - start + 1);
    for (unsigned long j = start; j <= end; ++j) {
        cols[j - start] = col(j);
    }
    return cols;
}

NPMatrix NPMatrix::subMatrix(unsigned long i1, unsigned long j1, unsigned long i2, unsigned long j2) {
    unsigned long n = i2 - i1 + 1;
    unsigned long p = j2 - j1 + 1;

    NPMatrix subMatrix = NPMatrix::zeros(n, p);
    for (unsigned long i = 0; i < n; ++i) {
        for (unsigned long j = 0; j < p; ++j) {
            subMatrix(i, j) = (*this)(i, j);
        }
    }
    return subMatrix;
}


// ROWS/COLS/SUB SETTERS


void NPMatrix::setRow(const ENVector &vector, unsigned long i1) {
    if(vector.dim() == _p && isValidRowIndex(i1)) {
        for (unsigned long j = 0; j < vector.dim(); ++j) {
            (*this)(i1, j) = vector(j);
        }
    }
}

void NPMatrix::setCol(const ENVector &vector, unsigned long j1) {
    if(vector.dim() == _n && isValidColIndex(j1)) {
        for (unsigned long i = 0; i < vector.dim(); ++i) {
            (*this)(i, j1) = vector(i);
        }
    }
}

void NPMatrix::setRows(const vector<ENVector> &vectors, unsigned long i1) {
    if(vectors.size() + i1 < _n) {
        for (unsigned long i = i1; i < i1 + vectors.size(); ++i) {
            setRow(vectors[i - i1], i);
        }
    }
}

void NPMatrix::setCols(const vector<ENVector> &vectors, unsigned long j1) {
    if(vectors.size() + j1 < _p) {
        for (unsigned long j = j1; j < j1 + vectors.size(); ++j) {
            setCol(vectors[j - j1], j);
        }
    }
}

void NPMatrix::setSubMatrix(const NPMatrix &matrix, unsigned long i1, unsigned long j1) {
    unsigned long n = matrix._n - i1 + 1;
    unsigned long p = matrix._p - j1 + 1;

    if(n <= _n && p <= _p) {
        for (unsigned long i = 0; i < n; ++i) {
            for (unsigned long j = 0; j < p; ++j) {
                (*this)(i + i1, j + j1) = matrix(i, j);
            }
        }
    }
}


// SWAPS


void NPMatrix::swap(unsigned long i1, unsigned long j1, unsigned long i2, unsigned long j2) {
    NVector::swap(getVectorIndex(i1, j1), getVectorIndex(i2, j2));
}

void NPMatrix::swapRow(unsigned long i1, unsigned long i2) {
    swap(Row, i1, i2);
}

void NPMatrix::swapCol(unsigned long j1, unsigned long j2) {
    swap(Col, j1, j2);
}


// SHIFTS


void NPMatrix::shiftRow(unsigned long i, const long iterations) {
    shift(Row, i, iterations);
}

void NPMatrix::shiftCol(unsigned long j, const long iterations) {
    shift(Col, j, iterations);
}


//MAX / MIN


unsigned long NPMatrix::maxAbsIndexRow(unsigned long i, unsigned long r) const {
    return maxAbsIndex(Row, i, r);
}

unsigned long NPMatrix::maxAbsIndexCol(unsigned long j, unsigned long r) const {
    return maxAbsIndex(Col, j, r);
}


// OPERATORS


NPMatrix operator+(const NPMatrix &m1, const NPMatrix &m2) {
    NPMatrix res{m1};
    if(m1._n == m2._n && m1._p == m2._p)
        res.add(m2);
    return res;
}

NPMatrix operator-(const NPMatrix &m1, const NPMatrix &m2) {
    NPMatrix res{m1};
    if(m1._n == m2._n && m1._p == m2._p)
        res.sub(m2);
    return res;
}

NPMatrix operator*(double s, const NPMatrix &m) {
    NPMatrix res{m};
    res.prod(s);
    return res;
}

NPMatrix operator*(const NPMatrix &m, double s) {
    NPMatrix res{m};
    res.prod(s);
    return res;
}


NPMatrix operator*(const NPMatrix &m1, const NPMatrix &m2) {
    NPMatrix res{m1};
    res.matrixProduct(m2);
    return res;
}

ENVector operator*(const NPMatrix &m, const ENVector &v) {
    NVector res{v};
    m.vectorProduct(res);
    return res;
}


NPMatrix operator|(const NPMatrix &m1, const NPMatrix &m2) {
    return m1.shifted(m2);
}


NPMatrix operator/(const NPMatrix &m, double s) {
    NPMatrix res{m};
    res.div(s);
    return res;
}

NPMatrix operator-(const NPMatrix &m) {
    NPMatrix res{m};
    res.opp();
    return res;
}

NPMatrix operator!(const NPMatrix &m) {
    NPMatrix res{m};
    res.transpose();
    return res;
}


// COMPOUND OPERATORS


NPMatrix & NPMatrix::operator*=(const NPMatrix &matrix) {
    matrixProduct(matrix);
    return *this;
}


NPMatrix &NPMatrix::operator*=(const double scalar) {
    prod(scalar);
    return (*this);
}


// BI-DIMENSIONAL ACCESSORS


double &NPMatrix::operator()(unsigned long i, unsigned long j) {
    unsigned long k = getVectorIndex(i, j);
    if(isValidIndex(i, j))
        return (*this)[k];
    else {
        auto* nanValue = new double; *nanValue = nan("");
        return *nanValue;
    }
}

double NPMatrix::operator()(unsigned long i, unsigned long j) const {
    unsigned long k = getVectorIndex(i, j);
    if(isValidIndex(i, j))
        return (*this).at((unsigned long) k);
    else
        return nan("");
}


// ALGEBRA

void NPMatrix::reduce() {
    const double epsilon = numeric_limits<double>::epsilon();

    unsigned long r = 0, k, i, j;
    ENVector spin;
    for(j = 0; j < floor(_p / 2); ++j) {

        k = maxAbsIndexCol(j, r);
        if( abs((*this)(k, j)) > epsilon ) {
            setRow( row(k) / (*this)(k, j), k );
            swapRow(k, r);

            spin = row(r);
            for(i = 0; i < _n; ++i) {
                if(i != r)
                    setRow( row(i) - (*this)(i, j) * spin, i );
            }
            r++;
        }

    }
}

// STATIC FUNCTIONS


NPMatrix NPMatrix::zeros(unsigned long n, unsigned long p) {
    p = (p > -1) ? p : n;
    return NPMatrix(NVector::zeros(n * p), n, p);
}

NPMatrix NPMatrix::ones(unsigned long n, unsigned long p) {
    p = (p > -1) ? p : n;
    return NPMatrix(NVector::ones(n * p), n, p);
}

NPMatrix NPMatrix::canonical(unsigned long i, unsigned long j, unsigned long n, unsigned long p) {
    p = (p >= -1) ? p : n;
    return NPMatrix(NVector::canonical(p * i + j, n * p), n, p);
}

// PRIVATE METHODS


unsigned long NPMatrix::getVectorIndex(unsigned long i, unsigned long j) const {
    return _p * i + j;
}

unsigned long NPMatrix::getRowFromVectorIndex(unsigned long k) const {
    return k / _p;
}

unsigned long NPMatrix::getColFromVectorIndex(unsigned long k) const {
    return k % _p;
}


void NPMatrix::swap(const ElementEnum element, unsigned long k1, unsigned long k2) {
    NVector temp = (element == Row) ? NPMatrix::row(k1) : col(k1);

    (element == Row) ? setRow(NPMatrix::row(k2), k1) : setCol(NPMatrix::col(k2), k1);
    (element == Row) ? setRow(temp, k2)              : setCol(temp, k2);
}

void NPMatrix::shift(const ElementEnum element, unsigned long k, const long iterations) {
    NVector vector = (element == Row) ? NPMatrix::row(k) : col(k);
    vector.shift(iterations);
    (element == Row) ? setRow(vector, k) : setCol(vector, k);
}

unsigned long NPMatrix::maxAbsIndex(const ElementEnum element, unsigned long k, unsigned long r) const {
    ENVector vector = (element == Row) ? NPMatrix::row(k) : col(k);
    vector = vector(r, (long) ((element == Row) ? _p : _n) - 1);
    return r + vector.maxAbsIndex();
}


void NPMatrix::transpose() {
    NPMatrix temp = NPMatrix::zeros(_p, _n);
    for(unsigned long i = 0; i < _n; i++) {
        for(unsigned long j = 0; j < _p; j++) {
            temp(j, i) = (*this)(i, j);
        }
    }
    (*this) = temp;
}

void NPMatrix::vectorProduct(NVector &vector) const {
    if(vector.dim() == _p) {
        std::vector<ENVector> vectorRows = rows();

        NVector res = NVector::zeros(vector.dim());
        for (unsigned long i = 0; i < _n; ++i) {
            res(i) = vectorRows[i] * vector;
        }
        vector = res;
    }
}

void NPMatrix::matrixProduct(const NPMatrix &matrix) {
    if(_p == matrix._n) {
        vector<ENVector> vectorRows = rows();
        vector<ENVector> vectorCols = matrix.cols();

        NPMatrix res = NPMatrix::zeros(_n, matrix._p);
        for (unsigned long i = 0; i < _n; ++i) {
            for (unsigned long j = 0; j < matrix._p; ++j) {
                res(i, j) = vectorRows[i] * vectorCols[j];
            }
        }
        (*this) = res;

    }
}

NPMatrix NPMatrix::shifted(const NPMatrix& matrix) const {
    NPMatrix shifted = NPMatrix::zeros(_n, matrix._p + _n);
    for(unsigned long i = 0; i < _n; i++) {
        for(unsigned long j = 0; j < _p; j++) {
            shifted(i, j) = (*this)(i, j);
        }

        for(unsigned long j = _p; j < matrix._p + _p; j++) {
            shifted(i, j) = matrix(i, j - _p);
        }
    }
    return shifted;
}




























