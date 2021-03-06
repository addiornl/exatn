#include "graph_executor_lazy.hpp"

namespace exatn {
namespace runtime {

void LazyGraphExecutor::execute(TensorGraph & dag) {
  auto num_nodes = dag.getNumNodes();
  decltype(num_nodes) nodes_executed = 0;
  while(nodes_executed <= num_nodes) {
    auto execnode_id = nextExecutableNodeId(dag);
    TensorOpExecHandle exec_handle;
    auto error_code = dag.getNodeProperties(execnode_id).getOperation()->accept(*node_executor_,&exec_handle);
    //TODO: update output tensor execution table
    dag.setNodeExecuted(execnode_id);
    nodes_executed++;
    num_nodes = dag.getNumNodes();
  }
}


VertexIdType LazyGraphExecutor::nextExecutableNodeId(TensorGraph & dag){
  auto num_nodes = dag.getNumNodes();
  VertexIdType vertex = num_nodes;
  for(VertexIdType i = 0; i < num_nodes; ++i) {
    if(!dag.nodeExecuted(i)) {
      if(dag.getNodeDegree(i) == 0){
        vertex = i;
        break;
      }else{
        auto n_list = dag.getNeighborList(i);
        for(const auto & vert: n_list){
          if(!dag.nodeExecuted(vert)) break;
        }
        vertex = i;
      }
    }
  }
  assert(vertex < num_nodes);
  return vertex;
}

} //namespace runtime
} //namespace exatn
