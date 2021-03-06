//
// Created by Sami Dahoux on 08/10/2018.
//

#include <NVector.h>
#include <gtest/gtest.h>

#define NVECTOR_SMALL_DIM_TEST 1000000
#define NVECTOR_ITERATIONS_TEST 1000

using namespace std;

class NVectorBenchTest : public ::testing::Test {

protected:
    void SetUp() override {
        _u = vec_t::scalar(3, NVECTOR_SMALL_DIM_TEST);
        _v = vec_t::scalar(6, NVECTOR_SMALL_DIM_TEST);
        _s = 8;
        _elapsed_time = 0.0;
    }

    void iterateTestVector(const std::function<void(vec_t &, const vec_t &)> &test, std::string op = "") {
        for (int k = 0; k < NVECTOR_ITERATIONS_TEST; ++k) {
            _t0 = clock();
            test(_u, _v);
            _t1 = clock();
            _elapsed_time += ((_t1 - _t0) / (double_t) CLOCKS_PER_SEC) / NVECTOR_ITERATIONS_TEST;
        }
        cout << op << " AVG ELAPSED TIME : " << _elapsed_time << "s" << endl;
    }

    void iterateTestScalar(const std::function<void(vec_t &, double_t)> &test, std::string op = "") {
        for (int k = 0; k < NVECTOR_ITERATIONS_TEST; ++k) {
            _t0 = clock();
            test(_u, _s);
            _t1 = clock();
            _elapsed_time += ((_t1 - _t0) / (double_t) CLOCKS_PER_SEC) / NVECTOR_ITERATIONS_TEST;
        }
        cout << op << " AVG ELAPSED TIME : " << _elapsed_time << "s" << endl;
    }

    clock_t _t0;
    clock_t _t1;
    double_t _elapsed_time;

    vec_t _u;
    vec_t _v;
    double_t _s;
};

TEST_F(NVectorBenchTest, Add) {
    iterateTestVector([](vec_t &u, const vec_t &v) { u += v; }, "+");
}

TEST_F(NVectorBenchTest, Sub) {
    iterateTestVector([](vec_t &u, const vec_t &v) { u -= v; }, "-");
}

TEST_F(NVectorBenchTest, Prod) {
    iterateTestScalar([](vec_t &u, double_t s) { u /= s; }, "*");
}

TEST_F(NVectorBenchTest, Div) {
    iterateTestScalar([](vec_t &u, double_t s) { u /= s; }, "/");
}

TEST_F(NVectorBenchTest, DotProduct) {
    iterateTestVector([](vec_t &u, const vec_t &v) { u | v; }, "|");
}