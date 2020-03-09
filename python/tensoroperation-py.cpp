#include "tensoroperation-py.hpp"

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

void bind_tensoroperation(py::module &m) {

py::class_<exatn::numerics::TensorOperation,
             std::shared_ptr<exatn::numerics::TensorOperation>
             >(m, "TensorOperation", "")
      .def("printIt", &exatn::numerics::TensorOperation::printIt, "")
      .def("isSet", &exatn::numerics::TensorOperation::isSet, "")
      .def("getNumOperands", &exatn::numerics::TensorOperation::getNumOperands,
           "")
      .def("getNumOperandsSet",
           &exatn::numerics::TensorOperation::getNumOperandsSet, "")
      .def("getTensorOperandHash",
           &exatn::numerics::TensorOperation::getTensorOperandHash, "")
      .def("getTensorOperand",
           &exatn::numerics::TensorOperation::getTensorOperand, "")
      .def("setTensorOperand",
           &exatn::numerics::TensorOperation::setTensorOperand, "")
      .def("getNumScalars", &exatn::numerics::TensorOperation::getNumScalars,
           "")
      .def("getNumScalarsSet",
           &exatn::numerics::TensorOperation::getNumScalarsSet, "")
      .def("getScalar", &exatn::numerics::TensorOperation::getScalar, "")
      .def("setScalar", &exatn::numerics::TensorOperation::setScalar, "")
      .def("getIndexPattern",
           &exatn::numerics::TensorOperation::getIndexPattern, "")
      .def("setIndexPattern",
           &exatn::numerics::TensorOperation::setIndexPattern, "");

}

 