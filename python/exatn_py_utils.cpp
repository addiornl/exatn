#include "exatn_py_utils.hpp"

namespace exatn {

TensorElementType TypeToTensorElementType<float>::type =
    TensorElementType::REAL32;
TensorElementType TypeToTensorElementType<double>::type =
    TensorElementType::REAL64;
TensorElementType TypeToTensorElementType<std::complex<double>>::type =
    TensorElementType::COMPLEX64;
    
bool createTensorWithDataNoNumServer(const std::string name,
                          py::array &data) {
  auto n = exatn::numericalServer;
  auto shape = data.shape();
  std::vector<std::size_t> dims(data.ndim());
  for (int i = 0; i < data.ndim(); i++) {
    dims[i] = shape[i];
  }

  // Learn underlying data type of py::array in order
  // to set TensorElementType
  TensorElementType type;
  if (py::isinstance<py::array_t<double>>(data)) {
    type = TensorElementType::REAL64;
  } else if (py::isinstance<py::array_t<std::complex<double>>>(data)) {
    type = TensorElementType::COMPLEX64;
  } else if (py::isinstance<py::array_t<float>>(data)) {
    type = TensorElementType::REAL32;
  } else if (py::isinstance<py::array_t<std::complex<float>>>(data)) {
    type = TensorElementType::COMPLEX32;
  }

  auto created = n->createTensor(name, type, exatn::numerics::TensorShape(dims));
  assert(created);
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(data, type);
  return n->transformTensorSync(name, functor);
}

bool createTensorWithData(exatn::NumServer &n, const std::string name,
                          py::array &data) {
  auto shape = data.shape();
  std::vector<std::size_t> dims(data.ndim());
  for (int i = 0; i < data.ndim(); i++) {
    dims[i] = shape[i];
  }

  // Learn underlying data type of py::array in order
  // to set TensorElementType
  TensorElementType type;
  if (py::isinstance<py::array_t<double>>(data)) {
    type = TensorElementType::REAL64;
  } else if (py::isinstance<py::array_t<std::complex<double>>>(data)) {
    type = TensorElementType::COMPLEX64;
  } else if (py::isinstance<py::array_t<float>>(data)) {
    type = TensorElementType::REAL32;
  } else if (py::isinstance<py::array_t<std::complex<float>>>(data)) {
    type = TensorElementType::COMPLEX32;
  }

  auto created = n.createTensor(name, type, exatn::numerics::TensorShape(dims));
  assert(created);
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(data, type);
  return n.transformTensorSync(name, functor);
}

bool generalTransformWithData(exatn::NumServer &n, const std::string &name,
                              std::function<void(py::array& buffer)> &f) {
  auto type = n.getTensorElementType(name);
  std::function<void(py::array&)> wrapper = [f] (py::array& buffer) {
      py::gil_scoped_release r;
      f(buffer);
      return;
  };
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(
      wrapper,
      type);
  auto worked = n.transformTensorSync(name, functor);
  return worked;
}


bool generalTransformWithDataNoNumServer(const std::string &name,
                              std::function<void(py::array& buffer)> &f) {
auto n = exatn::numericalServer;
  auto type = n->getTensorElementType(name);
  std::function<void(py::array&)> wrapper = [f] (py::array& buffer) {
      py::gil_scoped_release r;
      f(buffer);
      return;
  };
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(
      wrapper,
      type);
  auto worked = n->transformTensorSync(name, functor);
  return worked;
}

void printTensorData(exatn::NumServer &n, const std::string &name) {
  auto type = n.getTensorElementType(name);
  std::function<void(py::array&)> f = [](py::array& data) {
     py::print("Shape: ", data.shape());
     py::print(data);

  };
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(
      f,
      type);
  auto worked = n.transformTensorSync(name, functor);
  return;
}


void printTensorDataNoNumServer(const std::string &name) {
    auto n = exatn::numericalServer;
  auto type = n->getTensorElementType(name);
  std::function<void(py::array&)> f = [](py::array& data) {
     py::print("Shape: ", data.shape());
     py::print(data);

  };
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(
      f,
      type);
  auto worked = n->transformTensorSync(name, functor);
  return;
}

const py::array getTensorData(const std::string& name) {
    py::array a;
    auto n = exatn::numericalServer;
  auto type = n->getTensorElementType(name);
    std::function<void(py::array&)> f = [&a](py::array& data) {
     py::print("Shape: ", data.shape());
     py::print(data);
     a = data;
  };
  auto functor = std::make_shared<NumpyTensorFunctorCppWrapper>(
      f,
      type);
  auto worked = n->transformTensorSync(name, functor);
  return a;
}
} // namespace exatn
