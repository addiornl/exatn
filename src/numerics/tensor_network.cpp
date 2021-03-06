/** ExaTN::Numerics: Tensor network
REVISION: 2020/01/13

Copyright (C) 2018-2020 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2020 Oak Ridge National Laboratory (UT-Battelle) **/

#include "tensor_network.hpp"
#include "tensor_symbol.hpp"
#include "contraction_seq_optimizer_factory.hpp"
#include "functor_init_val.hpp"

#include <iostream>
#include <cassert>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <algorithm>

namespace exatn{

namespace numerics{

//Tensor contraction sequence optmizers:
std::map<std::string,std::shared_ptr<ContractionSeqOptimizer>> optimizers;


TensorNetwork::TensorNetwork():
 explicit_output_(0), finalized_(1), max_tensor_id_(0), contraction_seq_flops_(0.0)
{
 auto res = emplaceTensorConnDirect(false,
                                    0U, //output tensor (id = 0)
                                    std::make_shared<Tensor>("_SMOKY_TENSOR_"),0U,std::vector<TensorLeg>{});
 if(!res){
  std::cout << "#ERROR(exatn::numerics::TensorNetwork::TensorNetwork): Tensor id already in use!" << std::endl;
  assert(false);
 }
}


TensorNetwork::TensorNetwork(const std::string & name):
 explicit_output_(0), finalized_(1), name_(name), max_tensor_id_(0), contraction_seq_flops_(0.0)
{
 auto res = emplaceTensorConnDirect(false,
                                    0U, //output tensor (id = 0)
                                    std::make_shared<Tensor>(name),0U,std::vector<TensorLeg>{});
 if(!res){
  std::cout << "#ERROR(exatn::numerics::TensorNetwork::TensorNetwork): Tensor id already in use!" << std::endl;
  assert(false);
 }
}


TensorNetwork::TensorNetwork(const std::string & name,
                             std::shared_ptr<Tensor> output_tensor,
                             const std::vector<TensorLeg> & output_legs):
 explicit_output_(1), finalized_(0), name_(name), max_tensor_id_(0), contraction_seq_flops_(0.0)
{
 auto res = emplaceTensorConnDirect(false,
                                    0U, //output tensor (id = 0)
                                    output_tensor,0U,output_legs);
 if(!res){
  std::cout << "#ERROR(exatn::numerics::TensorNetwork::TensorNetwork): Tensor id already in use!" << std::endl;
  assert(false);
 }
}


TensorNetwork::TensorNetwork(const std::string & name,
                             const std::string & tensor_network,
                             const std::map<std::string,std::shared_ptr<Tensor>> & tensors):
 explicit_output_(1), finalized_(0), name_(name), max_tensor_id_(0), contraction_seq_flops_(0.0)
{
 //Convert tensor hypernetwork into regular tensor network, if needed:
 //`Finish
 //Build a regular tensor network according the the provided symbolic specification:
 std::map<std::string,std::vector<TensorLeg>> index_map; //index label --> list of tensor legs associated with this index label
 std::vector<std::string> stensors; //individual tensors of the tensor network (symbolic)
 if(parse_tensor_network(tensor_network,stensors)){
  //Construct index correspondence map:
  std::string tensor_name;
  std::vector<IndexLabel> indices;
  for(unsigned int i = 0; i < stensors.size(); ++i){
   bool conjugated;
   if(parse_tensor(stensors[i],tensor_name,indices,conjugated)){
    for(unsigned int j = 0; j < indices.size(); ++j){
     auto pos = index_map.find(indices[j].label);
     if(pos == index_map.end()){
      auto res = index_map.emplace(std::make_pair(indices[j].label,std::vector<TensorLeg>{}));
      assert(res.second);
      pos = res.first;
     }
     pos->second.emplace_back(TensorLeg(i,j,indices[j].direction)); //directed index #j of tensor #i
    }
   }else{
    std::cout << "#ERROR(TensorNetwork::TensorNetwork): Invalid tensor in symbolic tensor network specification: " <<
     stensors[i] << std::endl;
    assert(false);
   }
   indices.clear();
   tensor_name.clear();
  }
  //Build the tensor network object:
  for(unsigned int i = 0; i < stensors.size(); ++i){
   bool conjugated;
   if(parse_tensor(stensors[i],tensor_name,indices,conjugated)){
    auto tensor = tensors.find(tensor_name);
    if(tensor != tensors.end()){
     std::vector<TensorLeg> legs;
     for(unsigned int j = 0; j < indices.size(); ++j){
      auto pos = index_map.find(indices[j].label);
      assert(pos != index_map.end());
      const auto & inds = pos->second;
      for(const auto & ind: inds){
       if(ind.getTensorId() != i || ind.getDimensionId() != j){
        legs.emplace_back(ind);
       }
      }
     }
     if(i == 0){
      assert(!conjugated); //output tensor must not appear complex conjugated
      auto res = emplaceTensorConnDirect(false,
                                         0U, //output tensor (id = 0)
                                         tensor->second,0U,legs);
      if(!res){
       std::cout << "#ERROR(exatn::numerics::TensorNetwork::TensorNetwork): Tensor id already in use!" << std::endl;
       assert(false);
      }
     }else{ //input tensor
      this->placeTensor(i,tensor->second,legs,conjugated);
     }
    }else{
     std::cout << "#ERROR(TensorNetwork::TensorNetwork): Unable to find tensor named " <<
      tensor_name << std::endl;
     assert(false);
    }
   }
  }
 }else{
  std::cout << "#ERROR(TensorNetwork::TensorNetwork): Invalid symbolic tensor network specification: " <<
   tensor_network << std::endl;
  assert(false);
 }
 bool finalized = this->finalize();
 assert(finalized);
}


TensorNetwork::TensorNetwork(const std::string & name,
                             std::shared_ptr<Tensor> output_tensor,
                             NetworkBuilder & builder):
 explicit_output_(1), finalized_(0), name_(name), max_tensor_id_(0), contraction_seq_flops_(0.0)
{
 auto res = emplaceTensorConnDirect(false,
                                    0U, //output tensor (id = 0)
                                    output_tensor,0U,
                                    std::vector<TensorLeg>(output_tensor->getRank(),TensorLeg(0,0))); //dummy legs
 if(!res){
  std::cout << "#ERROR(exatn::numerics::TensorNetwork::TensorNetwork): Tensor id already in use!" << std::endl;
  assert(false);
 }
 builder.build(*this); //create and link input tensors of the tensor network
 finalized_ = 1;
 updateConnectionsFromInputTensors(); //update output tensor legs
}


TensorNetwork::TensorNetwork(const TensorNetwork & another,
                             bool replace_output,
                             const std::string & new_output_name)
{
 *this = another;
 if(replace_output) this->resetOutputTensor(new_output_name);
}

/*
TensorNetwork::TensorNetwork(const TensorNetwork & another)
{
 explicit_output_ = 1;
 finalized_ = 0;
 name_ = another.getName();
 max_tensor_id_ = 0;
 contraction_seq_flops_ = 0.0;

 auto output_tensor = another.getTensor(0);
 const auto & output_legs = *(another.getTensorConnections(0));
 auto new_output_tensor = makeSharedTensor(*output_tensor);
 new_output_tensor->rename(generateTensorName(*new_output_tensor));
 auto res = emplaceTensorConnDirect(false,
                                    0U, //output tensor (id = 0)
                                    new_output_tensor,0U,output_legs);
 assert(res);
 for(auto iter = another.cbegin(); iter != another.cend(); ++iter){
  if(iter->first != 0){ //only input tensors
   res = emplaceTensorConn(iter->first,iter->second); assert(res);
  }
 }
 finalized_ = 1;
}


TensorNetwork & TensorNetwork::operator=(const TensorNetwork & another)
{
 explicit_output_ = 1;
 finalized_ = 0;
 name_ = another.getName();
 max_tensor_id_ = 0;
 contraction_seq_flops_ = 0.0;

 auto output_tensor = another.getTensor(0);
 const auto & output_legs = *(another.getTensorConnections(0));
 auto new_output_tensor = makeSharedTensor(*output_tensor);
 new_output_tensor->rename(generateTensorName(*new_output_tensor));
 auto res = emplaceTensorConnDirect(false,
                                    0U, //output tensor (id = 0)
                                    new_output_tensor,0U,output_legs);
 assert(res);
 for(auto iter = another.cbegin(); iter != another.cend(); ++iter){
  if(iter->first != 0){ //only input tensors
   res = emplaceTensorConn(iter->first,iter->second); assert(res);
  }
 }
 finalized_ = 1;
 return *this;
}
*/

void TensorNetwork::printIt() const
{
 std::cout << "TensorNetwork(" << name_
           << ")[rank = " << this->getRank()
           << ", size = " << this->getNumTensors() << "]{" << std::endl;
 for(const auto & kv: tensors_){
  std::cout << " ";
  kv.second.printIt();
 }
 std::cout << "}" << std::endl;
 return;
}


bool TensorNetwork::isEmpty() const
{
 return (tensors_.size() <= 1); //only output tensor exists => still empty
}


bool TensorNetwork::isExplicit() const
{
 return (explicit_output_ != 0);
}


bool TensorNetwork::isFinalized() const
{
 return (finalized_ != 0);
}


bool TensorNetwork::isValid()
{
 return checkConnections();
}


unsigned int TensorNetwork::getRank() const
{
 assert(this->isFinalized());
 return tensors_.at(0).getNumLegs(); //output tensor
}


unsigned int TensorNetwork::getNumTensors() const
{
 return static_cast<unsigned int>(tensors_.size() - 1); //output tensor is not counted
}


unsigned int TensorNetwork::getMaxTensorId()
{
 if(max_tensor_id_ == 0){
  for(const auto & kv: tensors_) max_tensor_id_ = std::max(max_tensor_id_,kv.first);
 }
 return max_tensor_id_;
}


void TensorNetwork::updateMaxTensorIdOnAppend(unsigned int tensor_id)
{
 auto curr_max_id = getMaxTensorId();
 max_tensor_id_ = std::max(curr_max_id,tensor_id);
 return;
}


void TensorNetwork::updateMaxTensorIdOnRemove(unsigned int tensor_id)
{
 if(tensor_id != 0 && tensor_id == max_tensor_id_){
  max_tensor_id_ = 0; //reset max tensor id to Undefined
  //auto refresh_max_tensor_id = getMaxTensorId();
 }
 return;
}


void TensorNetwork::resetOutputTensor(const std::string & name)
{
 assert(finalized_ != 0);
 auto iter = tensors_.find(0);
 assert(iter != tensors_.end());
 iter->second.replaceStoredTensor(name);
 return;
}


void TensorNetwork::resetOutputTensor(const std::vector<unsigned int> & order,
                                      const std::string & name)
{
 assert(finalized_ != 0);
 auto iter = tensors_.find(0);
 assert(iter != tensors_.end());
 iter->second.replaceStoredTensor(order,name);
 return;
}


const std::string & TensorNetwork::getName() const
{
 return name_;
}


void TensorNetwork::rename(const std::string & name)
{
 assert(finalized_ != 0);
 resetOutputTensor();
 name_ = name;
 return;
}


TensorConn * TensorNetwork::getTensorConn(unsigned int tensor_id)
{
 auto it = tensors_.find(tensor_id);
 if(it == tensors_.end()) return nullptr;
 return &(it->second);
}


std::vector<TensorConn*> TensorNetwork::getTensorConnAll()
{
 std::vector<TensorConn*> tensors(this->getNumTensors(),nullptr);
 unsigned int i = 0;
 for(auto & kv: tensors_){
  if(kv.first != 0) tensors[i++] = &(kv.second);
 }
 return tensors;
}


std::shared_ptr<Tensor> TensorNetwork::getTensor(unsigned int tensor_id, bool * conjugated) const
{
 auto it = tensors_.find(tensor_id);
 if(it == tensors_.end()) return std::shared_ptr<Tensor>(nullptr);
 if(conjugated != nullptr) *conjugated = (it->second).isComplexConjugated();
 return (it->second).getTensor();
}


const std::vector<TensorLeg> * TensorNetwork::getTensorConnections(unsigned int tensor_id) const
{
 auto it = tensors_.find(tensor_id);
 if(it == tensors_.end()) return nullptr;
 return &((it->second).getTensorLegs());
}


bool TensorNetwork::finalize(bool check_validity)
{
 if(finalized_ == 0){
  if(this->isEmpty()){ //empty networks cannot be finalized
   std::cout << "#ERROR(TensorNetwork::finalize): Empty tensor network cannot be finalized!" << std::endl;
   return false;
  }
  finalized_ = 1;
  if(check_validity){
   if(!checkConnections()){
    finalized_ = 0;
    std::cout << "#ERROR(TensorNetwork::finalize): Invalid connectivity prevents tensor network finalization!" << std::endl;
    return false;
   }
  }
 }
 return true;
}


bool TensorNetwork::checkConnections(unsigned int tensor_id)
{
 assert(finalized_ != 0); //tensor network must be in finalized state
 auto * tensor = this->getTensorConn(tensor_id);
 assert(tensor != nullptr); //invalid tensor_id
 auto tensor_rank = tensor->getNumLegs();
 for(unsigned int i = 0; i < tensor_rank; ++i){
  const auto & tensor_leg = tensor->getTensorLeg(i);
  auto other_tensor_id = tensor_leg.getTensorId();
  auto other_tensor_leg_id = tensor_leg.getDimensionId();
  auto * other_tensor = this->getTensorConn(other_tensor_id);
  assert(other_tensor != nullptr); //unable to find the linked tensor
  const auto & other_tensor_leg = other_tensor->getTensorLeg(other_tensor_leg_id);
  if(other_tensor_leg.getTensorId() != tensor_id ||
     other_tensor_leg.getDimensionId() != i ||
     other_tensor_leg.getDirection() != reverseLegDirection(tensor_leg.getDirection())) return false;
 }
 return true;
}


bool TensorNetwork::checkConnections()
{
 assert(finalized_ != 0); //tensor network must be in finalized state
 for(const auto & kv: tensors_){
  if(!checkConnections(kv.first)) return false;
 }
 return true;
}


void TensorNetwork::updateConnections(unsigned int tensor_id)
{
 assert(finalized_ != 0); //tensor network must be in finalized state
 auto * tensor = this->getTensorConn(tensor_id);
 assert(tensor != nullptr); //invalid tensor_id
 auto tensor_rank = tensor->getNumLegs();
 for(unsigned int i = 0; i < tensor_rank; ++i){
  const auto & tensor_leg = tensor->getTensorLeg(i);
  auto other_tensor_id = tensor_leg.getTensorId();
  auto other_tensor_leg_id = tensor_leg.getDimensionId();
  auto * other_tensor = this->getTensorConn(other_tensor_id);
  assert(other_tensor != nullptr); //unable to find the linked tensor
  auto other_tensor_leg = other_tensor->getTensorLeg(other_tensor_leg_id);
  other_tensor_leg.resetTensorId(tensor_id);
  other_tensor_leg.resetDimensionId(i);
  other_tensor->resetLeg(other_tensor_leg_id,other_tensor_leg);
 }
 return;
}


void TensorNetwork::updateConnectionsFromInputTensors()
{
 for(auto iter = this->cbegin(); iter != this->cend(); ++iter){
  if(iter->first != 0) updateConnections(iter->first);
 }
 return;
}


void TensorNetwork::invalidateMaxTensorId()
{
 max_tensor_id_ = 0;
 return;
}


void TensorNetwork::invalidateContractionSequence()
{
 operations_.clear();
 contraction_seq_.clear();
 contraction_seq_flops_ = 0.0;
 return;
}


double TensorNetwork::determineContractionSequence(ContractionSeqOptimizer & contr_seq_optimizer)
{
 assert(finalized_ != 0); //tensor network must be in finalized state
 if(contraction_seq_.empty()){
  auto intermediate_num_begin = this->getMaxTensorId() + 1;
  auto intermediate_num_generator = [intermediate_num_begin]() mutable {return intermediate_num_begin++;};
  contraction_seq_flops_ = contr_seq_optimizer.determineContractionSequence(*this,contraction_seq_,intermediate_num_generator);
 }
 return contraction_seq_flops_;
}


void TensorNetwork::importContractionSequence(const std::list<ContrTriple> & contr_sequence)
{
 assert(finalized_ != 0); //tensor network must be in finalized state
 contraction_seq_.clear();
 contraction_seq_ = contr_sequence;
 contraction_seq_flops_ = 0.0; //flop count is unknown yet
 return;
}


const std::list<ContrTriple> & TensorNetwork::exportContractionSequence() const
{
 return contraction_seq_;
}


bool TensorNetwork::placeTensor(unsigned int tensor_id,                     //in: tensor id (unique within the tensor network)
                                std::shared_ptr<Tensor> tensor,             //in: appended tensor
                                const std::vector<TensorLeg> & connections, //in: tensor connections (fully specified)
                                bool conjugated,                            //in: complex conjugation flag for the appended tensor
                                bool leg_matching_check)                    //in: tensor leg matching check
{
 if(explicit_output_ == 0){
  std::cout << "#ERROR(TensorNetwork::placeTensor): Invalid request: " <<
   "Appending a tensor via explicit connections to the tensor network that is missing a full output tensor!" << std::endl;
  return false;
 }
 if(finalized_ != 0){
  std::cout << "#ERROR(TensorNetwork::placeTensor): Invalid request: " <<
   "Appending a tensor via explicit connections to the tensor network that has been finalized!" << std::endl;
  return false;
 }
 if(tensor_id == 0){
  std::cout << "#ERROR(TensorNetwork::placeTensor): Invalid request: " <<
   "Attempt to append an output tensor (id = 0) to a tensor network with an explicit output tensor!" << std::endl;
  return false;
 }
 //Check the validity of new connections:
 if(leg_matching_check){
  unsigned int mode = 0;
  for(const auto & leg: connections){
   const auto * tensconn = this->getTensorConn(leg.getTensorId());
   if(tensconn != nullptr){ //connected tensor is already in the tensor network
    const auto & tens_legs = tensconn->getTensorLegs();
    const auto & tens_leg = tens_legs[leg.getDimensionId()];
    if(tens_leg.getTensorId() != tensor_id || tens_leg.getDimensionId() != mode){
     std::cout << "#ERROR(TensorNetwork::placeTensor): Invalid argument: Connections are invalid: "
               << "Failed input leg: "; leg.printIt(); std::cout << std::endl;
     return false;
    }
   }
   ++mode;
  }
 }
 //Append the tensor to the tensor network:
 auto res = emplaceTensorConnDirect(true,
                                    tensor_id,
                                    tensor,tensor_id,connections,conjugated);
 if(!res){
  std::cout << "#ERROR(TensorNetwork::placeTensor): Invalid request: " <<
   "A tensor with id " << tensor_id << " already exists in the tensor network!" << std::endl;
  return false;
 }
 return true;
}


bool TensorNetwork::appendTensor(unsigned int tensor_id,                                             //in: tensor id (unique within the tensor network)
                                 std::shared_ptr<Tensor> tensor,                                     //in: appended tensor
                                 const std::vector<std::pair<unsigned int, unsigned int>> & pairing, //in: leg pairing: output tensor mode -> appended tensor mode
                                 const std::vector<LegDirection> & leg_dir,                          //in: optional leg direction (for all tensor modes)
                                 bool conjugated)                                                    //in: complex conjugation flag for the appended tensor
{
 if(explicit_output_ != 0 && finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid request: " <<
   "Appending a tensor via implicit pairing with the output tensor, but the tensor network is not finalized!" << std::endl;
  return false;
 }
 if(tensor_id == 0){
  std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid request: " <<
   "Attempt to append an output tensor (id = 0) to a finalized tensor network!" << std::endl;
  return false;
 }
 //Reset the output tensor to a new one:
 this->resetOutputTensor();
 //Check validity of leg pairing:
 auto tensor_rank = tensor->getRank();
 bool dir_present = (leg_dir.size() > 0);
 if(dir_present && leg_dir.size() != tensor_rank){
  std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid argument: Incomplete vector of leg directions!" << std::endl;
  return false;
 }
 auto * output_tensor = this->getTensorConn(0);
 assert(output_tensor != nullptr); //output tensor must be present
 auto output_rank = output_tensor->getNumLegs();
 if(output_rank > 0 && tensor_rank > 0){
  int ouf[output_rank] = {0};
  int tef[tensor_rank] = {0};
  for(const auto & link: pairing){
   if(link.first >= output_rank || link.second >= tensor_rank){
    std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid argument: Invalid leg pairing!" << std::endl;
    return false;
   }
   if(ouf[link.first]++ != 0){
    std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid argument: Pairing: Repeated output leg!" << std::endl;
    return false;
   }
   if(tef[link.second]++ != 0){
    std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid argument: Pairing: Repeated new tensor leg!" << std::endl;
    return false;
   }
  }
 }else{
  if(pairing.size() > 0){
   std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid argument: Pairing: Pairing on a scalar tensor!" << std::endl;
   return false;
  }
 }
 //Pair legs of the new tensor with the input tensors of the tensor network:
 if(tensor_rank > 0){ //true tensor
  std::vector<TensorLeg> new_tensor_legs(tensor_rank,TensorLeg(0,0)); //placeholders for legs
  if(pairing.size() > 0){
   std::vector<unsigned int> matched_output_legs(pairing.size(),0);
   unsigned int mode = 0;
   for(const auto & link: pairing){
    const auto & output_tensor_leg_id = link.first;
    const auto & tensor_leg_id = link.second;
    auto output_tensor_leg = output_tensor->getTensorLeg(output_tensor_leg_id);
    const auto input_tensor_id = output_tensor_leg.getTensorId();
    const auto input_tensor_leg_id = output_tensor_leg.getDimensionId();
    auto * input_tensor = this->getTensorConn(input_tensor_id);
    assert(input_tensor != nullptr);
    auto input_tensor_leg = input_tensor->getTensorLeg(input_tensor_leg_id);
    input_tensor_leg.resetTensorId(tensor_id);
    input_tensor_leg.resetDimensionId(tensor_leg_id);
    input_tensor->resetLeg(input_tensor_leg_id,input_tensor_leg);
    new_tensor_legs[tensor_leg_id].resetTensorId(input_tensor_id);
    new_tensor_legs[tensor_leg_id].resetDimensionId(input_tensor_leg_id);
    if(dir_present){
     new_tensor_legs[tensor_leg_id].resetDirection(leg_dir[tensor_leg_id]);
     if(input_tensor_leg.getDirection() != reverseLegDirection(new_tensor_legs[tensor_leg_id].getDirection())){
      std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid argument: Leg directions: Pairing leg direction mismatch!" << std::endl;
      return false;
     }
    }else{
     new_tensor_legs[tensor_leg_id].resetDirection(reverseLegDirection(input_tensor_leg.getDirection()));
    }
    matched_output_legs[mode++] = output_tensor_leg_id;
   }
   //Delete matched legs from the output tensor:
   output_tensor->deleteLegs(matched_output_legs);
   updateConnections(0); //update tensor network connections due to deletion of the matched output tensor legs
  }
  //Append unpaired legs of the new tensor to the output tensor of the network:
  output_rank = output_tensor->getNumLegs();
  unsigned int mode = 0;
  for(auto & leg: new_tensor_legs){
   if(leg.getTensorId() == 0){ //unpaired tensor leg
    LegDirection dir = LegDirection::UNDIRECT;
    if(dir_present) dir = leg_dir[mode];
    leg.resetDimensionId(output_rank);
    leg.resetDirection(dir);
    output_tensor->appendLeg(tensor->getDimSpaceAttr(mode),tensor->getDimExtent(mode),
                             TensorLeg(tensor_id,mode,reverseLegDirection(dir)));
    output_rank = output_tensor->getNumLegs();
   }
   ++mode;
  }
  auto res = emplaceTensorConnDirect(true,
                                     tensor_id,
                                     tensor,tensor_id,new_tensor_legs,conjugated);
  if(!res){
   std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid request: " <<
    "A tensor with id " << tensor_id << " already exists in the tensor network!" << std::endl;
   return false;
  }
 }else{ //scalar tensor
  auto res = emplaceTensorConnDirect(true,
                                     tensor_id,
                                     tensor,tensor_id,std::vector<TensorLeg>{},conjugated);
  if(!res){
   std::cout << "#ERROR(TensorNetwork::appendTensor): Invalid request: " <<
    "A tensor with id " << tensor_id << " already exists in the tensor network!" << std::endl;
   return false;
  }
 }
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 finalized_ = 1; //implicit leg pairing always keeps the tensor network in a finalized state
 return true;
}


bool TensorNetwork::appendTensorGate(unsigned int tensor_id,
                                     std::shared_ptr<Tensor> tensor,
                                     const std::vector<unsigned int> & pairing,
                                     bool conjugated)
{
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid request: " <<
   "Appending a tensor gate to an unfinalized tensor network is forbidden!" << std::endl;
  return false;
 }
 if(tensor_id == 0){
  std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid request: " <<
   "Tensor 0 (output tensor) must already be present in the tensor network!" << std::endl;
  return false;
 }
 //Reset the output tensor to a new one:
 this->resetOutputTensor();
 //Check validity of leg pairing:
 auto * output_tensor = this->getTensorConn(0);
 assert(output_tensor != nullptr); //output tensor must be present
 auto output_rank = output_tensor->getNumLegs();
 auto tensor_rank = tensor->getRank();
 if((tensor_rank % 2) != 0){
  std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid argument: Odd-rank tensors are not allowed as gates!" << std::endl;
  return false;
 }
 if(tensor_rank != pairing.size() * 2){
  std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid argument: Wrong size of the leg pairing vector!" << std::endl;
  return false;
 }
 if(tensor_rank > output_rank * 2){
  std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid argument: Tensor network does not have enough open legs!" << std::endl;
  return false;
 }
 if(output_rank > 0){
  char inds[output_rank] = {0};
  for(const auto & leg_id: pairing){
   if(leg_id >= output_rank){
    std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid argument: Invalid content of the pairing vector!" << std::endl;
    return false;
   }
   if(inds[leg_id]++ != 0){
    std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid argument: Invalid content of the pairing vector!" << std::endl;
    return false;
   }
  }
 }
 //Pair legs of the new tensor with the input tensors of the tensor network:
 if(tensor_rank > 0){
  std::vector<TensorLeg> new_tensor_legs(tensor_rank,TensorLeg(0,0)); //placeholders for legs
  unsigned int paired_leg_id = 0;
  unsigned int unpaired_leg_id = tensor_rank / 2;
  if(conjugated) std::swap(paired_leg_id,unpaired_leg_id);
  for(const auto & output_tensor_leg_id: pairing){
   auto output_tensor_leg = output_tensor->getTensorLeg(output_tensor_leg_id);
   //Relink the input tensor with the new tensor:
   const auto input_tensor_id = output_tensor_leg.getTensorId();
   const auto input_tensor_leg_id = output_tensor_leg.getDimensionId();
   auto * input_tensor = this->getTensorConn(input_tensor_id);
   assert(input_tensor != nullptr);
   auto input_tensor_leg = input_tensor->getTensorLeg(input_tensor_leg_id);
   input_tensor_leg.resetTensorId(tensor_id);
   input_tensor_leg.resetDimensionId(paired_leg_id);
   input_tensor->resetLeg(input_tensor_leg_id,input_tensor_leg);
   new_tensor_legs[paired_leg_id].resetTensorId(input_tensor_id);
   new_tensor_legs[paired_leg_id].resetDimensionId(input_tensor_leg_id);
   new_tensor_legs[paired_leg_id].resetDirection(reverseLegDirection(input_tensor_leg.getDirection()));
   //Relink the output tensor leg with the new tensor:
   output_tensor_leg.resetTensorId(tensor_id);
   output_tensor_leg.resetDimensionId(unpaired_leg_id);
   output_tensor->resetLeg(output_tensor_leg_id,output_tensor_leg);
   new_tensor_legs[unpaired_leg_id].resetTensorId(0);
   new_tensor_legs[unpaired_leg_id].resetDimensionId(output_tensor_leg_id);
   new_tensor_legs[unpaired_leg_id].resetDirection(reverseLegDirection(output_tensor_leg.getDirection()));
   ++paired_leg_id; ++unpaired_leg_id;
  }
  auto res = emplaceTensorConnDirect(true,
                                     tensor_id,
                                     tensor,tensor_id,new_tensor_legs,conjugated);
  if(!res){
   std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid request: " <<
    "A tensor with id " << tensor_id << " already exists in the tensor network!" << std::endl;
   return false;
  }
 }else{ //scalar tensor
  auto res = emplaceTensorConnDirect(true,
                                     tensor_id,
                                     tensor,tensor_id,std::vector<TensorLeg>{},conjugated);
  if(!res){
   std::cout << "#ERROR(TensorNetwork::appendTensorGate): Invalid request: " <<
    "A tensor with id " << tensor_id << " already exists in the tensor network!" << std::endl;
   return false;
  }
 }
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 finalized_ = 1; //implicit leg pairing always keeps the tensor network in a finalized state
 return true;
}


bool TensorNetwork::appendTensorNetwork(TensorNetwork && network,                                           //in: appended tensor network
                                        const std::vector<std::pair<unsigned int, unsigned int>> & pairing) //in: leg pairing: output tensor mode (primary) -> output tensor mode (appended)
{
 if(!((*this).isFinalized()) || !(network.isFinalized())){
  std::cout << "#ERROR(TensorNetwork::appendTensorNetwork): Invalid request: " <<
   "Either primary or appended tensor network is not finalized!" << std::endl;
  return false;
 }
 //Reset the output tensor to a new one:
 this->resetOutputTensor();
 network.resetOutputTensor();
 //Check validity of leg pairing:
 auto * output0 = this->getTensorConn(0);
 assert(output0 != nullptr);
 auto output0_rank = output0->getNumLegs();
 auto * output1 = network.getTensorConn(0);
 assert(output1 != nullptr);
 auto output1_rank = output1->getNumLegs();
 if(output0_rank > 0 && output1_rank > 0){
  int ou0[output0_rank] = {0};
  int ou1[output1_rank] = {0};
  for(const auto & link: pairing){
   if(link.first >= output0_rank || link.second >= output1_rank){
    std::cout << "#ERROR(TensorNetwork::appendTensorNetwork): Invalid argument: Pairing: Out of bounds!" << std::endl;
    return false;
   }
   if(ou0[link.first]++ != 0){
    std::cout << "#ERROR(TensorNetwork::appendTensorNetwork): Invalid argument: Pairing: Repeated primary output leg!" << std::endl;
    return false;
   }
   if(ou1[link.second]++ != 0){
    std::cout << "#ERROR(TensorNetwork::appendTensorNetwork): Invalid argument: Pairing: Repeated secondary output leg!" << std::endl;
    return false;
   }
  }
 }else{
  if(pairing.size() > 0){
   std::cout << "#ERROR(TensorNetwork::appendTensorNetwork): Invalid argument: Pairing: Non-trivial pairing on scalar networks!" << std::endl;
   return false;
  }
 }
 //Shift input tensor numeration in all internal legs of the appended tensor network:
 auto max_tensor_id = this->getMaxTensorId(); assert(max_tensor_id > 0);
 for(auto tensor_conn_iter = network.begin(); tensor_conn_iter != network.end(); ++tensor_conn_iter){
  if(tensor_conn_iter->first != 0){
   auto & tensor_conn = tensor_conn_iter->second;
   const auto tensor_conn_rank = tensor_conn.getNumLegs();
   for(unsigned int i = 0; i < tensor_conn_rank; ++i){
    TensorLeg new_leg = tensor_conn.getTensorLeg(i);
    const auto conn_tensor_id = new_leg.getTensorId();
    if(conn_tensor_id != 0){
     new_leg.resetTensorId(conn_tensor_id+max_tensor_id);
     tensor_conn.resetLeg(i,new_leg);
    }
   }
  }
 }
 //Pair output legs of the primary tensor network with output legs of the appended (secondary) tensor network:
 if(pairing.size() > 0){
  for(const auto & link: pairing){
   const auto & output0_leg_id = link.first;
   const auto & output1_leg_id = link.second;
   const auto & output0_leg = output0->getTensorLeg(output0_leg_id);
   const auto & output1_leg = output1->getTensorLeg(output1_leg_id);
   const auto input0_id = output0_leg.getTensorId();
   const auto input0_leg_id = output0_leg.getDimensionId();
   const auto input1_id = output1_leg.getTensorId();
   const auto input1_leg_id = output1_leg.getDimensionId();
   auto * input0 = this->getTensorConn(input0_id);
   assert(input0 != nullptr);
   auto * input1 = network.getTensorConn(input1_id);
   assert(input1 != nullptr);
   auto input0_leg = input0->getTensorLeg(input0_leg_id);
   input0_leg.resetTensorId(input1_id+max_tensor_id); //shift other network's tensor ids
   input0_leg.resetDimensionId(input1_leg_id);
   input0->resetLeg(input0_leg_id,input0_leg);
   auto input1_leg = input1->getTensorLeg(input1_leg_id);
   input1_leg.resetTensorId(input0_id);
   input1_leg.resetDimensionId(input0_leg_id);
   input1->resetLeg(input1_leg_id,input1_leg);
  }
  //Delete matched legs from both output tensors:
  std::vector<unsigned int> matched_output_legs(pairing.size(),0);
  for(unsigned int i = 0; i < pairing.size(); ++i) matched_output_legs[i] = pairing[i].first;
  output0->deleteLegs(matched_output_legs);
  this->updateConnections(0);
  for(unsigned int i = 0; i < pairing.size(); ++i) matched_output_legs[i] = pairing[i].second;
  output1->deleteLegs(matched_output_legs);
  network.updateConnections(0);
 }
 //Append unmatched legs of the output tensor from the appended tensor network to the output tensor from the primary tensor network:
 output0_rank = output0->getNumLegs();
 output1_rank = output1->getNumLegs();
 for(unsigned int i = 0; i < output1_rank; ++i){
  TensorLeg out1_leg(output1->getTensorLeg(i));
  out1_leg.resetTensorId(out1_leg.getTensorId()+max_tensor_id);
  output0->appendLeg(output1->getDimSpaceAttr(i),
                     output1->getDimExtent(i),
                     out1_leg);
 }
 output0_rank = output0->getNumLegs();
 //Append all input tensors from the appended tensor network into the primary tensor network:
 auto tensors = network.getTensorConnAll();
 for(auto & tensor: tensors){
  unsigned int tensor_id = tensor->getTensorId() + max_tensor_id;
  auto res = emplaceTensorConn(false,tensor_id,*tensor);
  if(!res){
   std::cout << "#ERROR(exatn::numerics::TensorNetwork::appendTensorNetwork): Tensor id already in use!" << std::endl;
   return false;
  }
 }
 this->updateConnections(0); //update connections in just appended input tensors
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 finalized_ = 1; //implicit leg pairing always keeps the primary tensor network in a finalized state
 return true;
}


bool TensorNetwork::appendTensorNetworkGate(TensorNetwork && network,
                                            const std::vector<unsigned int> & pairing)
{
 if(!((*this).isFinalized()) || !(network.isFinalized())){
  std::cout << "#ERROR(TensorNetwork::appendTensorNetworkGate): Invalid request: " <<
   "Either primary or appended tensor network is not finalized!" << std::endl;
  return false;
 }
 //Reset the output tensor to a new one:
 this->resetOutputTensor();
 network.resetOutputTensor();
 //Check validity of leg pairing:
 auto * output0 = this->getTensorConn(0);
 assert(output0 != nullptr);
 auto output0_rank = output0->getNumLegs();
 auto * output1 = network.getTensorConn(0);
 assert(output1 != nullptr);
 auto output1_rank = output1->getNumLegs();
 if(output1_rank % 2 != 0){
  std::cout << "#ERROR(TensorNetwork::appendTensorNetworkGate): Invalid argument: Odd-rank tensor networks are not allowed as gates!"
            << std::endl;
  return false;
 }
 if(output1_rank != pairing.size() * 2){
  std::cout << "#ERROR(TensorNetwork::appendTensorNetworkGate): Invalid argument: Wrong size of the leg pairing vector!"
            << std::endl;
  return false;
 }
 if(output1_rank > output0_rank * 2){
  std::cout << "#ERROR(TensorNetwork::appendTensorNetworkGate): Invalid argument: Primary tensor network does not have enough open legs!"
            << std::endl;
  return false;
 }
 if(output0_rank > 0){
  char inds[output0_rank] = {0};
  for(const auto & leg_id: pairing){
   if(leg_id >= output0_rank){
    std::cout << "#ERROR(TensorNetwork::appendTensorNetworkGate): Invalid argument: Invalid content of the pairing vector!" << std::endl;
    return false;
   }
   if(inds[leg_id]++ != 0){
    std::cout << "#ERROR(TensorNetwork::appendTensorNetworkGate): Invalid argument: Invalid content of the pairing vector!" << std::endl;
    return false;
   }
  }
 }
 //Shift input tensor numeration in all internal legs of the appended tensor network:
 auto max_tensor_id = this->getMaxTensorId(); assert(max_tensor_id > 0);
 for(auto tensor_conn_iter = network.begin(); tensor_conn_iter != network.end(); ++tensor_conn_iter){
  if(tensor_conn_iter->first != 0){
   auto & tensor_conn = tensor_conn_iter->second;
   const auto tensor_conn_rank = tensor_conn.getNumLegs();
   for(unsigned int i = 0; i < tensor_conn_rank; ++i){
    TensorLeg new_leg = tensor_conn.getTensorLeg(i);
    const auto conn_tensor_id = new_leg.getTensorId();
    if(conn_tensor_id != 0){
     new_leg.resetTensorId(conn_tensor_id+max_tensor_id);
     tensor_conn.resetLeg(i,new_leg);
    }
   }
  }
 }
 //Pair output legs of the primary tensor network with the output legs of the appended tensor network:
 if(pairing.size() > 0){
  unsigned int output1_leg_id = 0;
  unsigned int output1_replace_leg_id = output1_rank / 2;
  for(const auto & output0_leg_id: pairing){
   const auto & output0_leg = output0->getTensorLeg(output0_leg_id);
   const auto & output1_leg = output1->getTensorLeg(output1_leg_id);
   const auto input0_id = output0_leg.getTensorId();
   const auto input0_leg_id = output0_leg.getDimensionId();
   const auto input1_id = output1_leg.getTensorId();
   const auto input1_leg_id = output1_leg.getDimensionId();
   auto * input0 = this->getTensorConn(input0_id);
   assert(input0 != nullptr);
   auto * input1 = network.getTensorConn(input1_id);
   assert(input1 != nullptr);
   auto input0_leg = input0->getTensorLeg(input0_leg_id);
   input0_leg.resetTensorId(input1_id+max_tensor_id); //shift other network's tensor ids
   input0_leg.resetDimensionId(input1_leg_id);
   input0->resetLeg(input0_leg_id,input0_leg);
   auto input1_leg = input1->getTensorLeg(input1_leg_id);
   input1_leg.resetTensorId(input0_id);
   input1_leg.resetDimensionId(input0_leg_id);
   input1->resetLeg(input1_leg_id,input1_leg);
   TensorLeg output1_replace_leg(output1->getTensorLeg(output1_replace_leg_id));
   output1_replace_leg.resetTensorId(output1_replace_leg.getTensorId()+max_tensor_id);
   output0->resetLeg(output0_leg_id,output1_replace_leg);
   ++output1_leg_id; ++output1_replace_leg_id;
  }
  //Delete matched legs in the output tensor of the appended tensor network:
  std::vector<unsigned int> matched_legs(pairing.size(),0);
  for(unsigned int i = 0; i < pairing.size(); ++i) matched_legs[i] = i; //first half has been matched
  output1->deleteLegs(matched_legs);
  network.updateConnections(0);
 }
 //Append all input tensors from the appended tensor network into the primary tensor network:
 auto tensors = network.getTensorConnAll();
 for(auto & tensor: tensors){
  unsigned int tensor_id = tensor->getTensorId() + max_tensor_id;
  auto res = emplaceTensorConn(false,tensor_id,*tensor);
  if(!res){
   std::cout << "#ERROR(exatn::numerics::TensorNetwork::appendTensorNetworkGate): Tensor id already in use!" << std::endl;
   return false;
  }
 }
 this->updateConnections(0); //update connections in just appended input tensors
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 finalized_ = 1; //implicit leg pairing always keeps the primary tensor network in a finalized state
 return true;
}


bool TensorNetwork::reorderOutputModes(const std::vector<unsigned int> & order)
{
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::reorderOutputModes): Invalid request: " <<
   "Reordering modes in the output tensor of an unfinalized tensor network is forbidden!" << std::endl;
  return false;
 }
 const auto output_tensor_rank = this->getTensorConn(0)->getNumLegs();
 if(order.size() != output_tensor_rank){
  std::cout << "#ERROR(TensorNetwork::reorderOutputModes): Invalid argument: Dimension order: Wrong length: "
            << order.size() << " versus " << output_tensor_rank << std::endl;
  return false;
 }
 if(output_tensor_rank > 0){
  resetOutputTensor(order);
  updateConnections(0);
 }
 return true;
}


bool TensorNetwork::deleteTensor(unsigned int tensor_id)
{
 if(tensor_id == 0){
  std::cout << "#ERROR(TensorNetwork::deleteTensor): Invalid request: " <<
   "Deleting the output tensor of the tensor network is forbidden!" << std::endl;
  return false;
 }
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::deleteTensor): Invalid request: " <<
   "Deleting a tensor from an unfinalized tensor network is forbidden!" << std::endl;
  return false;
 }
 //Reset the output tensor to a new one:
 this->resetOutputTensor();
 //Append the released legs from the deleted tensor to the output tensor:
 auto * tensor = this->getTensorConn(tensor_id);
 if(tensor == nullptr){
  std::cout << "#ERROR(TensorNetwork::deleteTensor): Invalid request: " <<
   "Tensor with id " << tensor_id << " is not found in the tensor network!" << std::endl;
  return false;
 }
 //Reconnect the tensors connected to the deleted tensor to the output tensor:
 auto tensor_rank = tensor->getNumLegs();
 if(tensor_rank > 0){
  auto * output_tensor = this->getTensorConn(0);
  assert(output_tensor != nullptr);
  auto output_tensor_rank = output_tensor->getNumLegs();
  //Reconnect input tensors:
  std::vector<unsigned int> orphaned_legs;
  const auto & legs = tensor->getTensorLegs();
  for(const auto & leg: legs){
   const auto other_tensor_id = leg.getTensorId();
   const auto other_tensor_leg_id = leg.getDimensionId();
   if(other_tensor_id != 0){ //connections to the output tensor are ingored (they will disappear)
    auto * other_tensor = this->getTensorConn(other_tensor_id);
    assert(other_tensor != nullptr);
    auto other_tensor_leg = other_tensor->getTensorLeg(other_tensor_leg_id);
    other_tensor_leg.resetTensorId(0);
    other_tensor_leg.resetDimensionId(output_tensor_rank);
    other_tensor->resetLeg(other_tensor_leg_id,other_tensor_leg);
    output_tensor->appendLeg(other_tensor->getDimSpaceAttr(other_tensor_leg_id),
                             other_tensor->getDimExtent(other_tensor_leg_id),
                             TensorLeg(
                              other_tensor_id,
                              other_tensor_leg_id,
                              reverseLegDirection(other_tensor_leg.getDirection())
                             )
                            );
    output_tensor_rank = output_tensor->getNumLegs();
   }else{ //orphaned leg (former connection to the output tensor)
    orphaned_legs.emplace_back(other_tensor_leg_id);
   }
  }
  //Delete orphaned legs of the output tensor:
  if(orphaned_legs.size() > 0){
   output_tensor->deleteLegs(orphaned_legs);
   this->updateConnections(0);
  }
 }
 //Delete the tensor from the network:
 tensor = nullptr;
 auto erased = eraseTensorConn(tensor_id); assert(erased);
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 return true;
}


bool TensorNetwork::mergeTensors(unsigned int left_id, unsigned int right_id, unsigned int result_id,
                                 std::string * contr_pattern)
{
 if(left_id == right_id || left_id == result_id || right_id == result_id){
  std::cout << "#ERROR(TensorNetwork::mergeTensors): Invalid arguments: Cannot be identical: " <<
   left_id << " " << right_id << " " << result_id << std::endl;
  return false;
 }
 if(left_id == 0 || right_id == 0 || result_id == 0){
  std::cout << "#ERROR(TensorNetwork::mergeTensors): Invalid arguments: Output tensor #0 cannot participate: " <<
   left_id << " " << right_id << " " << result_id << std::endl;
  return false;
 }
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::mergeTensors): Invalid request: " <<
   "Merging tensors in an unfinalized tensor network is forbidden!" << std::endl;
  return false;
 }
 //Get tensor info:
 auto * left_tensor = this->getTensorConn(left_id);
 assert(left_tensor != nullptr);
 auto left_tensor_rank = left_tensor->getNumLegs();
 auto left_tensor_conj = left_tensor->isComplexConjugated();
 const auto & left_legs = left_tensor->getTensorLegs();
 auto * right_tensor = this->getTensorConn(right_id);
 assert(right_tensor != nullptr);
 auto right_tensor_rank = right_tensor->getNumLegs();
 auto right_tensor_conj = right_tensor->isComplexConjugated();
 const auto & right_legs = right_tensor->getTensorLegs();
 //Count contracted and uncontracted legs:
 unsigned int num_contracted = 0;
 for(const auto & leg: left_legs){if(leg.getTensorId() == right_id) ++num_contracted;}
 unsigned int num_uncontracted = (left_legs.size() + right_legs.size()) - num_contracted*2;
 //Create the resulting legs and contraction pattern:
 std::vector<TensorLeg> result_legs(num_uncontracted,TensorLeg(0,0)); //placeholders for result-tensor legs
 std::vector<TensorLeg> pattern(left_legs.size()+right_legs.size(),TensorLeg(0,0)); //tensor contraction pattern (placeholder)
 unsigned int mode = 0;
 unsigned int res_mode = 0;
 for(const auto & leg: left_legs){
  if(leg.getTensorId() == right_id){ //contracted leg
   pattern[mode++] = TensorLeg(2,leg.getDimensionId());
  }else{ //uncontracted leg
   pattern[mode++] = TensorLeg(0,res_mode);
   result_legs[res_mode++] = leg;
  }
 }
 for(const auto & leg: right_legs){
  if(leg.getTensorId() == left_id){ //contracted leg
   pattern[mode++] = TensorLeg(1,leg.getDimensionId());
  }else{ //uncontracted leg
   pattern[mode++] = TensorLeg(0,res_mode);
   result_legs[res_mode++] = leg;
  }
 }
 assert(res_mode == num_uncontracted);
 //Generate symbolic contraction pattern if needed:
 if(contr_pattern != nullptr){
  auto generated = generate_contraction_pattern(pattern,left_tensor_rank,right_tensor_rank,
                                                *contr_pattern,left_tensor_conj,right_tensor_conj);
  assert(generated);
 }
 //Append the tensor result:
 auto res = emplaceTensorConnDirect(true,
                                    result_id,
                                    std::make_shared<Tensor>(
                                     "_y" + std::to_string(result_id),
                                     *(left_tensor->getTensor()),
                                     *(right_tensor->getTensor()),
                                     pattern
                                    ),
                                    result_id,result_legs);
 if(!res){
  std::cout << "#ERROR(exatn::numerics::TensorNetwork::mergeTensors): Tensor id already in use!" << std::endl;
  return false;
 }
 //Delete two original tensors:
 auto erased = eraseTensorConn(left_id); assert(erased);
 erased = eraseTensorConn(right_id); assert(erased);
 //Update connections:
 this->updateConnections(result_id);
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 return true;
}


bool TensorNetwork::splitTensor(unsigned int tensor_id,
                                unsigned int left_tensor_id,
                                const std::string & left_tensor_name,
                                unsigned int right_tensor_id,
                                const std::string & right_tensor_name,
                                const TensorShape & contracted_dims,
                                const std::vector<int> & right_dims)
{
 //Check arguments:
 if(tensor_id == 0){
  std::cout << "#ERROR(TensorNetwork::splitTensor): Invalid request: " <<
   "Splitting the output tensor of the tensor network is forbidden!" << std::endl;
  return false;
 }
 if(left_tensor_id == 0 || right_tensor_id == 0 || left_tensor_id == right_tensor_id){
  std::cout << "#ERROR(TensorNetwork::splitTensor): Invalid request: " <<
   "Split tensors must acquire unique positive ids!" << std::endl;
  return false;
 }
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::splitTensor): Invalid request: " <<
   "Splitting a tensor in an unfinalized tensor network is forbidden!" << std::endl;
  return false;
 }
 auto * tensor = this->getTensorConn(tensor_id);
 assert(tensor != nullptr);
 const auto tensor_rank = tensor->getNumLegs();
 if(right_dims.size() != tensor_rank){
  std::cout << "#ERROR(TensorNetwork::splitTensor): Invalid request: " <<
   "The vector of tensor dimension split assignment has wrong size!" << std::endl;
  return false;
 }
 //Create new tensors from the original tensor:
 unsigned int left_rank = 0;
 unsigned int right_rank = 0;
 for(const auto & ass: right_dims) (ass == 0) ? ++left_rank : ++right_rank;
 unsigned int num_contr_dims = contracted_dims.getRank();
 unsigned int left_full_rank = left_rank + num_contr_dims;
 unsigned int right_full_rank = right_rank + num_contr_dims;
 auto left_tensor = tensor->getTensor()->createSubtensor(left_tensor_name,right_dims,0);
 assert(left_tensor);
 auto right_tensor = tensor->getTensor()->createSubtensor(right_tensor_name,right_dims,1);
 assert(right_tensor);
 std::vector<TensorLeg> left_legs(left_rank,TensorLeg(0,0));
 std::vector<TensorLeg> right_legs(right_rank,TensorLeg(0,0));
 for(unsigned int l = 0, r = 0, i = 0; i < tensor_rank; ++i){
  (right_dims[i] == 0) ? left_legs[l++] = tensor->getTensorLeg(i):
                         right_legs[r++] = tensor->getTensorLeg(i);
 }
 //Remove the original tensor from the tensor network:
 auto erased = eraseTensorConn(tensor_id); assert(erased);
 //Append the new derived tensors to the tensor network:
 auto res = emplaceTensorConnDirect(true,
                                    left_tensor_id,
                                    left_tensor,left_tensor_id,left_legs);
 if(!res){
  std::cout << "#ERROR(TensorNetwork::splitTensor): Invalid request: " <<
   "A tensor with id " << left_tensor_id << " already exists in the tensor network!" << std::endl;
  return false;
 }
 res = emplaceTensorConnDirect(true,
                               right_tensor_id,
                               right_tensor,right_tensor_id,right_legs);
 if(!res){
  std::cout << "#ERROR(TensorNetwork::splitTensor): Invalid request: " <<
   "A tensor with id " << right_tensor_id << " already exists in the tensor network!" << std::endl;
  return false;
 }
 updateConnections(left_tensor_id);
 updateConnections(right_tensor_id);
 //Append new (contracted) dimensions to both the left and right derived tensors:
 auto * left = this->getTensorConn(left_tensor_id); assert(left);
 auto * right = this->getTensorConn(right_tensor_id); assert(right);
 for(unsigned int i = 0; i < num_contr_dims; ++i){
  auto dim_extent = contracted_dims.getDimExtent(i);
  left->appendLeg(dim_extent,TensorLeg(right_tensor_id,right_rank++));
  right->appendLeg(dim_extent,TensorLeg(left_tensor_id,left_rank++));
 }
 assert(left_rank == left_full_rank && right_rank == right_full_rank);
 invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 return true;
}


bool TensorNetwork::substituteTensor(unsigned int tensor_id, std::shared_ptr<Tensor> tensor)
{
 assert(tensor);
 auto * old_tensor_conn = this->getTensorConn(tensor_id);
 if(old_tensor_conn == nullptr) return false;
 if(!(tensor->isCongruentTo(*(old_tensor_conn->getTensor())))) return false;
 old_tensor_conn->replaceStoredTensor(tensor);
 return true;
}


bool TensorNetwork::substituteTensor(const std::string & name, std::shared_ptr<Tensor> tensor)
{
 assert(name.length() > 0);
 int tensor_id = -1;
 for(const auto & kv: tensors_){
  if(kv.second.getName() == name){
   tensor_id = static_cast<int>(kv.first);
   break;
  }
 }
 if(tensor_id < 0) return false; //tensor name not found
 return substituteTensor(tensor_id,tensor);
}


std::vector<unsigned int> TensorNetwork::getTensorIdsInNetwork(const std::string & name, bool conjugated) const
{
 assert(name.length() > 0);
 std::vector<unsigned int> ids;
 for(const auto & kv: tensors_){
  if(kv.second.getName() == name &&
     kv.second.isComplexConjugated() == conjugated) ids.emplace_back(kv.first);
 }
 return ids;
}


bool TensorNetwork::conjugate()
{
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::conjugate): Invalid request: " <<
   "Unfinalized tensor network may not be conjugated!" << std::endl;
  return false;
 }
 for(auto iter = this->begin(); iter != this->end(); ++iter) (iter->second).conjugate();
 return true;
}


bool TensorNetwork::collapseIsometries()
{
 bool simplified = false;
 if(finalized_ == 0){
  std::cout << "#ERROR(TensorNetwork::collapseIsometries): Invalid request: " <<
   "Unfinalized tensor network may not be simplified!" << std::endl;
  return simplified;
 }
 auto another_collapse = true;
 while(another_collapse){
  another_collapse = false;
  auto iter = this->begin();
  while(!another_collapse && iter != this->end()){
   if(iter->first != 0){ //only input tensors can collapse
    auto & tensor = iter->second; //connected tensor pointed to by the iterator
    const auto tensor_id = tensor.getTensorId();
    const auto & tensor_name = tensor.getName();
    const auto & tensor_legs = tensor.getTensorLegs(); //legs of the connected tensor
    const auto & tensor_isometries = tensor.retrieveIsometries(); //isometries of the connected tensor
    for(const auto & iso_group: tensor_isometries){
     bool iso_match = !(iso_group.empty());
     unsigned int iso_matched_tensor_id = 0;
     for(const auto & dimsn: iso_group){
      const auto other_tensor_id = tensor_legs[dimsn].getTensorId();
      const auto other_tensor_dimsn = tensor_legs[dimsn].getDimensionId();
      if(other_tensor_dimsn == dimsn){
       auto & other_tensor = *(this->getTensorConn(other_tensor_id)); //other connected tensor
       if(other_tensor.getName() == tensor_name){
        if((tensor.isComplexConjugated() && !(other_tensor.isComplexConjugated())) ||
           (!(tensor.isComplexConjugated()) && other_tensor.isComplexConjugated())){
         if(iso_matched_tensor_id == 0) iso_matched_tensor_id = other_tensor_id;
         if(other_tensor_id != iso_matched_tensor_id){
          iso_match = false;
          break;
         }
        }else{
         iso_match = false;
         break;
        }
       }else{
        iso_match = false;
        break;
       }
      }else{
       iso_match = false;
       break;
      }
     }
     if(iso_match && iso_matched_tensor_id > 0){ // [Tensor---Isometric_group---Tensor+] detected
      auto & conj_tensor = *(this->getTensorConn(iso_matched_tensor_id)); //conjugate tensor
      const auto & conj_tensor_legs = conj_tensor.getTensorLegs();
      unsigned int num_iso_legs = 0;
      unsigned int num_open_legs = 0;
      unsigned int num_spectators = 0;
      for(unsigned int i = 0; i < tensor.getNumLegs(); ++i){
       if(tensor_legs[i].getTensorId() == iso_matched_tensor_id) num_iso_legs++;
       if(tensor_legs[i].getTensorId() == 0 || conj_tensor_legs[i].getTensorId() == 0) num_open_legs++;
       if(tensor_legs[i].getTensorId() == 0 && conj_tensor_legs[i].getTensorId() == 0) num_spectators++;
      }
      if(num_iso_legs == iso_group.size()){ //isometric tensor connection identified: Collapse
       if(num_spectators == 0){ //`Spectators (orphaned legs) are not yet supported
        if(num_open_legs > 0) this->resetOutputTensor(); //new open legs need to be appended to the output tensor
        for(unsigned int i = 0; i < tensor.getNumLegs(); ++i){
         auto first_tensor_id = tensor_legs[i].getTensorId();
         if(first_tensor_id != iso_matched_tensor_id){
          auto first_tensor_dimsn = tensor_legs[i].getDimensionId();
          auto second_tensor_id = conj_tensor_legs[i].getTensorId();
          assert(second_tensor_id != tensor_id);
          auto second_tensor_dimsn = conj_tensor_legs[i].getDimensionId();
          auto leg = this->getTensorConn(first_tensor_id)->getTensorLeg(first_tensor_dimsn);
          leg.resetTensorId(second_tensor_id);
          leg.resetDimensionId(second_tensor_dimsn);
          this->getTensorConn(first_tensor_id)->resetLeg(first_tensor_dimsn,leg);
          leg = this->getTensorConn(second_tensor_id)->getTensorLeg(second_tensor_dimsn);
          leg.resetTensorId(first_tensor_id);
          leg.resetDimensionId(first_tensor_dimsn);
          this->getTensorConn(second_tensor_id)->resetLeg(second_tensor_dimsn,leg);
         }
        }
        auto erased = eraseTensorConn(tensor_id); assert(erased);
        erased = eraseTensorConn(iso_matched_tensor_id); assert(erased);
        another_collapse = true;
        simplified = true;
       }
      }
     }
    }
   }
   ++iter;
  }
 }
 if(simplified) invalidateContractionSequence(); //invalidate previously cached tensor contraction sequence
 return simplified;
}


double TensorNetwork::getContractionCost(unsigned int left_id, unsigned int right_id,
                                         double * arithm_intensity, bool adjust_cost)
{
 double flops = 0.0, left_vol = 1.0, right_vol = 1.0, contr_vol = 1.0;
 if(left_id != 0 && right_id != 0){
  const auto * left_tensor = this->getTensorConn(left_id);
  assert(left_tensor != nullptr);
  const auto left_rank = left_tensor->getNumLegs();
  const auto * right_tensor = this->getTensorConn(right_id);
  assert(right_tensor != nullptr);
  const auto right_rank = right_tensor->getNumLegs();
  const auto & right_legs = right_tensor->getTensorLegs();
  for(unsigned int i = 0; i < left_rank; ++i){
   left_vol *= static_cast<double>(left_tensor->getDimExtent(i));
  }
  for(unsigned int i = 0; i < right_rank; ++i){
   double dim_ext = static_cast<double>(right_tensor->getDimExtent(i));
   if(right_legs[i].getTensorId() == left_id) contr_vol *= dim_ext; //contracted dimension
   right_vol *= dim_ext;
  }
  flops = left_vol * right_vol / contr_vol;
  if(arithm_intensity != nullptr) *arithm_intensity = flops / (left_vol + right_vol);
  if(adjust_cost){ //increase the "effective" flop count if arithmetic intensity is low
   //`Finish: flops *= f(arithm_intensity): [max --> 1]
  }
 }else{
  std::cout << "#ERROR(TensorNetwork::getContractionCost): Invalid request: " <<
   "The output tensor of the tensor network (tensor 0) cannot be contracted!" << std::endl;
  flops = -1.0; //error
 }
 return flops;
}


std::list<std::shared_ptr<TensorOperation>> & TensorNetwork::getOperationList(const std::string & contr_seq_opt_name)
{
 if(operations_.empty()){
  //Get the tensor contraction sequence optimizer:
  auto iter = optimizers.find(contr_seq_opt_name);
  if(iter == optimizers.end()){ //not cached
   auto & optimizer_factory = *(ContractionSeqOptimizerFactory::get());
   auto optimizer = optimizer_factory.createContractionSeqOptimizer(contr_seq_opt_name);
   if(optimizer){
    auto res = optimizers.emplace(std::make_pair(contr_seq_opt_name,
                                  std::shared_ptr<ContractionSeqOptimizer>(std::move(optimizer))));
    assert(res.second);
    iter = res.first;
   }else{
    std::cout << "#ERROR(TensorNetwork::getOperationList): Invalid request: " <<
     "Tensor contraction sequence optimizer" << contr_seq_opt_name << "has not been registered before!" << std::endl;
    assert(false);
   }
  }
  //Determine the pseudo-optimal sequence of tensor contractions:
  double flops = determineContractionSequence(*(iter->second));
  //Generate the list of operations:
  auto & tensor_op_factory = *(TensorOpFactory::get());
  if(this->getNumTensors() > 1){ //two or more input tensors: One or more contractions
   TensorNetwork net(*this);
   std::list<unsigned int> intermediates;
   unsigned int num_contractions = contraction_seq_.size();
   for(auto contr = contraction_seq_.cbegin(); contr != contraction_seq_.cend(); ++contr){
    //std::cout << "#DEBUG(TensorNetwork::getOperationList): Contracting " << contr->left_id << " * " << contr->right_id
    //          << " -> " << contr->result_id << std::endl; //debug
    bool conj1, conj2;
    auto tensor1 = net.getTensor(contr->left_id,&conj1);
    auto tensor2 = net.getTensor(contr->right_id,&conj2);
    std::string contr_pattern;
    if(num_contractions > 1){ //intermediate contraction
     auto merged = net.mergeTensors(contr->left_id,contr->right_id,contr->result_id,&contr_pattern);
     assert(merged);
    }else{ //last contraction
     assert(contr->result_id == 0); //last tensor contraction accumulates into the output tensor of the tensor network
     const auto * tensor1_legs = net.getTensorConnections(contr->left_id);
     assert(tensor1_legs != nullptr);
     const auto * tensor2_legs = net.getTensorConnections(contr->right_id);
     assert(tensor2_legs != nullptr);
     std::vector<TensorLeg> pattern(*tensor1_legs);
     pattern.insert(pattern.end(),tensor2_legs->begin(),tensor2_legs->end());
     auto generated = generate_contraction_pattern(pattern,tensor1_legs->size(),tensor2_legs->size(),
                                                   contr_pattern,conj1,conj2);
     assert(generated);
    }
    auto tensor0 = net.getTensor(contr->result_id);
    if(contr->result_id != 0){ //intermediate tensors need to be created/destroyed
     auto op_create = tensor_op_factory.createTensorOpShared(TensorOpCode::CREATE); //create intermediate
     op_create->setTensorOperand(tensor0);
     if(tensor0->getElementType() != TensorElementType::VOID)
      std::dynamic_pointer_cast<TensorOpCreate>(op_create)->resetTensorElementType(tensor0->getElementType());
     operations_.emplace_back(op_create);
     intermediates.emplace_back(contr->result_id);
     std::shared_ptr<TensorOperation> op_init(std::move(tensor_op_factory.createTensorOp(TensorOpCode::TRANSFORM))); //init intermediate to zero
     op_init->setTensorOperand(tensor0);
     std::dynamic_pointer_cast<TensorOpTransform>(op_init)->
          resetFunctor(std::shared_ptr<talsh::TensorFunctor<Identifiable>>(new FunctorInitVal(0.0)));
     operations_.emplace_back(op_init);
    }else{ //make sure the output tensor has its type set
     if(tensor0->getElementType() == TensorElementType::VOID) tensor0->setElementType(tensor1->getElementType());
    }
    auto op = tensor_op_factory.createTensorOp(TensorOpCode::CONTRACT);
    op->setTensorOperand(tensor0);
    op->setTensorOperand(tensor1,conj1);
    op->setTensorOperand(tensor2,conj2);
    op->setIndexPattern(contr_pattern);
    assert(op->isSet());
    operations_.emplace_back(std::shared_ptr<TensorOperation>(std::move(op)));
    auto left_intermediate = std::find(intermediates.begin(),intermediates.end(),contr->left_id);
    if(left_intermediate != intermediates.end()){
     auto op_destroy = tensor_op_factory.createTensorOp(TensorOpCode::DESTROY);
     op_destroy->setTensorOperand(tensor1);
     operations_.emplace_back(std::shared_ptr<TensorOperation>(std::move(op_destroy)));
     intermediates.erase(left_intermediate);
    }
    auto right_intermediate = std::find(intermediates.begin(),intermediates.end(),contr->right_id);
    if(right_intermediate != intermediates.end()){
     auto op_destroy = tensor_op_factory.createTensorOp(TensorOpCode::DESTROY);
     op_destroy->setTensorOperand(tensor2);
     operations_.emplace_back(std::shared_ptr<TensorOperation>(std::move(op_destroy)));
     intermediates.erase(right_intermediate);
    }
    --num_contractions;
   }
   assert(intermediates.empty());
  }else{ //one input tensor: Single addition
   std::shared_ptr<Tensor> tensor0(nullptr);
   std::shared_ptr<Tensor> tensor1(nullptr);
   bool conj1;
   unsigned int left_tensor_id = 0;
   for(auto iter = this->begin(); iter != this->end(); ++iter){
    if(iter->first == 0){
     tensor0 = this->getTensor(iter->first);
    }else{
     tensor1 = this->getTensor(iter->first,&conj1);
     left_tensor_id = iter->first;
    }
   }
   if(tensor0->getElementType() == TensorElementType::VOID) tensor0->setElementType(tensor1->getElementType());
   auto op = tensor_op_factory.createTensorOp(TensorOpCode::ADD);
   op->setTensorOperand(tensor0);
   op->setTensorOperand(tensor1,conj1);
   const auto * tensor1_legs = this->getTensorConnections(left_tensor_id);
   assert(tensor1_legs != nullptr);
   std::string contr_pattern;
   auto generated = generate_contraction_pattern(*tensor1_legs,tensor1_legs->size(),0,contr_pattern,conj1);
   assert(generated);
   op->setIndexPattern(contr_pattern);
   assert(op->isSet());
   operations_.emplace_back(std::shared_ptr<TensorOperation>(std::move(op)));
  }
 }
 return operations_;
}

} //namespace numerics

} //namespace exatn
