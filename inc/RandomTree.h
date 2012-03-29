/*
 * RandomTree.h
 *
 *  Created on: Mar 26, 2012
 *      Author: anthony
 */

#ifndef __RandomTree_h__
#define __RandomTree_h__

#include "Dataset.h"
#include "Node.h"

/**
 * Grows and classifies with a real-valued binary classification tree.
 */
class RandomTree
{
  public:
    /**
     * Constructor.
     */
    RandomTree( void ) :
      root( null(Node) )
    {
      //
    }

    /**
     * Destructor.
     * @note This will delete all child nodes.
     */
    ~RandomTree( void )
    {
      burn();
    }

    /**
     * Delete any pre-existing tree.
     * @note This will delete all child nodes.
     */
    void burn( void )
    {
      if ( root != null(Node) )
      {
        delete root;
        root = null(Node);
      }
    }

    /**
     * Returns the root node.
     * @return Pointer to the root node.
     */
    Node * const get_root( void ) const
    {
      return root;
    }
    Node * get_root( void )
    {
      return root;
    }

    /**
     * Draws the GraphVis representation of the tree and returns it as a string.
     * @return The string GraphVis representation.
     */
    std::string draw( void ) const;

    /**
     * Grows the tree.
     * @param data The dataset to use.
     * @param split_keys The valid keys to split from.
     * @param keys_per_node This is the number of keys per node to compare for
     *    splitting.
     * @param decision_column The column of the classification.
     */
    void grow_decision_tree(
      Dataset & data,
      const Dataset::KeyList & split_keys,
      const unsigned int keys_per_node,
      const unsigned int decision_column);

    /**
     * Classifies a row of data.
     * @param row A pointer to the data row.
     * @return The binary classification (true or false).
     */
    bool classify( const double * const & row ) const;

    /**
     * Classifies the out-of-bag (OOB) dataset.
     * @param [out] oob_error The OOB set classification error.
     * @param [out]
     * @return
     */
    void classify_oob_set( double & oob_error ) const;

  private:
    /**
     * Private helper method to grow the tree.
     * @param data The dataset to use.
     * @param split_keys The valid keys to split from.
     * @param keys_per_node This is the number of keys per node to compare for
     *    splitting.
     * @param decision_column The column of the classification.
     * @param parent The parent node.
     * @param action The transition action.
     */
    void _grow_decision_tree(
      Dataset & data,
      const Dataset::KeyList & split_keys,
      const unsigned int keys_per_node,
      const unsigned int decision_column,
      Node * const parent,
      const std::string action );

  private:
    Node * root;                        ///< Root node.

  private:
    friend class ut_RandomTree;         ///< For unit testing.
};

#endif
