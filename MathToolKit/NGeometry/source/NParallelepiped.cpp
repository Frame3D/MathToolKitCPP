//
// Created by Sami Dahoux on 06/05/2018.
//

#include <NParallelepiped.h>

#include "NParallelepiped.h"
#include "NSegment.h"

NParallelepiped::NParallelepiped(const mat_t &base, const vec_t &pos) : NCompact(base.n()),
                                                                        _base(base),
                                                                        _pos(pos),
                                                                        _vol(base.det()) {}

std::string NParallelepiped::str() const {
    const std::vector<NSegment> seg = segments();

    std::stringstream stream;
    for (int k = 0; k < seg.size(); ++k) {
        stream << "\nsegment " << k << " : " << seg[k];
    }
    return stream.str();
}


bool NParallelepiped::isIn(const vec_t &x) const {
    vec_t u = x - _pos;
    mat_t base_inv = _base ^-1;
    u = base_inv * u;
    for (int k = 0; k < _dim; ++k) {
        if (u(k) > 1 || u(k) < 0) {
            return false;
        }
    }
    return true;
}

bool NParallelepiped::isEmpty() const {
    return abs(_vol) < D_EPSILON;
}

int NParallelepiped::card() const {
    return std::numeric_limits<int>::infinity();
}

const mat_t &NParallelepiped::base() const {
    return _base;
}

const vec_t &NParallelepiped::pos() const {
    return _pos;
}

double_t NParallelepiped::vol() const {
    return _vol;
}


void NParallelepiped::setBase(const mat_t &base) {
    NParallelepiped::_base = base;
    _vol = base.det();
}

void NParallelepiped::setPos(const vec_t &pos) {
    NParallelepiped::_pos = pos;
}

vector<vec_t> NParallelepiped::border() const {
    //TODO : Implement parallelepiped triangulation algorithm
    return vector<vec_t>();
}

std::vector<vec_t> NParallelepiped::mesh(const vec_t &h) const {
    std::vector<vec_t> e = _base.rows();
    vec_t end = vec_t::sum(e);

    size_t mesh_size = meshSize(h);

    std::vector<vec_t> mesh{};
    vec_t x = vec_t::zeros(_dim);

    for (int k = 0; k < mesh_size; ++k) {
        mesh.push_back(_base * x + _pos);
        if (x(0) <= 1) {
            x(0) += h(0);
        }
        for (size_t i = 0; i < _dim; ++i) {
            if (x(i) > 1) {
                for (size_t j = 0; j <= i; ++j) {
                    x(j) = 0;
                }
                if (i < _dim - 1) {
                    x(i + 1) += h(i + 1);
                }
            }
        }
    }
    return mesh;
}


std::vector<NSegment> NParallelepiped::segments() const {
    const std::vector<vec_t> e = _base.cols();
    std::vector<NSegment> seg(_dim);
    for (int k = 0; k < _dim; ++k) {
        seg[k] = NSegment(_pos, _pos + e[k]);
    }
    return seg;
}


size_t NParallelepiped::meshSize(const vec_t &h) const {
    size_t res = 1;
    for (int k = 0; k < _base.p(); ++k) {
        res *= (size_t) ceil(1 / abs(h(k)));
    }
    return res;
}








