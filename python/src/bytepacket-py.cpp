#include "bytepacket-py.hpp"

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

void bind_bytepacket(py::module &m) {

   py::class_<BytePacket>(m, "BytePacket", "")
      .def_readwrite("base_addr", &BytePacket::base_addr, "")
      .def_readwrite("size_bytes", &BytePacket::size_bytes, "");
}

 