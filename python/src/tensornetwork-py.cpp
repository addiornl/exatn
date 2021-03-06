#include "tensornetwork-py.hpp"

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

void bind_tensornetwork(py::module &m) {

  py::class_<exatn::numerics::TensorNetwork,
             std::shared_ptr<exatn::numerics::TensorNetwork>>(
      m, "TensorNetwork", "")
      .def(py::init<>())
      .def(py::init<const std::string>())
      .def(py::init<const std::string, std::shared_ptr<exatn::numerics::Tensor>,
                    const std::vector<exatn::numerics::TensorLeg>>())
      .def(py::init<const std::string, const std::string,
                    const std::map<std::string,
                                   std::shared_ptr<exatn::numerics::Tensor>>>())
      .def("printIt", &exatn::numerics::TensorNetwork::printIt, "")
      .def("getName", &exatn::numerics::TensorNetwork::getName, "")
      .def("isEmpty", &exatn::numerics::TensorNetwork::isEmpty, "")
      .def("isExplicit", &exatn::numerics::TensorNetwork::isExplicit, "")
      .def("finalize", &exatn::numerics::TensorNetwork::finalize,
           "check_validity"_a = false, "")
      .def("isFinalized", &exatn::numerics::TensorNetwork::isFinalized, "")
      .def("getNumTensors", &exatn::numerics::TensorNetwork::getNumTensors, "")
      .def("getName", &exatn::numerics::TensorNetwork::getName, "")
      .def("getTensor", &exatn::numerics::TensorNetwork::getTensor, "")
      .def("placeTensor",
           (bool (exatn::numerics::TensorNetwork::*)(
               unsigned int,
               std::shared_ptr<exatn::numerics::Tensor>,
               const std::vector<exatn::numerics::TensorLeg> &,
               bool conjugated,
               bool leg_matching_check)) &
               exatn::numerics::TensorNetwork::placeTensor,
           "")
      .def("appendTensor",
           (bool (exatn::numerics::TensorNetwork::*)(
               unsigned int,
               std::shared_ptr<exatn::numerics::Tensor>,
               const std::vector<std::pair<unsigned int, unsigned int>> &,
               const std::vector<exatn::LegDirection> &,
               bool conjugated)) &
               exatn::numerics::TensorNetwork::appendTensor,
           "")
      .def("reorderOutputModes",
           &exatn::numerics::TensorNetwork::reorderOutputModes, "")
      .def("deleteTensor", &exatn::numerics::TensorNetwork::deleteTensor, "")
      .def("mergeTensors", &exatn::numerics::TensorNetwork::mergeTensors, "")
      .def("isValid", &exatn::numerics::TensorNetwork::isValid, "")
      .def("getRank", &exatn::numerics::TensorNetwork::getRank, "")
      .def("getMaxTensorId", &exatn::numerics::TensorNetwork::getMaxTensorId, "")
      .def("rename", &exatn::numerics::TensorNetwork::rename, "")
      .def("getTensorConnections", &exatn::numerics::TensorNetwork::getTensorConnections, "")    
      .def("appendTensorGate", &exatn::numerics::TensorNetwork::appendTensorGate, "")
      .def("splitTensor", &exatn::numerics::TensorNetwork::splitTensor, "")
      .def("getTensorIdsInNetwork", &exatn::numerics::TensorNetwork::getTensorIdsInNetwork, "")
      .def("conjugate", &exatn::numerics::TensorNetwork::conjugate, "")
      .def("collapseIsometries", &exatn::numerics::TensorNetwork::collapseIsometries, "")
      .def("getContractionCost", &exatn::numerics::TensorNetwork::getContractionCost, "")
     //.def("substituteTensor", &exatn::numerics::TensorNetwork::substituteTensor, "")
     //.def("checkConnections", &exatn::numerics::TensorNetwork::checkConnections, "")   
     //.def("resetOutputTensor", &exatn::numerics::TensorNetwork::resetOutputTensor, "")
     //  .def("appendTensorNetwork", &exatn::numerics::TensorNetwork::appendTensorNetwork, "")
     //  .def("appendTensorNetworkGate", &exatn::numerics::TensorNetwork::appendTensorNetworkGate, "")
     //  .def("importContractionSequence", &exatn::numerics::TensorNetwork::importContractionSequence, "")
     //  .def("exportContractionSequence", &exatn::numerics::TensorNetwork::exportContractionSequence, "")
       .def("getOperationList", &exatn::numerics::TensorNetwork::getOperationList, "");     

}

 