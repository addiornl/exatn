#include "pybind11/detail/common.h"
#include <pybind11/complex.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include "pybind11/pybind11.h"

#include "exatn.hpp"

namespace py = pybind11;
using namespace exatn;
using namespace exatn::numerics;
using namespace pybind11::literals;

/**
 
*/

void bind_vectorspace(py::module& m);