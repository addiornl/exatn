#include "tensorelementtype-py.hpp"

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
void bind_tensorelementtype(py::module &m) {
    
  py::enum_<exatn::TensorElementType>(m, "DataType", py::arithmetic(), "")
      .value("float32", exatn::TensorElementType::REAL32, "")
      .value("float64", exatn::TensorElementType::REAL64, "")
      .value("complex32", exatn::TensorElementType::COMPLEX32, "")
      .value("complex64", exatn::TensorElementType::COMPLEX64, "")
      .value("complex", exatn::TensorElementType::COMPLEX64, "")
      .value("float", exatn::TensorElementType::REAL64, "");
}

 