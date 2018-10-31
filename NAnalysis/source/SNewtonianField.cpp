//
// Created by Sami Dahoux on 08/05/2018.
//

#include <SNewtonianField.h>

SNewtonianField::SNewtonianField(vec_t h, const std::vector<double> &mu, const std::vector<vec_t> &r, double k) :
        NPField<double>(h), _mu(mu), _r(r), _k(k) {}

vec_t SNewtonianField::apply(const vec_t &r) const {
    vec_t g = vec_t::zeros(h.dim());
    vec_t rj = vec_t::zeros(h.dim());
    double d;

    for (int j = 0; j < _mu.size(); j++) {
        if ((d = (r / _r[j])) > std::numeric_limits<double>::epsilon()) {
            g += _k * _mu[j] * (r - _r[j]) / pow(d, 3);
        }
    }
    return g;
}


