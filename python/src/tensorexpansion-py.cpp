#include "tensor-py.hpp"

#include "tensor_expansion.hpp"
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
void bind_tensorexpansion(py::module &m) {

  py::class_<exatn::numerics::TensorExpansion, std::shared_ptr<exatn::numerics::TensorExpansion>>(
      m, "TensorExpansion", "")      
      .def("appendComponent", &exatn::numerics::TensorExpansion::appendComponent, "")
      .def("conjugate", &exatn::numerics::TensorExpansion::conjugate, "")
      .def("rename", &exatn::numerics::TensorExpansion::rename, "")
      .def("printIt", &exatn::numerics::TensorExpansion::printIt, "");
            
}
 