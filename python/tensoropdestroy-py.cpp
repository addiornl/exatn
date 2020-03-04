#include "tensoropdestroy-py.hpp"

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

void bind_tensoropdestroy(py::module &m) {

  py::class_<exatn::numerics::TensorOpDestroy,
             std::shared_ptr<exatn::numerics::TensorOpDestroy>,
             exatn::numerics::TensorOperation>(m, "TensorOpDestroy", "")
      .def(py::init<>())
      .def("isSet", &exatn::numerics::TensorOpDestroy::isSet, "")
      .def("createNew", &exatn::numerics::TensorOpDestroy::createNew, "");
}

 