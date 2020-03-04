#include "spaceregister-py.hpp"

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

void bind_spaceregister(py::module &m) {

  py::class_<exatn::numerics::SpaceRegister>(m, "SpaceRegister")
      .def(py::init<>())
      .def("registerSpace", &exatn::numerics::SpaceRegister::registerSpace, "")
      .def("registerSubspace",
           &exatn::numerics::SpaceRegister::registerSubspace, "")
      .def("getSubspace", &exatn::numerics::SpaceRegister::getSubspace, "")
      .def("getSpace",
           (const exatn::numerics::VectorSpace *(
               exatn::numerics::SpaceRegister::*)(SpaceId) const) &
               exatn::numerics::SpaceRegister::getSpace,
           "")
      .def("getSpace",
           (const exatn::numerics::VectorSpace *(
               exatn::numerics::SpaceRegister::*)(const std::string &)const) &
               exatn::numerics::SpaceRegister::getSpace,
           "");
  
}

 