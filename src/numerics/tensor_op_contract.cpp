/** ExaTN::Numerics: Tensor operation: Contracts two tensors and accumulates the result into another tensor
REVISION: 2019/08/30

Copyright (C) 2018-2019 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2019 Oak Ridge National Laboratory (UT-Battelle) **/

#include "exatn_service.hpp"

#include "tensor_op_contract.hpp"

#include "tensor_node_executor.hpp"

namespace exatn{

namespace numerics{

TensorOpContract::TensorOpContract():
 TensorOperation(TensorOpCode::CONTRACT,3,2)
{
 this->setScalar(0,std::complex<double>{1.0,0.0}); //default alpha prefactor
 this->setScalar(1,std::complex<double>{1.0,0.0}); //default beta prefactor (accumulative)
}

bool TensorOpContract::isSet() const
{
 return (this->getNumOperandsSet() == this->getNumOperands() && this->getIndexPattern().length() > 0);
}

int TensorOpContract::accept(runtime::TensorNodeExecutor & node_executor,
                             runtime::TensorOpExecHandle * exec_handle)
{
 return node_executor.execute(*this,exec_handle);
}

std::unique_ptr<TensorOperation> TensorOpContract::createNew()
{
 return std::unique_ptr<TensorOperation>(new TensorOpContract());
}

} //namespace numerics

} //namespace exatn
