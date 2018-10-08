# MathToolKitCPP

### Author : samiBendou

### Last Update : 08/10/2018

### Current Version : 0.5

MathToolKit is C++ maths framework. It provides libraries that ease N-dimensional calculation.

- Linear algebra library (NAlgebra)

- Differential equations solving library (NAnalysis)

- Geometry library (NGeometry)

- Designed to ease dynamic system simulation

- High level syntax and functionality

- Complete integration of STL

- Highly serializable as string or std::vector

- Unit tested with googletest

The framework can be used to produce C++ code for various domains such as applied maths, physics, economics.

## NAlgebra

#### Overview

- Classic linear algebra objects such as euclidean vectors or dense matrices (tensors coming soon).

- Very large range of operations such as +, *, linear mapping, matrix inversions... look at TestNAlgebra directory to have an overview.

##### NVector

N-dimensional euclidean vectors :
    - Algebraical operations (E, +, *)
    - Swaps, shifts, ...
    - Classic vectors generator such as ones, zeros...
  
##### NPMatrix

NxP dense matrices :
    - Algebraical operations such as matrix product, linear map, gauss jordan elimination. 
    - Setters (row, col) & getters (row, col), swappers, shift. 
    - Classic matrix generators such as ones, zeros...
  
##### NMatrix

Optimized NxN dense matrices :
    - Inversion, determinant and solve using LU decomposition.
    - Trace, power, ...
    - Classic matrix generators such as eye, diag, scalar...
  
##### Vector3

3D Space vectors. Featuring cross product, coordinate transforms and rotations.
  
 ### NAnalysis
 
 #### Overview
 
 Representing numerical vector fields in finite dimension vector space. WIP.
 
 ### NGeometry
 
 #### Overview
 
 Representing compact domains in N-dimension and mesh theses. WIP.
 
 ### Compatibility
 
 The framework is written in C++11. It's recommended to use CMake and g++ to compile sources. I had some issues with Cygwin and Mingwin... Project has been coded using CLion, it provides easy to handle unit testing with googletest and stock build configuration.
 
 ### Install
 
 - Make sure you have CMake version 3.12 at least;
 - Clone googletest : https://github.com/google/googletest;
 - Clone the project;
 - Put the googletest library at TestMathToolKit/lib/googletest where googletest is the root of the previously downloaded googletest;
 - Compile using CMake.
