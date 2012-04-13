/*
 * RandomForest.h
 *
 *  Created on: Mar 29, 2012
 *      Author: anthony
 */

#ifndef __RandomForest_h__
#define __RandomForest_h__

#include "RandomTree.h"

#include <pthread.h>
#include <semaphore.h>

class RandomForest
{
  public:
    /**
     * Defines a forest (a vector of trees).
     */
    typedef std::vector<RandomTree*> Forest;

  private:
    /**
     * Defines a thread-safe result queue.
     */
    class ResultQueue
    {
      public:
        /**
         * Constructor.
         */
        ResultQueue( void )
        {
          // Initialize mutex.
          pthread_mutex_init(&mutex, NULL);

          // Initialize semaphore.
          sem_init( &semaphore, 0, 0 );
        }

        /**
         * Function to push a result tree.
         */
        void push( RandomTree * result )
        {
          pthread_mutex_lock(&mutex);
          results.push_back(result);
          pthread_mutex_unlock(&mutex);
          sem_post( &semaphore );
        }

        /**
         * Pull next result.
         * @return Pointer to a RandomTree.
         */
        RandomTree * pop( void )
        {
          sem_wait(&semaphore);
          pthread_mutex_lock(&mutex);
          RandomTree *result = results[results.size()-1];
          results.pop_back();
          pthread_mutex_unlock(&mutex);

          return result;
        }

      private:
        pthread_mutex_t mutex;  ///< Access mutex.
        sem_t semaphore;        ///< Data semaphore. Indicates data is available for reading.
        Forest results;         ///< The results list.
    };

    /**
     * Defines a forest growing task.
     */
    class ForestGrowingTask
    {
      public:
        /**
         * Constructor.
         */
        ForestGrowingTask(
          Dataset & dataset,
          const unsigned int decision_column,
          const unsigned int bootstrap_size,
          const Dataset::KeyList & split_keys,
          const unsigned int keys_per_node,
          const unsigned int tree_count,
          ResultQueue * const result_queue ) :
            dataset(dataset),
            decision_column(decision_column),
            bootstrap_size(bootstrap_size),
            split_keys(split_keys),
            keys_per_node(keys_per_node),
            tree_count(tree_count),
            result_queue(result_queue)
        {
          //
        }

        /**
         * Spawn the task.
         * @return The return value of the thread creation routine.
         */
        int spawn( void )
        {
          return pthread_create(
            &thread, NULL, ForestGrowingTask::route,
            reinterpret_cast<void*>(this) );
        }

      private:
        /**
         * Thread routing function.
         * @param class_pointer The pointer to the task class to execute.
         * @return Null pointer (unused, required for interface).
         */
        static void * route( void * class_pointer )
        {
          ForestGrowingTask *task = reinterpret_cast<ForestGrowingTask*>(class_pointer);
          task->run();
          return null(void);
        }

        /**
         * Run the task.
         */
        void run( void )
        {
          // Iteratively build each tree.
          for ( unsigned int tree_index = 0; tree_index < tree_count; ++tree_index )
          {
            // Generate a bootstrap sample.
            Dataset bootstrap = dataset.bootstrap_sample(bootstrap_size);

            // Generate a random tree.
            RandomTree *tree = new RandomTree;
            tree->grow_decision_tree(
              bootstrap, split_keys, keys_per_node, decision_column );

            // Push result.
            result_queue->push(tree);
          }
        }

      private:
        pthread_t thread;
        Dataset & dataset;
        const unsigned int decision_column;
        const unsigned int bootstrap_size;
        const Dataset::KeyList & split_keys;
        const unsigned int keys_per_node;
        const unsigned int tree_count;
        ResultQueue * const result_queue;
    };

  public:
    /**
     * Clears the built forest.
     */
    void burn( void )
    {
      // Destroy all trees.
      for (
        Forest::iterator iter = forest.begin();
        iter != forest.end(); ++iter )
      {
        delete (*iter);
        *iter = null(RandomTree);
      }
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

    /**
     * Serializes the tree.
     * @param stream The output stream.
     * @return The output stream.
     */
    std::ostream & serialize( std::ostream & stream )
    {
      // Write the trees.
      std::size_t size = forest.size();
      stream.write((char*)&size, sizeof(size));
      for ( std::size_t t = 0; t < size; ++t )
      {
        forest[t]->serialize(stream);
      }
      return stream;
    }

    /**
     * Deserializes the tree and appends it to the current tree.
     * @param stream The input stream.
     * @return The input stream.
     */
    std::istream & deserialize_append( std::istream & stream )
    {
      // Read tree count.
      std::size_t size = 0;
      stream.read((char*)&size, sizeof(size));
      for ( std::size_t t = 0; t < size; ++t )
      {
        RandomTree * tree = new RandomTree;
        tree->deserialize(stream);
        forest.push_back(tree);
      }

      // Done.
      return stream;
    }

    /**
     * Deserializes the tree.
     * @param stream The input stream.
     * @return The input stream.
     */
    std::istream & deserialize( std::istream & stream )
    {
      // Burn the forest to the ground!
      burn();

      // Deserialize.
      return deserialize_append( stream );
    }

    /**
     * Returns the size of the forest.
     * @return Forest size.
     */
    unsigned int get_size( void ) const
    {
      return forest.size();
    }

  private:
    Forest forest;        ///< The random forest generated.

  private:
    friend class ut_RandomForest;   ///< For unit testing.
};

#endif
