//
// Created by Sami Dahoux on 07/05/2018.
//

#include "../header/NConstantField.h"

NConstantField::NConstantField(NCompact* domain, const ENVector& cst) : NPField(domain, cst.dim()), _cst(cst) {

}

ENVector NConstantField::g(const ENVector &x) {
    return _cst;
}

