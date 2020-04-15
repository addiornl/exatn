#include "tensorleg-py.hpp"

#include "tensor.hpp"
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
void bind_tensorleg(py::module &m) {
      py::class_<exatn::numerics::TensorLeg,
             std::shared_ptr<exatn::numerics::TensorLeg>>(m, "TensorLeg", "")
      // Specify the default LegDirection argument for this constructor
      .def(py::init<unsigned int, unsigned int, exatn::LegDirection>(),
           "tensor_id"_a, "dimensn_id"_a,
           "direction"_a = exatn::LegDirection::UNDIRECT)
      .def(py::init<const TensorLeg>())
      .def("printIt", &exatn::numerics::TensorLeg::printIt, "")
      .def("getTensorId", &exatn::numerics::TensorLeg::getTensorId, "")
      .def("getDimensionId", &exatn::numerics::TensorLeg::getDimensionId, "")
      .def("getDirection", &exatn::numerics::TensorLeg::getDirection, "")
      .def("resetConnection", &exatn::numerics::TensorLeg::resetConnection, "")
      .def("resetTensorId", &exatn::numerics::TensorLeg::resetTensorId, "")
      .def("resetDimensionId", &exatn::numerics::TensorLeg::resetDimensionId,
           "")
      .def("resetDirection", &exatn::numerics::TensorLeg::resetDirection, "");

}

 