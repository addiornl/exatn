#include "driverclient-py.hpp"

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

void bind_driverclient(py::module &m) {


  py::class_<exatn::rpc::DriverClient,
             std::shared_ptr<exatn::rpc::DriverClient>>(m, "DriverClient", "")
      .def("interpretTAProL", &exatn::rpc::DriverClient::interpretTAProL, "")
      .def("registerTensorMethod",
           &exatn::rpc::DriverClient::registerTensorMethod, "")
      .def("getResults", &exatn::rpc::DriverClient::getResults, "")
      .def("shutdown", &exatn::rpc::DriverClient::shutdown, "");
}

 