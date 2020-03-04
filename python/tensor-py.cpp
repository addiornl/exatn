#include "tensor-py.hpp"

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
void bind_tensor(py::module &m) {


  py::class_<exatn::numerics::Tensor, std::shared_ptr<exatn::numerics::Tensor>>(
      m, "Tensor", "")
      .def(py::init<std::string>())
      .def(py::init<std::string, exatn::numerics::TensorShape,
                    exatn::numerics::TensorSignature>())
      .def(py::init<std::string, exatn::numerics::TensorShape>())
      .def(py::init<std::string, exatn::numerics::Tensor,
                    exatn::numerics::Tensor,
                    std::vector<exatn::numerics::TensorLeg>>())
      // templated constructor requires integral type for TensorShape -
      // need a definition for each templated constructor and (common) integral
      // type
      .def(py::init<std::string, std::initializer_list<int>,
                    std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::initializer_list<short>,
                    std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::initializer_list<long>,
                    std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::initializer_list<unsigned int>,
                    std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::initializer_list<unsigned short>,
                    std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::initializer_list<unsigned long>,
                    std::initializer_list<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<char>,
                    std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<short>,
                    std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<long>,
                    std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<unsigned int>,
                    std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<unsigned short>,
                    std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<unsigned long>,
                    std::vector<std::pair<SpaceId, SubspaceId>>>())
      .def(py::init<std::string, std::vector<int>>())
      .def(py::init<std::string, std::vector<short>>())
      .def(py::init<std::string, std::vector<long>>())
      .def(py::init<std::string, std::vector<unsigned int>>())
      .def(py::init<std::string, std::vector<unsigned short>>())
      .def(py::init<std::string, std::vector<unsigned long>>())
      .def(py::init<std::string, std::initializer_list<int>>())
      .def(py::init<std::string, std::initializer_list<short>>())
      .def(py::init<std::string, std::initializer_list<long>>())
      .def(py::init<std::string, std::initializer_list<unsigned int>>())
      .def(py::init<std::string, std::initializer_list<unsigned short>>())
      .def(py::init<std::string, std::initializer_list<unsigned long>>())
      .def("printIt", &exatn::numerics::Tensor::printIt, "")
      .def("getName", &exatn::numerics::Tensor::getName, "")
      .def("getRank", &exatn::numerics::Tensor::getRank, "")
      .def("getShape", &exatn::numerics::Tensor::getShape, "")
      .def("getSignature", &exatn::numerics::Tensor::getSignature, "")
      .def("getDimExtent", &exatn::numerics::Tensor::getDimExtent, "")
      .def("getDimExtents", &exatn::numerics::Tensor::getDimExtents, "")
      .def("getDimSpaceId", &exatn::numerics::Tensor::getDimSpaceId, "")
      .def("getDimSubspaceId", &exatn::numerics::Tensor::getDimSubspaceId, "")
      .def("getDimSpaceAttr", &exatn::numerics::Tensor::getDimSpaceAttr, "")
      .def("deleteDimension", &exatn::numerics::Tensor::deleteDimension, "")
      .def("appendDimension",
           (void (exatn::numerics::Tensor::*)(std::pair<SpaceId, SubspaceId>,
                                              DimExtent)) &
               exatn::numerics::Tensor::appendDimension,
           "")
      .def("appendDimension",
           (void (exatn::numerics::Tensor::*)(DimExtent)) &
               exatn::numerics::Tensor::appendDimension,
           "")
      .def("getTensorHash", &exatn::numerics::Tensor::getTensorHash, "");
}

 