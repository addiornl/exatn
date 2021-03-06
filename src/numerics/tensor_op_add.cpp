/** ExaTN::Numerics: Tensor operation: Adds a tensor to another tensor
REVISION: 2019/08/30

Copyright (C) 2018-2019 Dmitry I. Lyakh (Liakh)
Copyright (C) 2018-2019 Oak Ridge National Laboratory (UT-Battelle) **/

#include "exatn_service.hpp"

#include "tensor_op_add.hpp"

#include "tensor_node_executor.hpp"

namespace exatn{

namespace numerics{

TensorOpAdd::TensorOpAdd():
 TensorOperation(TensorOpCode::ADD,2,1)
{
 this->setScalar(0,std::complex<double>{1.0,0.0}); //default alpha prefactor
}

bool TensorOpAdd::isSet() const
{
 return (this->getNumOperandsSet() == this->getNumOperands() && this->getIndexPattern().length() > 0);
}

int TensorOpAdd::accept(runtime::TensorNodeExecutor & node_executor,
                        runtime::TensorOpExecHandle * exec_handle)
{
 return node_executor.execute(*this,exec_handle);
}

std::unique_ptr<TensorOperation> TensorOpAdd::createNew()
{
 return std::unique_ptr<TensorOperation>(new TensorOpAdd());
}

} //namespace numerics

} //namespace exatn
