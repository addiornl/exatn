#include "vectorspace-py.hpp"

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

void bind_vectorspace(py::module &m) {

py::class_<exatn::numerics::VectorSpace>(m, "VectorSpace", "")
      .def(py::init<DimExtent>())
      .def(py::init<DimExtent, const std::string>())
      .def(py::init<DimExtent, const std::string,
                    const std::vector<SymmetryRange>>())
      .def("getDimension", &exatn::numerics::VectorSpace::getDimension, "")
      .def("printIt", &exatn::numerics::VectorSpace::printIt, "")
      .def("getName", &exatn::numerics::VectorSpace::getName, "")
      .def("getSymmetrySubranges",
           &exatn::numerics::VectorSpace::getSymmetrySubranges, "")
      .def("registerSymmetrySubrange",
           &exatn::numerics::VectorSpace::registerSymmetrySubrange, "")
      .def("getRegisteredId", &exatn::numerics::VectorSpace::getRegisteredId,
           "");
  
}

 