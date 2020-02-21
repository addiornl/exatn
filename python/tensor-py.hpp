/*#ifndef EXATN_TENSOR_PY_HPP_
#define EXATN_TENSOR_PY_HPP_*/

#include "num_server.hpp"
#include "pybind11/detail/common.h"
#include <pybind11/complex.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "DriverClient.hpp"
#include "exatn.hpp"
#include "pybind11/pybind11.h"
#include "talshxx.hpp"
#include "tensor_basic.hpp"
#include "tensor_method.hpp"

#include <type_traits>

namespace py = pybind11;
using namespace exatn;
using namespace exatn::numerics;
using namespace pybind11::literals;

/**
  Trampoline class for abstract virtual functions in TensorOperation
*/


class PyTensor : public exatn::numerics::Tensor {
public:
  /* Inherit the constructors */
  using Tensor::Tensor;

 /** Print. **/
void printIt(){  PYBIND11_OVERLOAD(void, exatn::numerics::Tensor, printIt,); }
 void printItFile(std::ofstream & output_file){PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, printItFile,output_file ); }

 /** Rename (use this method with care as it can mess up higher-level maps). **/
 void rename(const std::string & name){ PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, rename,name );} 

 /** Get tensor name. **/
 const std::string & getName(){ PYBIND11_OVERLOAD_PURE(std::string &, exatn::numerics::Tensor, getName, );} 

 /** Get the tensor rank (order). **/
 unsigned int getRank(){ PYBIND11_OVERLOAD_PURE( unsigned int, exatn::numerics::Tensor, getRank, );} 

 /** Get the tensor shape. **/
 const TensorShape & getShape(){ PYBIND11_OVERLOAD_PURE(TensorShape &, exatn::numerics::Tensor, getShape, );} 

 /** Get the tensor signature. **/
 const TensorSignature & getSignature(){ PYBIND11_OVERLOAD_PURE(TensorSignature &, exatn::numerics::Tensor, getSignature, );} 

 /** Get the extent of a specific tensor dimension. **/
 DimExtent getDimExtent(unsigned int dim_id){ PYBIND11_OVERLOAD_PURE(DimExtent, exatn::numerics::Tensor, getDimExtent, dim_id );} 

 /** Get the extents of all tensor dimensions. **/
 const std::vector<DimExtent> & getDimExtents(){ PYBIND11_OVERLOAD_PURE(std::vector<DimExtent> & , exatn::numerics::Tensor, getDimExtents, );} 

 /** Get the strides for all tensor dimensions.
     Column-major tensor storage layout is assumed. **/
 const std::vector<DimExtent> getDimStrides(DimExtent * volume = nullptr){ PYBIND11_OVERLOAD_PURE(std::vector<DimExtent>, exatn::numerics::Tensor, getDimStrides,volume );} 

 /** Get the space/subspace id for a specific tensor dimension. **/
 SpaceId getDimSpaceId(unsigned int dim_id){ PYBIND11_OVERLOAD_PURE(SpaceId, exatn::numerics::Tensor, getDimSpaceId,dim_id );} 
 SubspaceId getDimSubspaceId(unsigned int dim_id){ PYBIND11_OVERLOAD_PURE(SubspaceId, exatn::numerics::Tensor, getDimSubspaceId,dim_id );} 
 /*std::pair<SpaceId,SubspaceId> getDimSpaceAttr(unsigned int dim_id){ PYBIND11_OVERLOAD_PURE(std::pair<SpaceId,SubspaceId>, exatn::numerics::Tensor, getDimSpaceAttr,dim_id );} */

 /** Returns TRUE if the tensor is congruent to another tensor, that is,
     it has the same shape and signature. **/
 bool isCongruentTo(const Tensor & another){ PYBIND11_OVERLOAD_PURE(bool, exatn::numerics::Tensor, isCongruentTo,another );} 

 /** Deletes a specific tensor dimension, reducing the tensor rank by one. **/
 void deleteDimension(unsigned int dim_id){ PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, deleteDimension,dim_id );} 

 /** Appends a new dimension to the tensor at the end, increasing the tensor rank by one. **/
 void appendDimension(std::pair<SpaceId,SubspaceId> subspace,
                      DimExtent dim_extent){ PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, appendDimension,subspace, dim_extent );} 
 void appendDimension(DimExtent dim_extent){ PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, appendDimension,dim_extent );} 

 /** Creates a new tensor from the current tensor by selecting a subset of its modes.
     Vector mode_mask must have the size equal to the original tensor rank:
     mode_mask[i] == mask_val will select dimension i for appending to the subtensor. **/
 std::shared_ptr<Tensor> createSubtensor(const std::string & name,           //in: subtensor name
                                         const std::vector<int> & mode_mask, //in: mode masks
                                         int mask_val){ PYBIND11_OVERLOAD_PURE(std::shared_ptr<Tensor>, exatn::numerics::Tensor,createSubtensor,name,mode_mask, mask_val);}                     //in: chosen mask value

 /** Sets the tensor element type. **/
 void setElementType(TensorElementType element_type){ PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, setElementType,element_type);} 

 /** Returns the tensor element type. **/
 TensorElementType getElementType(){ PYBIND11_OVERLOAD_PURE(TensorElementType, exatn::numerics::Tensor, getElementType );} 

 /** Registers an isometry in the tensor. **/
 void registerIsometry(const std::vector<unsigned int> & isometry){ PYBIND11_OVERLOAD_PURE(void, exatn::numerics::Tensor, registerIsometry,isometry );} 

 /** Retrieves the list of all registered isometries in the tensor. **/
 const std::list<std::vector<unsigned int>> & retrieveIsometries(){ PYBIND11_OVERLOAD_PURE(std::list<std::vector<unsigned int>> &, exatn::numerics::Tensor, printItFile, );} 

 /** Get the unique integer tensor id. **/
 TensorHashType getTensorHash(){ PYBIND11_OVERLOAD_PURE(TensorHashType, exatn::numerics::Tensor, getTensorHash );}
 }; 

void bind_tensor(py::module& m);