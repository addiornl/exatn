#include "spacebasis-py.hpp"

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

void bind_spacebasis(py::module &m) {

  py::class_<exatn::numerics::SpaceBasis>(m, "SpaceBasis")
      .def(py::init<DimExtent>())
      .def(py::init<DimExtent,
                    const std::vector<exatn::numerics::SymmetryRange>>())
      .def("printIt", &exatn::numerics::SpaceBasis::printIt, "")
      .def("getDimension", &exatn::numerics::SpaceBasis::getDimension, "")
      .def("getSymmetrySubranges",
           &exatn::numerics::SpaceBasis::getSymmetrySubranges, "")
      .def("registerSymmetrySubrange",
           &exatn::numerics::SpaceBasis::registerSymmetrySubrange, "");
  
}

 