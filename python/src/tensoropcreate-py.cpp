#include "tensoropcreate-py.hpp"

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

void bind_tensoropcreate(py::module &m) {

  py::class_<exatn::numerics::TensorOpCreate,
             std::shared_ptr<exatn::numerics::TensorOpCreate>,
             exatn::numerics::TensorOperation>(m, "TensorOpCreate", "")
      .def(py::init<>())
      .def("isSet", &exatn::numerics::TensorOpCreate::isSet, "")
      .def("createNew", &exatn::numerics::TensorOpCreate::createNew, "");

}

 