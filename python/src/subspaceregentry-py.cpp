#include "subspaceregentry-py.hpp"

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

void bind_subspaceregentry(py::module &m) {

  py::class_<exatn::numerics::SubspaceRegEntry>(m, "SubspaceRegEntry")
      .def(py::init<std::shared_ptr<Subspace>>());
  
}

 