//
// Created by Sami Dahoux on 07/05/2018.
//

#ifndef MATHTOOLKIT_NODE_H
#define MATHTOOLKIT_NODE_H


#include <NVector.h>
#include "NPField.h"

class NOde {
public:
    static std::vector<NVector<double>> euler(const NVector<double> &u0, NPField &f,
                                              double t0 = 0,
                                              double t1 = 1,
                                              double h = 0.1);
};


#endif //MATHTOOLKIT_NODE_H
