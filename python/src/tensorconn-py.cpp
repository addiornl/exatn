#include "tensorconn-py.hpp"

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

void bind_tensorconn(py::module &m) {

    py::class_<exatn::numerics::TensorConn>(m, "TensorConn", "")
      .def(py::init<std::shared_ptr<exatn::numerics::Tensor>, unsigned int,
                    const std::vector<exatn::numerics::TensorLeg>>())
      .def("printIt", &exatn::numerics::TensorConn::printIt, "")
      .def("getNumLegs", &exatn::numerics::TensorConn::getNumLegs, "")
      .def("getTensorId", &exatn::numerics::TensorConn::getTensorId, "")
      .def("getTensor", &exatn::numerics::TensorConn::getTensor, "")
      .def("getTensorLeg", &exatn::numerics::TensorConn::getTensorLeg, "")
      .def("getTensorLegs", &exatn::numerics::TensorConn::getTensorLegs, "")
      .def("getDimExtent", &exatn::numerics::TensorConn::getDimExtent, "")
      .def("getDimSpaceAttr", &exatn::numerics::TensorConn::getDimSpaceAttr, "")
      .def("resetLeg", &exatn::numerics::TensorConn::resetLeg, "")
      .def("deleteLeg", &exatn::numerics::TensorConn::deleteLeg, "")
      .def("deleteLegs", &exatn::numerics::TensorConn::deleteLegs, "")
      .def("appendLeg",
           (void (exatn::numerics::TensorConn::*)(
               std::pair<SpaceId, SubspaceId>, DimExtent,
               exatn::numerics::TensorLeg)) &
               exatn::numerics::TensorConn::appendLeg,
           "")
      .def("appendLeg",
           (void (exatn::numerics::TensorConn::*)(DimExtent,
                                                  exatn::numerics::TensorLeg)) &
               exatn::numerics::TensorConn::appendLeg,
           "");
  
}

 