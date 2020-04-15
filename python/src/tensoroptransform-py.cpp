#include "tensoroptransform-py.hpp"

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
void bind_tensoroptransform(py::module &m) {

 py::class_<exatn::numerics::TensorOpTransform,
             std::shared_ptr<exatn::numerics::TensorOpTransform>,
             exatn::numerics::TensorOperation>(m, "TensorOpTransform", "")
      .def("isSet", &exatn::numerics::TensorOpTransform::isSet, "")
      .def("createNew", &exatn::numerics::TensorOpTransform::createNew, "");

}

 