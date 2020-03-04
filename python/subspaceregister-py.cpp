#include "subspaceregister-py.hpp"

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

void bind_subspaceregister(py::module &m) {

  py::class_<exatn::numerics::SubspaceRegister>(m, "SubspaceRegister", "")
      .def(py::init<>())
      .def("registerSubspace",
           &exatn::numerics::SubspaceRegister::registerSubspace, "")
      .def("getSubspace",
           (const exatn::numerics::Subspace *(
               exatn::numerics::SubspaceRegister::*)(SubspaceId) const) &
               exatn::numerics::SubspaceRegister::getSubspace,
           "")
      .def(
          "getSubspace",
          (const exatn::numerics::Subspace *(
              exatn::numerics::SubspaceRegister::*)(const std::string &)const) &
              exatn::numerics::SubspaceRegister::getSubspace,
          "");
}

 