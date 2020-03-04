
#include "pybind11/pybind11.h"

#include "exatn.hpp"

namespace py = pybind11;
using namespace exatn;
using namespace exatn::numerics;
using namespace pybind11::literals;

/**
 
*/

void bind_subspaceregentry(py::module& m);