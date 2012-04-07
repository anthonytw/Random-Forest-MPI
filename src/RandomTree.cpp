/*
 * RandomTree.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: anthony
 */

#include "RandomTree.h"
#include "Node.h"
#include <list>
#include <sstream>
#include <stdlib.h>

using namespace std;

//------------------------------------------------------------------------------

std::string RandomTree::draw( void ) const
{
  stringstream output;
  if (root != null(Node))
  {
    output << "graph G{\n";
    list<Node*> open_list;
    open_list.push_back(root);

    unsigned int node_id      = 0;
    unsigned int last_node_id = 0;
    while ( !open_list.empty() )
    {
      Node * node = open_list.front();

      // Draw node.
      output << "N" << node_id << "[" << node->draw() << "];\n";

      // Has children?
      if ( !node->is_leaf() )
      {
        Node::NodeSet children = node->get_children();
        for (
          Node::NodeSet::const_iterator iter = children.begin();
          iter != children.end(); ++iter )
        {
          Node * child = *iter;

          ++last_node_id;
          output << "N" << node_id << " -- N" << last_node_id << ";\n";
          open_list.push_back( child );
        }
      }

      // Pop element and increment ID.
      open_list.pop_front();
      ++node_id;
    }
    output << "}";
  }
  return output.str();
}

//------------------------------------------------------------------------------

void RandomTree::grow_decision_tree(
  Dataset & data,
  const Dataset::KeyList & split_keys,
  const unsigned int keys_per_node,
  const unsigned int decision_column)
{
  // If tree exists, DESTROY IT.
  burn();

  // There should be enough keys to split on.
  if ( split_keys.size() < keys_per_node )
  {
    return;
  }

  // Build the decision tree.
  _grow_decision_tree(
    data, split_keys, keys_per_node,
    decision_column, root, "<root>" );
}

//---------

void RandomTree::_grow_decision_tree(
  Dataset & data,
  const Dataset::KeyList & split_keys,
  const unsigned int keys_per_node,
  const unsigned int decision_column,
  Node * const parent,
  const std::string action )
{
/*
 *
Data keys: ['id', 'recency', 'frequency', 'monetary', 'time', 'donated']
Data size: 748 rows
5 - 0.791644629845
Split thresholds:  6.0 ,  4.0 ,  1000.0 ,  51.0 ,
Split stats:
  - Highest IG    :  0.0730344351
  - Highest IG Thr:  6.0
  - Highest IG Col:  recency  /  1
 */
  Dataset::ProbabilityMap pm_class =
    data.enumerate_threshold( decision_column, 0 );
  double entropy = pm_class.entropy();

  // Choose a random subset of keys.
  Dataset::KeyList sample_keys;
  while ( sample_keys.size() < keys_per_node )
  {
    unsigned int index = rand() % split_keys.size();
    Dataset::KeyList::const_iterator iter = split_keys.begin();
    unsigned int index_dec = index;
    while ( index_dec-- )
    {
      ++iter;
    }
    if ( sample_keys.find(iter->first) == sample_keys.end() )
    {
      sample_keys[iter->first] = iter->second;
    }
  }

  // Determine highest information gain.
  bool          finished_splitting    = true;
  double        highest_ig            =  0.0;
  unsigned int  highest_ig_column     =  0;
  std::string   highest_ig_column_str = "";
  double        highest_ig_threshold  = -1.0;
  for (
    Dataset::KeyList::const_iterator key = sample_keys.begin();
    key != sample_keys.end(); ++key )
  {
    double threshold = 0.0;
    double ig = data.information_gain(
      // INPUT
      decision_column, key->second,
      // OUTPUT
      threshold );
    if ( ig > highest_ig )
    {
      finished_splitting = false;
      highest_ig = ig;
      highest_ig_threshold = threshold;
      highest_ig_column = key->second;
      highest_ig_column_str = key->first;
    }
  }

  // Finished splitting?
  if ( finished_splitting )
  {
    // Determine best classification.
    bool best_class = (pm_class["LE"] < pm_class["G"]);

    // Add leaf node.
    Node * leaf = new LeafNode( action, best_class );
    if ( parent == null(Node) )
    {
      root = leaf;
    }
    else
    {
      parent->add_child(leaf);
    }
  }

  // Not finished splitting.
  else
  {
    // Add split node.
    Node * split = new SplitNode( action, highest_ig_column, highest_ig_threshold );

    // Split data.
    DatasetSplitPair split_pair = data.split( highest_ig_column, highest_ig_threshold );

    // Build <= node.
    stringstream action_le;
    action_le << highest_ig_column_str << " <= " << highest_ig_threshold;
    _grow_decision_tree(
      split_pair.ds_le, split_keys, keys_per_node,
      decision_column, split, action_le.str() );

    // Build > node.
    stringstream action_g;
    action_g << highest_ig_column_str << " > " << highest_ig_threshold;
    _grow_decision_tree(
      split_pair.ds_g, split_keys, keys_per_node,
      decision_column, split, action_g.str() );

    // Add node to tree.
    if ( parent == null(Node) )
    {
      root = split;
    }
    else
    {
      parent->add_child(split);
    }
  }
}

//------------------------------------------------------------------------------

bool RandomTree::classify( const double * const & row ) const
{
  // Classify.
  Node * next_node = root;
  while ( next_node != null(Node) )
  {
    // Leaf?
    if ( next_node->is_leaf() )
    {
      return next_node->get_classification();
    }

    // Not a leaf. Must be a split.
    else
    {
      SplitNode *split = reinterpret_cast<SplitNode*>(next_node);
      unsigned int split_column = split->get_column();
      double split_threshold = split->get_threshold();

      // Get children.
      Node::NodeSet children = split->get_children();

      // Two children?
      if ( children.size() == 2u )
      {
        // Move to correct child.
        unsigned int child_index = (row[split_column] > split_threshold) ? 1 : 0;
        next_node = children[child_index];
      }

      // < than or > than two children? Odd...
      else
      {
        return false;
      }
    }
  }

  // Nothing found? ... Odd...
  return false;
}

//------------------------------------------------------------------------------

void RandomTree::classify_oob_set( double & oob_error ) const
{
  //
}

//------------------------------------------------------------------------------

