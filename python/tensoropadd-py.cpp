#include "tensoropadd-py.hpp"

#include "pybind11/pybind11.h"
#include "talshxx.hpp"
#include "tensor_basic.hpp"
#include "exatn_numerics.hpp"

namespace py = pybind11;
using namespace exatn;
using namespace exatn::numerics;
using namespace pybind11::literals;

/**

 */

void bind_tensoropadd(py::module &m) {

  py::class_<exatn::numerics::TensorOpAdd,
             std::shared_ptr<exatn::numerics::TensorOpAdd>,
             exatn::numerics::TensorOperation>(m, "TensorOpAdd", "")
      .def(py::init<>())
      .def("isSet", &exatn::numerics::TensorOpAdd::isSet, "")
      .def("createNew", &exatn::numerics::TensorOpAdd::createNew, "");
}

 