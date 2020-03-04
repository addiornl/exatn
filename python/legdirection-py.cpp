#include "legdirection-py.hpp"

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
void bind_legdirection(py::module &m) {

  py::enum_<exatn::LegDirection>(m, "LegDirection")
      .value("UNDIRECT", exatn::LegDirection::UNDIRECT)
      .value("INWARD", exatn::LegDirection::INWARD)
      .value("OUTWARD", exatn::LegDirection::OUTWARD)
      .export_values();

}

 