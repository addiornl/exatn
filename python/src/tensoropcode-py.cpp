#include "tensoropcode-py.hpp"

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
void bind_tensoropcode(py::module &m) {

  py::enum_<exatn::TensorOpCode>(m, "TensorOpCode")
      .value("NOOP", exatn::TensorOpCode::NOOP)
      .value("CREATE", exatn::TensorOpCode::CREATE)
      .value("DESTROY", exatn::TensorOpCode::DESTROY)
      .value("TRANSFORM", exatn::TensorOpCode::TRANSFORM)
      .value("ADD", exatn::TensorOpCode::ADD)
      .value("CONTRACT", exatn::TensorOpCode::CONTRACT)
      .export_values();

}

 