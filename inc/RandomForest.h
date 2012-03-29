/*
 * RandomForest.h
 *
 *  Created on: Mar 29, 2012
 *      Author: anthony
 */

#ifndef __RandomForest_h__
#define __RandomForest_h__

#include "RandomTree.h"

class RandomForest
{
  public:
    /*
     * Defines a forest (a vector of trees).
     */
    typedef std::vector<RandomTree> Forest;

  public:
    /**
     * Clears the built forest.
     */
    void burn( void )
    {
      forest.clear();
    }

    /**
     * Grows a forest of random trees.
     * @param dataset The dataset to use.
     * @param decision_column The column of the classification label.
     * @param bootstrap_size The size of the bootstrap samples.
     * @param split_keys The keys to split on.
     * @param keys_per_node The keys per node to compare for splits.
     * @param tree_count The number of trees to build.
     */
    void grow_forest(
      Dataset & dataset,
      const unsigned int decision_column,
      const unsigned int bootstrap_size,
      const Dataset::KeyList & split_keys,
      const unsigned int keys_per_node,
      const unsigned int tree_count );

    /**
     * Classify a row using the majority vote from the forest.
     * @param row The data row to classify.
     * @return The classification.
     */
    bool classify( const double * const & row );

    /**
     * Classify the out-of-bag set for each tree.
     */
    void classify_oob( const Dataset::KeyList & keys_to_permute );

  private:
    Forest forest;        ///< The random forest generated.
};

#endif
