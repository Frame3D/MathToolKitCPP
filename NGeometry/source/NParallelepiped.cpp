//
// Created by Sami Dahoux on 06/05/2018.
//

#include "../header/NParallelepiped.h"
#include "../header/NSegment.h"

NParallelepiped::NParallelepiped(NMatrix &base, const ENVector& pos) : NCompact(base.n()),
                                                                             _base(NMatrix(base)),
                                                                             _pos(pos),
                                                                             _vol(base.det())
{

}

std::string NParallelepiped::str() const {
    const std::vector<NSegment> segs = segments();

    std::string str = "NParallelepiped";
    char buffer[2];
    for (int k = 0; k < segs.size(); ++k) {
        sprintf(buffer, "%d", k);
        str += "\nsegment " + std::string(buffer) + " : " + segs[k].str();
    }
    return str;
}


bool NParallelepiped::isIn(const ENVector &x) const {
    ENVector u = x - _pos;
    NMatrix pInv = _base ^- 1;
    u = pInv * u;
    for (int k = 0; k < _dim; ++k) {
        if(u(k) > 1.0 ||  u(k) < 0.0) {
            return false;
        }
    }
    return true;
}

bool NParallelepiped::isEmpty() const {
    return abs(_vol) > std::numeric_limits<double>::epsilon();
}

void NParallelepiped::uni(const NSet* set) {

}

void NParallelepiped::inter(const NSet* set) {

}

int NParallelepiped::card() const {
    return 0;
}

NCompact *NParallelepiped::border() const {

    return nullptr;
}


std::vector<ENVector> NParallelepiped::mesh(const ENVector &h) const {
    const std::vector<ENVector> e = _base.cols();
    const ENVector end = ENVector::sum(e);
    const double tol = h.maxAbs();
    double d = std::numeric_limits<double>::infinity();

    std::vector<ENVector> mesh{};
    ENVector x = NVector::zeros(_dim);

    while( x / end >= tol) {
        mesh.push_back(x + _pos);
        if(x * e[0] <= e[0] * e[0]) {
            x += h(0) * e[0];
        }
        for (int i = 0; i < _dim; ++i) {
            if(x * e[i] > e[i] * e[i]) {
                for (int j = 0; j <= i; ++j) {
                    x -= x * e[j] / (e[j] * e[j]) * e[j];
                }
                if(i < _dim - 1) {
                    x += h(i + 1) * e[i + 1];
                }
            }
        }
    }
    mesh.push_back(x + _pos);
    return mesh;
}


std::vector<NSegment> NParallelepiped::segments() const {
    const std::vector<ENVector> e = _base.cols();
    std::vector<NSegment> seg;
    for (int k = 0; k < _dim; ++k) {
        seg.emplace_back(NSegment(_pos, _pos + e[k]));
    }
    return seg;
}

double NParallelepiped::volume() const {
    return _vol;
}



