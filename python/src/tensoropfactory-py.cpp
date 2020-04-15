#include "tensoropfactory-py.hpp"

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

void bind_tensoropfactory(py::module &m) {

  py::class_<exatn::numerics::TensorOpFactory>(m, "TensorOpFactory", "")
      .def("registerTensorOp",
           &exatn::numerics::TensorOpFactory::registerTensorOp, "")
      .def("get", &exatn::numerics::TensorOpFactory::get,
           py::return_value_policy::reference, "")
      .def("createTensorOpShared",
           &exatn::numerics::TensorOpFactory::createTensorOpShared, "");
}

 