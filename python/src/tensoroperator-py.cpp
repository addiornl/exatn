#include "tensor-py.hpp"

#include "tensor_operator.hpp"
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
void bind_tensoroperator(py::module &m) {


  py::class_<exatn::numerics::TensorOperator, std::shared_ptr<exatn::numerics::TensorOperator>>(
      m, "TensorOperator", "")      
      // .def("appendComponent", &exatn::numerics::TensorOperator::appendComponent, "")
      // .def("appendComponent", &exatn::numerics::TensorOperator::appendComponent, "")
      .def("conjugate", &exatn::numerics::TensorOperator::conjugate, "")
      .def("printIt", &exatn::numerics::TensorOperator::printIt, "");
     
}
 