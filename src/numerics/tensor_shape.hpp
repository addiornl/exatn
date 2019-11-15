/** ExaTN::Numerics: Tensor shape
REVISION: 2019/11/12

Copyright (C) 2018-2019 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2019 Oak Ridge National Laboratory (UT-Battelle) **/

/** Rationale:
 (a) Tensor shape is an ordered set of tensor dimension extents.
     A scalar tensor (rank-0 tensor) has an empty shape.
**/

#ifndef EXATN_NUMERICS_TENSOR_SHAPE_HPP_
#define EXATN_NUMERICS_TENSOR_SHAPE_HPP_

#include "tensor_basic.hpp"

#include <iostream>
#include <fstream>
#include <type_traits>
#include <initializer_list>
#include <vector>

#include <cassert>

namespace exatn{

namespace numerics{

class TensorShape{
public:

 /** Create an empty tensor shape. **/
 TensorShape();

 /** Create a tensor shape by specifying extents for all tensor dimensions. **/
 template<typename T>
 TensorShape(std::initializer_list<T> extents);
 template<typename T>
 TensorShape(const std::vector<T> & extents);

 /** Create a tensor shape by permuting another tensor shape. **/
 TensorShape(const TensorShape & another,              //in: another tensor shape
             const std::vector<unsigned int> & order); //in: new oder (N2O)

 TensorShape(const TensorShape &) = default;
 TensorShape & operator=(const TensorShape &) = default;
 TensorShape(TensorShape &&) noexcept = default;
 TensorShape & operator=(TensorShape &&) noexcept = default;
 virtual ~TensorShape() = default;

 /** Print. **/
 void printIt() const;
 void printItFile(std::ofstream & output_file) const;

 /** Get tensor rank (number of tensor dimensions). **/
 unsigned int getRank() const;

 /** Get the extent of a specific tensor dimension. **/
 DimExtent getDimExtent(unsigned int dim_id) const;

 /** Get the extents of all tensor dimensions. **/
 const std::vector<DimExtent> & getDimExtents() const;

 /** Returns TRUE if the tensor shape coincides with another tensor shape. **/
 bool isCongruentTo(const TensorShape & another) const;

 /** Resets a specific dimension. **/
 void resetDimension(unsigned int dim_id, DimExtent extent);

 /** Deletes a specific dimension, reducing the shape rank by one. **/
 void deleteDimension(unsigned int dim_id);

 /** Appends a new dimension at the end, increasing the shape rank by one. **/
 void appendDimension(DimExtent dim_extent);

private:

 std::vector<DimExtent> extents_; //tensor dimension extents
};


//TEMPLATES:
template<typename T>
TensorShape::TensorShape(std::initializer_list<T> extents):
extents_(extents.size())
{
 static_assert(std::is_integral<T>::value,"FATAL(TensorShape::TensorShape): TensorShape extent type must be integral!");

 //DEBUG:
 for(const auto & extent: extents){
  if(extent < 0) std::cout << "ERROR(TensorShape::TensorShape): Negative dimension extent passed!" << std::endl;
  assert(extent >= 0);
 }

 int i = 0;
 for(const auto & extent: extents) extents_[i++] = static_cast<DimExtent>(extent);
}

template<typename T>
TensorShape::TensorShape(const std::vector<T> & extents):
extents_(extents.size())
{
 static_assert(std::is_integral<T>::value,"FATAL(TensorShape::TensorShape): TensorShape extent type must be integral!");

 //DEBUG:
 for(const auto & extent: extents){
  if(extent < 0) std::cout << "ERROR(TensorShape::TensorShape): Negative dimension extent passed!" << std::endl;
  assert(extent >= 0);
 }

 int i = 0;
 for(const auto & extent: extents) extents_[i++] = static_cast<DimExtent>(extent);
}

} //namespace numerics

} //namespace exatn

#endif //EXATN_NUMERICS_TENSOR_SHAPE_HPP_
