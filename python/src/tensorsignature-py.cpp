#include "tensorsignature-py.hpp"

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

void bind_tensorsignature(py::module &m) {

  py::class_<exatn::numerics::TensorSignature>(m, "TensorSignature", "")
      .def(py::init<>())
      .def(py::init<std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<const std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<unsigned int>())
      .def("printIt", &exatn::numerics::TensorSignature::printIt, "")
      .def("getRank", &exatn::numerics::TensorSignature::getRank, "")
      .def("getDimSpaceId", &exatn::numerics::TensorSignature::getDimSpaceId,
           "")
      .def("getDimSubspaceId",
           &exatn::numerics::TensorSignature::getDimSubspaceId, "")
      .def("resetDimension", &exatn::numerics::TensorSignature::resetDimension,
           "")
      .def("deleteDimension",
           &exatn::numerics::TensorSignature::deleteDimension, "")
      .def("appendDimension",
           &exatn::numerics::TensorSignature::appendDimension, "")
      .def("getDimSpaceAttr",
           &exatn::numerics::TensorSignature::getDimSpaceAttr, "");
  
}

 