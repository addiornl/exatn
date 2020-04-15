#include "basisvector-py.hpp"

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

void bind_basisvector(py::module &m) {

  py::class_<exatn::numerics::BasisVector>(m, "BasisVector")
      .def(py::init<SubspaceId>())
      .def("printIt", &exatn::numerics::BasisVector::printIt, "");
}

 