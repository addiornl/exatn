#include "numserver-py.hpp"

#include "tensor.hpp"
#include "pybind11/pybind11.h"
#include "talshxx.hpp"
#include "tensor_basic.hpp"
#include "exatn_numerics.hpp"
#include "exatn_py_utils.hpp"

namespace py = pybind11;
using namespace exatn;
using namespace exatn::numerics;
using namespace pybind11::literals;

/**

 */
void bind_numserver(py::module &m) {

     py::class_<exatn::NumServer, std::shared_ptr<exatn::NumServer>>(
      m, "NumServer", "")
      .def(py::init<>())
      .def("reconfigureTensorRuntime",
           &exatn::NumServer::reconfigureTensorRuntime, "")
      .def("registerTensorMethod", &exatn::NumServer::registerTensorMethod, "")
      .def("getTensorMethod", &exatn::NumServer::getTensorMethod, "")
      .def("registerExternalData", &exatn::NumServer::registerExternalData, "")
      .def("getExternalData", &exatn::NumServer::getExternalData, "")
      .def("openScope", &exatn::NumServer::openScope, "")
      .def("closeScope", &exatn::NumServer::closeScope, "")
      .def("getVectorSpace", &exatn::NumServer::getVectorSpace, "")
      .def("destroyVectorSpace",
           (void (exatn::NumServer::*)(const std::string &)) &
               exatn::NumServer::destroyVectorSpace,
           "")
      .def("destroyVectorSpace",
           (void (exatn::NumServer::*)(SpaceId)) &
               exatn::NumServer::destroyVectorSpace,
           "")
      .def("getSubspace", &exatn::NumServer::getSubspace, "")
      .def("destroySubspace",
           (void (exatn::NumServer::*)(const std::string &)) &
               exatn::NumServer::destroySubspace,
           "")
      .def("destroySubspace",
           (void (exatn::NumServer::*)(SubspaceId)) &
               exatn::NumServer::destroySubspace,
           "")
      .def("submit",
           (bool (exatn::NumServer::*)(
               std::shared_ptr<exatn::numerics::TensorOperation>)) &
               exatn::NumServer::submit,
           "")
      .def("submit",
           (bool (exatn::NumServer::*)(exatn::numerics::TensorNetwork &)) &
               exatn::NumServer::submit,
           "")
      .def("submit",
           (bool (exatn::NumServer::*)(
               std::shared_ptr<exatn::numerics::TensorNetwork>)) &
               exatn::NumServer::submit,
           "")
      .def("sync",
           (bool (exatn::NumServer::*)(const exatn::numerics::Tensor &, bool)) &
               exatn::NumServer::sync,
           "")
      .def("sync",
           (bool (exatn::NumServer::*)(exatn::numerics::TensorOperation &,
                                       bool)) &
               exatn::NumServer::sync,
           "")
      .def(
          "sync",
          (bool (exatn::NumServer::*)(exatn::numerics::TensorNetwork &, bool)) &
              exatn::NumServer::sync,
          "")
      .def("sync",
           (bool (exatn::NumServer::*)(const std::string &, bool)) &
               exatn::NumServer::sync,
           "")
      .def("getTensorRef", &exatn::NumServer::getTensorRef, "")
      .def(
          "createTensor",
          [](exatn::NumServer &n, const std::string name,
             std::vector<std::size_t> dims) {
            bool created = false;
            created = n.createTensor(name, TensorElementType::REAL64,
                                     exatn::numerics::TensorShape(dims));
            assert(created);
            return;
          },
          "")
      .def(
          "createTensor",
          [](exatn::NumServer &n, const std::string name) {
            bool created = false;
            created = n.createTensor(name, TensorElementType::REAL64);
            assert(created);
            return;
          },
          "")
      .def(
          "createTensor",
          [](exatn::NumServer &n, const std::string name,
             std::vector<std::size_t> dims, exatn::TensorElementType type) {
            bool created = false;
            created =
                n.createTensor(name, type, exatn::numerics::TensorShape(dims));
            assert(created);
            return;
          },
          "")
      .def("createTensor", &exatn::createTensorWithData, "")
      .def(
          "initTensor",
          [](NumServer &n, const std::string &name, float value) {
            return n.initTensorSync(name, value);
          },
          "")
      .def(
          "initTensor",
          [](NumServer &n, const std::string &name, double value) {
            return n.initTensorSync(name, value);
          },
          "")
      .def(
          "initTensor",
          [](NumServer &n, const std::string &name, std::complex<float> value) {
            return n.initTensorSync(name, value);
          },
          "")
      .def(
          "initTensor",
          [](NumServer &n, const std::string &name,
             std::complex<double> value) {
            return n.initTensorSync(name, value);
          },
          "")
      .def("transformTensor", &exatn::NumServer::transformTensorSync, "")
      .def("transformTensor", &exatn::generalTransformWithData,
           "") // py::call_guard<py::gil_scoped_release>(), "")
      .def("print", &exatn::printTensorData, "")
      .def("destroyTensor", &exatn::NumServer::destroyTensor, "")
      .def("evaluateTensorNetwork", &exatn::NumServer::evaluateTensorNetwork,
           "");

}

 