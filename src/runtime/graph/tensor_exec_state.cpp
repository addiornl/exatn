/** ExaTN:: Tensor Runtime: Tensor graph execution state
REVISION: 2019/07/25

Copyright (C) 2018-2019 Dmitry Lyakh, Tiffany Mintz, Alex McCaskey
Copyright (C) 2018-2019 Oak Ridge National Laboratory (UT-Battelle)
**/

#include "tensor_exec_state.hpp"

#include <iostream>

namespace exatn {
namespace runtime {

int TensorExecState::incrTensorUpdate(const Tensor & tensor)
{
  auto tens_hash = tensor.getTensorHash();
  auto iter = tensor_update_cnt_.find(tens_hash);
  if(iter == tensor_update_cnt_.end()){
    auto pos = tensor_update_cnt_.emplace(std::make_pair(tens_hash,0));
    iter = pos.first;
  }
  auto & count = iter->second;
  ++count;
  return count;
}

int TensorExecState::decrTensorUpdate(const Tensor & tensor)
{
  auto tens_hash = tensor.getTensorHash();
  auto iter = tensor_update_cnt_.find(tens_hash);
  assert(iter != tensor_update_cnt_.end());
  auto & count = iter->second;
  --count;
  return count;
}

bool TensorExecState::getLastTensorRead(const Tensor & tensor, VertexIdType * node_id)
{
  auto tens_hash = tensor.getTensorHash();
  auto iter = tensor_last_read_.find(tens_hash);
  if(iter != tensor_last_read_.end()){
    *node_id = iter->second;
    return true;
  }
  return false;
}

bool TensorExecState::getLastTensorWrite(const Tensor & tensor, VertexIdType * node_id)
{
  auto tens_hash = tensor.getTensorHash();
  auto iter = tensor_last_write_.find(tens_hash);
  if(iter != tensor_last_write_.end()){
    *node_id = iter->second;
    return true;
  }
  return false;
}

void TensorExecState::updateLastTensorRead(const Tensor & tensor, VertexIdType node_id)
{
  auto tens_hash = tensor.getTensorHash();
  auto iter = tensor_last_read_.find(tens_hash);
  if(iter == tensor_last_read_.end()){
    auto pos = tensor_last_read_.emplace(std::make_pair(tens_hash,node_id));
  }else{
    iter->second = node_id;
  }
  return;
}

void TensorExecState::updateLastTensorWrite(const Tensor & tensor, VertexIdType node_id)
{
  auto tens_hash = tensor.getTensorHash();
  auto iter = tensor_last_write_.find(tens_hash);
  if(iter == tensor_last_write_.end()){
    auto pos = tensor_last_write_.emplace(std::make_pair(tens_hash,node_id));
  }else{
    iter->second = node_id;
  }
  return;
}

void TensorExecState::clearLastTensorRead(const Tensor & tensor)
{
  auto tens_hash = tensor.getTensorHash();
  assert(tensor_last_read_.erase(tens_hash) == 1);
  return;
}

void TensorExecState::clearLastTensorWrite(const Tensor & tensor)
{
  auto tens_hash = tensor.getTensorHash();
  assert(tensor_last_write_.erase(tens_hash) == 1);
  return;
}

void TensorExecState::registerDependencyFreeNode(VertexIdType node_id)
{
  nodes_ready_.emplace_back(node_id);
  return;
}

bool TensorExecState::extractDependencyFreeNode(VertexIdType * node_id)
{
  bool empty = nodes_ready_.empty();
  if(!empty){
    *node_id = nodes_ready_.front();
    nodes_ready_.pop_front();
  }
  return !empty;
}

void TensorExecState::registerExecutingNode(VertexIdType node_id)
{
  nodes_executing_.emplace_back(node_id);
  return;
}

bool TensorExecState::extractExecutingNode(VertexIdType * node_id)
{
  bool empty = nodes_executing_.empty();
  if(!empty){
    *node_id = nodes_executing_.front();
    nodes_executing_.pop_front();
  }
  return !empty;
}

} // namespace runtime
} // namespace exatn
