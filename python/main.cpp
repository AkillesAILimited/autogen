#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "autogen/autogen.hpp"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;
using CppADScalar = typename CppAD::AD<double>;
using ADVector = std::vector<CppADScalar>;

PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(ADVector);

PYBIND11_MODULE(autogen_python, m) {
  m.doc() = R"pbdoc(
        Autogen python plugin
        -----------------------

        .. currentmodule:: autogen_python

        .. autosummary::
           :toctree: _generate
    )pbdoc";

  py::bind_vector<std::vector<double>>(m, "DoubleVector");
  py::bind_vector<ADVector>(m, "ADVector");

  py::class_<CppADScalar>(m, "CppADScalar")
      .def(py::init([](double t) {
        //            return std::shared_ptr<CppADScalar>(new CppADScalar(t));
        return new CppADScalar(t);
      }))
      .def(py::init())
      .def(-py::self)
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self * py::self)
      .def(py::self / py::self)
      .def(py::self + float())
      .def(py::self - float())
      .def(py::self * float())
      .def(py::self / float())
      .def(float() + py::self)
      .def(float() - py::self)
      .def(float() * py::self)
      .def(float() / py::self)
      .def(py::self += py::self)
      .def(py::self -= py::self)
      .def(py::self *= py::self)
      .def(py::self /= py::self)
      .def(py::self == py::self)
      .def(py::self != py::self)
      //      .def("__pow__",
      //           [](CppADScalar* s, int exponent) {
      //             return &CppAD::pow(*s,exponent); })
      .def("abs", &CppADScalar::abs_me)
      .def("acos", &CppADScalar::acos_me)
      .def("asin", &CppADScalar::asin_me)
      .def("atan", &CppADScalar::atan_me)
      .def("cos", &CppADScalar::cos_me)
      .def("cosh", &CppADScalar::cosh_me)
      .def("exp", &CppADScalar::exp_me)
      .def("fabs", &CppADScalar::fabs_me)
      .def("log", &CppADScalar::log_me)
      .def("sin", &CppADScalar::sin_me)
      .def("sign", &CppADScalar::sign_me)
      .def("sinh", &CppADScalar::sinh_me)
      .def("sqrt", &CppADScalar::sqrt_me)
      .def("tan", &CppADScalar::tan_me)
      .def("tanh", &CppADScalar::tanh_me)
      .def("asinh", &CppADScalar::asinh_me)
      .def("acosh", &CppADScalar::acosh_me)
      .def("atanh", &CppADScalar::atanh_me)
      .def("erf", &CppADScalar::erf_me)
      .def("expm1", &CppADScalar::expm1_me)
      .def("log1p", &CppADScalar::log1p_me)
      //      .def("__repr__",
      //           [](CppADScalar* s) { return std::to_string(CppAD::Value(*s));
      //           });
      .def("__repr__",
           [](CppADScalar s) { return std::to_string(CppAD::Value(s)); });

  //  py::class_<CppAD::ADFun<double>>(m, "ADFun")
  //      .def(py::init(
  //          [](std::vector<CppADScalar*> x, std::vector<CppADScalar*> y) {
  //            std::vector<CppADScalar> vx;
  //            for (CppADScalar* s : x) {
  //              vx.push_back(*s);
  //            }
  //            std::vector<CppADScalar> vy;
  //            for (CppADScalar* s : y) {
  //              vy.push_back(*s);
  //            }
  //            return new CppAD::ADFun<double>(vx, vy);
  //          }))
  //      .def(
  //          "forward",
  //          [](CppAD::ADFun<double>* f, size_t q, std::vector<double> xq) {
  //            return f->Forward(q, xq);
  //          },
  //          py::return_value_policy::reference)
  //      .def(
  //          "jacobian",
  //          [](CppAD::ADFun<double>* f, std::vector<double> x) {
  //            return f->Jacobian(x);
  //          },
  //          py::return_value_policy::reference);
  //
  //  // doesn't work b/c we should change the existing vector
//    m.def("independent", [](std::vector<CppADScalar*> x) {
//      std::vector<CppADScalar> v;
//      for (CppADScalar* s : x) {
//        v.push_back(*s);
//      }
//      CppAD::Independent(v);
//    });

  py::class_<CppAD::ADFun<double>, std::unique_ptr<CppAD::ADFun<double>>>(
      m, "ADFun")
      .def(py::init<ADVector, ADVector>(),
           py::return_value_policy::reference_internal)
      .def(
          "forward",
          [](CppAD::ADFun<double>& f, size_t q, std::vector<double> xq) {
            return f.Forward(q, xq);
          },
          py::return_value_policy::reference)
      .def(
          "jacobian",
          [](CppAD::ADFun<double>& f, std::vector<double> x) {
            return f.Jacobian(x);
          },
          py::return_value_policy::reference);

  m.def("independent", [](ADVector& x) { CppAD::Independent(x); });

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}