#include "tensoropcontract-py.hpp"

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

void bind_tensoropcontract(py::module &m) {

  py::class_<exatn::numerics::TensorOpContract,
             std::shared_ptr<exatn::numerics::TensorOpContract>,
             exatn::numerics::TensorOperation>(m, "TensorOpContract", "")
      .def(py::init<>())
      .def("isSet", &exatn::numerics::TensorOpContract::isSet, "")
      .def("createNew", &exatn::numerics::TensorOpContract::createNew, "");

}

 