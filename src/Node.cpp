/*
 * Node.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: anthony
 */

#include "Node.h"

using namespace std;

std::ostream & Node::serialize( std::ostream & stream, const Node & node )
{
  stream.write((char*)&node.get_type(), sizeof(node.get_type()));
  const Node * node_ptr = &node;
  switch ( node.get_type() )
  {
    case LeafType:
      LeafNode::serialize_node(stream, *(reinterpret_cast<const LeafNode*>(node_ptr)));
      break;

    case SplitType:
      SplitNode::serialize_node(stream, *(reinterpret_cast<const SplitNode*>(node_ptr)));
      break;

    default:
      break;
  }
  return stream;
}

Node * Node::deserialize( std::istream & stream )
{
  // Read type.
  Type type;
  stream.read((char*)&type, sizeof(type));

  // Create node.
  Node * node = reinterpret_cast<Node*>(0);
  switch ( type )
  {
    case LeafType:
      LeafNode::deserialize_node(stream, node);
      break;

    case SplitType:
      SplitNode::deserialize_node(stream, node);
      break;

    default:
      break;
  }

  // Done.
  return node;
}
