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

PYBIND11_MAKE_OPAQUE(ADVector);

PYBIND11_MODULE(_autogen, m) {
  m.doc() = R"pbdoc(
        Autogen python plugin
        -----------------------

        .. currentmodule:: autogen

        .. autosummary::
           :toctree: _generate
    )pbdoc";

  py::bind_vector<ADVector>(m, "Vector");

  py::class_<CppADScalar>(m, "Scalar")
      .def(py::init([](double t) { return CppADScalar(t); }))
      .def(py::init(
          [](const CppADScalar& scalar) { return CppADScalar(scalar); }))
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
      .def("__pow__",
           [](const CppADScalar& s, const CppADScalar& exponent) {
             return CppAD::pow(s, exponent);
           })
      .def("__pow__", [](const CppADScalar& s,
                         double exponent) { return CppAD::pow(s, exponent); })
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
      .def("__repr__", [](const CppADScalar& s) {
        return "<" + std::to_string(CppAD::Value(s)) + ">";
      });

  py::class_<CppAD::ADFun<double>>(m, "Function")
      .def(py::init([](const ADVector& x, const ADVector& y) {
        return CppAD::ADFun<double>(x, y);
      }))
      .def(
          "forward",
          [](CppAD::ADFun<double>& f, const std::vector<double>& xq, size_t q) {
            return f.Forward(q, xq);
          },
          "Evaluates the forward pass of the function", py::arg("x"),
          py::arg("order") = 0)
      .def(
          "jacobian",
          [](CppAD::ADFun<double>& f, const std::vector<double>& x) {
            return f.Jacobian(x);
          },
          "Evaluates the Jacobian of the function")
      .def("to_json", &CppAD::ADFun<double>::to_json,
           "Represents the traced function by a JSON string");

  m.def("independent", [](ADVector& x) { CppAD::Independent(x); });

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}