#include "exatn_py_utils.hpp"
#include "pybind11/pybind11.h"
#include "talshxx.hpp"
#include "tensor_basic.hpp"
#include "exatn_numerics.hpp"
#include "bytepacket-py.hpp"
#include "driverclient-py.hpp"
#include "tensoroperation-py.hpp"
#include "tensoropadd-py.hpp"
#include "tensoropcontract-py.hpp"
#include "tensoropcreate-py.hpp"
#include "tensoropdestroy-py.hpp"
#include "tensoropfactory-py.hpp"
#include "tensoroptransform-py.hpp"
#include "tensornetwork-py.hpp"
#include "vectorspace-py.hpp"
#include "subspace-py.hpp"
#include "tensor-py.hpp"
#include "legdirection-py.hpp"
#include "tensoropcode-py.hpp"
#include "tensorleg-py.hpp"
#include "tensorelementtype-py.hpp"
#include "numserver-py.hpp"
#include "tensorconn-py.hpp"
#include "tensorshape-py.hpp"
#include "tensorsignature-py.hpp"
#include "subspaceregentry-py.hpp"
#include "subspaceregister-py.hpp"
#include "spaceregentry-py.hpp"
#include "spaceregister-py.hpp"
#include "symmetryrange-py.hpp"
#include "spacebasis-py.hpp"
#include "basisvector-py.hpp"
#include "tensoroperator-py.hpp"
#include "tensorexpansion-py.hpp"

namespace py = pybind11;
using namespace exatn;
using namespace exatn::numerics;
using namespace pybind11::literals;

/**
 * This module provides the necessary bindings for the ExaTN classes and
 * functionality to be used within python. The various classes are bound with
 * pybind11 and, when this is included in CMake compilation, can be used to
 * write python scripts which leverage the ExaTN functionality.
 */

void create_exatn_py_module(py::module &m) {
  m.doc() = "Python bindings for ExaTN.";

     bind_bytepacket(m);
     bind_driverclient(m);
     bind_tensoroperation(m);
     bind_tensoropadd(m);
     bind_tensoropcontract(m);
     bind_tensoropcreate(m);
     bind_tensoropdestroy(m);
     bind_tensoropfactory(m);
     bind_tensoroptransform(m);
     bind_tensornetwork(m);
     bind_vectorspace(m);
     bind_subspace(m);
     bind_tensor(m);     
     bind_legdirection(m);
     bind_tensoropcode(m);
     bind_tensorleg(m);
     bind_tensorelementtype(m);
     bind_numserver(m);
     bind_tensorconn(m);
     bind_tensorshape(m);
     bind_tensorsignature(m);
     bind_subspaceregentry(m);
     bind_subspaceregister(m);
     bind_spaceregentry(m);
     bind_spaceregister(m);
     bind_symmetryrange(m);
     bind_spacebasis(m);
     bind_basisvector(m);
     bind_tensoroperator(m);
     bind_tensorexpansion(m);

  /**
   ExaTN module definitions
  */
  m.def("Initialize", (void (*)()) & exatn::initialize,
        "Initialize the exatn framework.");
  m.def(
      "getDriverClient",
      [](const std::string name) -> std::shared_ptr<exatn::rpc::DriverClient> {
        return exatn::getService<exatn::rpc::DriverClient>(name);
      },
      "");
  m.def("Finalize", &exatn::finalize, "Finalize the framework");

  m.def(
      "getNumServer", []() { return exatn::numericalServer; },
      py::return_value_policy::reference, "");

  m.def(
      "createVectorSpace",
      [](const std::string &space_name, DimExtent space_dim) {
        const VectorSpace *space;
        return exatn::numericalServer->createVectorSpace(space_name, space_dim,
                                                         &space);
      },
      py::return_value_policy::reference, "");

  m.def(
      "getVectorSpace",
      [](const std::string &space_name) {
        return exatn::numericalServer->getVectorSpace(space_name);
      },
      py::return_value_policy::reference, "");

  m.def(
      "createSubspace",
      [](const std::string &subspace_name, const std::string &space_name,
         std::pair<DimOffset, DimOffset> bounds) {
        const Subspace *subspace;
        return numericalServer->createSubspace(subspace_name, space_name,
                                               bounds, &subspace);
      },
      py::return_value_policy::reference, "");
  m.def(
      "getSubspace",
      [](const std::string &subspace_name) {
        return exatn::numericalServer->getSubspace(subspace_name);
      },
      py::return_value_policy::reference, "");

  // TensorNetwork *network is the network to append to, and TensorNetwork
  // append_network is the network that will be appended to *network PyBind
  // cannot bind this function simply within the TensorNetwork class due to the
  // && argument
  m.def(
      "appendTensorNetwork",
      [](TensorNetwork *network, TensorNetwork append_network,
         const std::vector<std::pair<unsigned int, unsigned int>> &pairing) {
        return network->appendTensorNetwork(std::move(append_network), pairing);
      },
      "");

  m.def("createTensor", [](const std::string &name, double &value) {
    auto success = exatn::createTensor(name, exatn::TensorElementType::REAL64);
    if (success) {
      return exatn::initTensorSync(name, value);
    }
    return success;
  });
  m.def("createTensor",
        [](const std::string &name, std::complex<double> &value) {
          auto success =
              exatn::createTensor(name, exatn::TensorElementType::COMPLEX64);
          if (success) {
            return exatn::initTensorSync(name, value);
          }
          return success;
        });

  m.def(
      "createTensor",
      [](const std::string &name, TensorElementType type) {
        return exatn::createTensor(name, type);
      },
      "");
  m.def(
      "createTensor",
      [](const std::string &name, std::vector<std::size_t> dims,
         TensorElementType type) {
        return exatn::createTensor(name, type,
                                   exatn::numerics::TensorShape(dims));
      },
      "");
  m.def("createTensor", &createTensorWithDataNoNumServer, "");
  m.def("print", &printTensorDataNoNumServer, "");
  m.def("transformTensor", &generalTransformWithDataNoNumServer, "");
  m.def("evaluateTensorNetwork", &evaluateTensorNetwork, "");
  m.def("getTensorData", &getTensorData, "");
  m.def("getLocalTensor", [](const std::string &name) {
    auto local_tensor = exatn::getLocalTensor(name);
    unsigned int nd = local_tensor->getRank();

    std::vector<std::size_t> dims_vec(nd);
    auto dims = local_tensor->getDimExtents(nd);
    for (int i = 0; i < nd; i++) {
      dims_vec[i] = dims[i];
    }

    auto tensorType = local_tensor->getElementType();

    if (tensorType == talsh::REAL64) {
      double *elements;
      auto worked = local_tensor->getDataAccessHost(&elements);
      auto cap = py::capsule(
          elements, [](void *v) { /* deleter, I do not own this... */ });
      auto arr = py::array(dims_vec, elements, cap);
      return arr;
    } else if (tensorType == talsh::COMPLEX64) {
      std::complex<double> *elements;
      auto worked = local_tensor->getDataAccessHost(&elements);
      auto cap = py::capsule(
          elements, [](void *v) { /* deleter, I do not own this... */ });
      auto arr = py::array(dims_vec, elements, cap);
      return arr;

    } else if (tensorType == talsh::COMPLEX32) {
      std::complex<float> *elements;
      auto worked = local_tensor->getDataAccessHost(&elements);
      auto cap = py::capsule(
          elements, [](void *v) { /* deleter, I do not own this... */ });
      auto arr = py::array(dims_vec, elements, cap);
      return arr;

    } else if (tensorType == talsh::REAL32) {
      float *elements;
      auto worked = local_tensor->getDataAccessHost(&elements);
      auto cap = py::capsule(
          elements, [](void *v) { /* deleter, I do not own this... */ });
      auto arr = py::array(dims_vec, elements, cap);
      return arr;

    } else {
      assert(false && "Invalid TensorElementType");
    }
    return py::array();
  });
  m.def("destroyTensor", &destroyTensor, "");
}

PYBIND11_MODULE(_pyexatn, m) { create_exatn_py_module(m); }