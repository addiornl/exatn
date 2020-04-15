#include "subspace-py.hpp"

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

void bind_subspace(py::module &m) {

  py::class_<exatn::numerics::Subspace>(m, "Subspace", "")
      .def(py::init<exatn::numerics::VectorSpace *, DimOffset, DimOffset>())
      .def(py::init<exatn::numerics::VectorSpace *,
                    std::pair<DimOffset, DimOffset>>())
      .def(py::init<exatn::numerics::VectorSpace *, DimOffset, DimOffset,
                    const std::string &>())
      .def(py::init<exatn::numerics::VectorSpace *,
                    std::pair<DimOffset, DimOffset>, const std::string &>())
      .def("getDimension", &exatn::numerics::Subspace::getDimension, "")
      .def("printIt", &exatn::numerics::Subspace::printIt, "")
      .def("getLowerBound", &exatn::numerics::Subspace::getLowerBound, "")
      .def("getUpperBound", &exatn::numerics::Subspace::getUpperBound, "")
      .def("getBounds", &exatn::numerics::Subspace::getBounds, "")
      .def("getName", &exatn::numerics::Subspace::getName, "")
      .def("getVectorSpace", &exatn::numerics::Subspace::getVectorSpace, "")
      .def("getRegisteredId", &exatn::numerics::Subspace::getRegisteredId, "");

 
}

 