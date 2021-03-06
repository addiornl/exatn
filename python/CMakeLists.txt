include_directories(${CMAKE_SOURCE_DIR}/src/exatn)
include_directories(${CMAKE_SOURCE_DIR}/src/driver-rpc)
include_directories(${CMAKE_SOURCE_DIR}/src/numerics)

include_directories(${Python_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/tpls/pybind11/include)
include_directories(${CMAKE_SOURCE_DIR}/tpls/ExaTensor/TALSH)
include_directories(${CMAKE_SOURCE_DIR}/tpls/eigen)

##if (CUDA_FOUND)
 # include_directories(${CUDA_INCLUDE_DIRS})
#endif()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -O2 -g -pipe -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -Wformat -fexceptions --param=ssp-buffer-size=4 -grecord-gcc-switches -m64 -mtune=generic -D_GNU_SOURCE -fPIC -fwrapv")

if(APPLE)
   set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

file (GLOB_RECURSE SOURCES *.cpp)
add_library(_pyexatn SHARED ${SOURCES})

set_target_properties(_pyexatn PROPERTIES PREFIX "")

target_link_libraries(_pyexatn PUBLIC CppMicroServices exatn exatn-numerics)

if(APPLE)
   set_target_properties(_pyexatn PROPERTIES INSTALL_RPATH "@loader_path/lib;@loader_path/plugins")
   set_target_properties(_pyexatn PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
else()
   set_target_properties(_pyexatn PROPERTIES INSTALL_RPATH "$ORIGIN/lib;$ORIGIN/plugins")
   set_target_properties(_pyexatn PROPERTIES LINK_FLAGS "-shared")
endif()

install(TARGETS _pyexatn DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES exatn.py DESTINATION ${CMAKE_INSTALL_PREFIX})
