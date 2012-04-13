/*
 * RandomForest.cpp
 *
 *  Created on: Mar 29, 2012
 *      Author: anthony
 */

#include "RandomForest.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------

void RandomForest::grow_forest(
  Dataset & dataset,
  const unsigned int decision_column,
  const unsigned int bootstrap_size,
  const Dataset::KeyList & split_keys,
  const unsigned int keys_per_node,
  const unsigned int tree_count )
{
  // If a forest exits, destroy it.
  burn();

  // Initialize result queue.
  ResultQueue results;

  // Initialize tasks.
  unsigned int task_count = 1;
  unsigned int trees_per_task = tree_count / task_count;
  unsigned int total_trees = trees_per_task * task_count;
  vector<ForestGrowingTask*> task_list;
  for ( unsigned int task_id = 0; task_id < task_count; ++task_id )
  {
    ForestGrowingTask *task = new ForestGrowingTask(
      dataset, decision_column, bootstrap_size, split_keys, keys_per_node,
      trees_per_task, &results );
    task->spawn();
  }

  // Fetch results.
  unsigned int on_tree = 0;
  while ( forest.size() < total_trees )
  {
    RandomTree * result = results.pop();
    forest.push_back( result );

    ++on_tree;
    if ((on_tree % 50) == 0)
    {
      cout << " ; " << on_tree << " of " << total_trees << " (" << on_tree*100.0/total_trees << "%)" << endl;
    }
    else if ((on_tree % 10) == 0)
    {
      cout << "|" << flush;
    }
    else
    {
      cout << "." << flush;
    }
  }
  cout << endl;
}

//------------------------------------------------------------------------------

bool RandomForest::classify( const double * const & row )
{
  // Ensure a forest is built.
  if ( forest.size() <= 0 )
  {
    return false;
  }

  // Classify the forest.
  unsigned int votes_0 = 0;
  unsigned int votes_1 = 0;
  for ( unsigned int tree_index = 0; tree_index < forest.size(); ++tree_index )
  {
    bool classification = forest[tree_index]->classify(row);
    if ( classification )
    {
      ++votes_1;
    }
    else
    {
      ++votes_0;
    }
  }

  return votes_1 > votes_0;
}

//------------------------------------------------------------------------------

void RandomForest::classify_oob( const Dataset::KeyList & keys_to_permute )
{
  //
}

//------------------------------------------------------------------------------
