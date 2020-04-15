#include "tensorshape-py.hpp"

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

void bind_tensorshape(py::module &m) {

py::class_<exatn::numerics::TensorShape>(m, "TensorShape", "")
      .def(py::init<>())
     //  .def(py::init<std::initializer_list<int>>())
     //  .def(py::init<std::initializer_list<short>>())
     //  .def(py::init<std::initializer_list<long>>())
     //  .def(py::init<std::initializer_list<unsigned int>>())
     //  .def(py::init<std::initializer_list<unsigned short>>())
     //  .def(py::init<std::initializer_list<unsigned long>>())
      .def(py::init<std::vector<int>>())
     //  .def(py::init<std::vector<short>>())
     //  .def(py::init<std::vector<long>>())
     //  .def(py::init<std::vector<unsigned int>>())
     //  .def(py::init<std::vector<unsigned short>>())
     //  .def(py::init<std::vector<unsigned long>>())
      .def("printIt", &exatn::numerics::TensorShape::printIt, "")
      .def("getRank", &exatn::numerics::TensorShape::getRank, "")
      .def("getDimExtent", &exatn::numerics::TensorShape::getDimExtent, "")
      .def("getDimExtents", &exatn::numerics::TensorShape::getDimExtents, "")
      .def("resetDimension", &exatn::numerics::TensorShape::resetDimension, "")
      .def("deleteDimension", &exatn::numerics::TensorShape::deleteDimension,
           "")
      .def("appendDimension", &exatn::numerics::TensorShape::appendDimension,
           "");
  
}

 