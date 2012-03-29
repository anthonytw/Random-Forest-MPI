/*
 * RandomForest.cpp
 *
 *  Created on: Mar 29, 2012
 *      Author: anthony
 */

#include "RandomForest.h"

//------------------------------------------------------------------------------

void RandomForest::grow_forest(
  Dataset & dataset,
  const unsigned int decision_column,
  const unsigned int bootstrap_size,
  const Dataset::KeyList & split_keys,
  const unsigned int keys_per_node,
  const unsigned int tree_count )
{
  //
}

//------------------------------------------------------------------------------

bool RandomForest::classify( const double * const & row )
{
  return false;
}

//------------------------------------------------------------------------------

void RandomForest::classify_oob( const Dataset::KeyList & keys_to_permute )
{
  //
}

//------------------------------------------------------------------------------
