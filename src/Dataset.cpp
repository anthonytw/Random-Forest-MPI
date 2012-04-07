#include "Dataset.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

//------------------------------------------------------------------------------

Dataset::Dataset( const unsigned int rows, const unsigned int columns ) :
  data_ref(rows),
  data( new RealMatrix(rows, columns) ),
  usingRef(false)
{
  // Initialize dataset to point to data elements.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    data_ref[row] = row;
  }
}

//------------------------------------------------------------------------------

Dataset::Dataset( Dataset & reference, const unsigned int rows ) :
  data_ref(rows),
  data(&reference.data_matrix()),
  usingRef(true)
{
  //
}

//------------------------------------------------------------------------------

Dataset::~Dataset( void )
{
  // Destroy dataset if it's not a reference.
  if ( !usingRef )
  {
    delete data;
  }
}

//------------------------------------------------------------------------------

Dataset::ProbabilityMap Dataset::enumerate_threshold(
  const unsigned int column, const double threshold ) const
{
  ProbabilityMap pmap;
  pmap["LE"]  = 0.0;
  pmap["G"]   = 0.0;

  unsigned int rows = data_ref.row_count();
  if ( rows > 0 )
  {
    // Iterate through elements to separate them into two categories.
    for ( unsigned int row = 0; row < rows; ++row )
    {
      if ( operator[](row)[column] > threshold )
      {
        pmap["G"] += 1.0;
      }
      else
      {
        pmap["LE"] += 1.0;
      }
    }

    // Normalize.
    pmap["LE"] /= rows;
    pmap["G"] /= rows;
  }

  // Done.
  return pmap;
}

//------------------------------------------------------------------------------

Dataset::ThresholdVector Dataset::get_thresholds( const unsigned int column ) const
{
  ThresholdVector thresholds;
  for ( unsigned int row = 0; row < row_count(); ++row )
  {
    thresholds.push_back( operator[](row)[column] );
  }
  return thresholds;
}

//------------------------------------------------------------------------------

DatasetSplitPair Dataset::split(
  const unsigned int column, const double threshold )
{
  // Start with two referenced datasets allocating enough space for the entire
  // dataset.
  DatasetSplitPair split( *this, row_count() );
  unsigned int ds_le_elements = 0;
  unsigned int ds_g_elements = 0;

  // Partition data.
  for ( unsigned int row = 0; row < row_count(); ++row )
  {
    if ( operator[](row)[column] > threshold )
    {
      split.ds_g(ds_g_elements) = operator()(row);
      ++ds_g_elements;
    }
    else
    {
      split.ds_le(ds_le_elements) = operator()(row);
      ++ds_le_elements;
    }
  }

  // Shrink datasets appropriately.
  split.ds_le.resize( ds_le_elements );
  split.ds_g.resize( ds_g_elements );

  return split;
}

//------------------------------------------------------------------------------

double Dataset::information_gain(
  const unsigned int decision_column,
  const unsigned int attribute_column,
  double & threshold )
{
  // Calculate classification entropy.
  ProbabilityMap enum_dc = enumerate_threshold( decision_column, 0.0 );
  double class_entropy = enum_dc.entropy();

  // Iterate over all possible thresholds.
  ThresholdVector thresholds = get_thresholds( attribute_column );
  double best_split_threshold = -1e199;
  double best_split_ig        = -1000.0;
  for (
    ThresholdVector::const_iterator iter = thresholds.begin();
    iter != thresholds.end(); ++iter )
  {
    double split_ig = class_entropy;
    double threshold = *iter;

    // Enumerate at threshold.
    DatasetSplitPair split_pair = split( attribute_column, threshold );
    double split_prob_le = split_pair.ds_le.row_count() / static_cast<double>(row_count());
    double split_prob_g = split_pair.ds_g.row_count() / static_cast<double>(row_count());

    // For LE split, calculate new entropy.
    ProbabilityMap enum_dc_le = split_pair.ds_le.enumerate_threshold( decision_column, 0.0 );
    double class_entropy_le = enum_dc_le.entropy();
    split_ig -= split_prob_le * class_entropy_le;

    // For G split, calculate new entropy.
    ProbabilityMap enum_dc_g = split_pair.ds_g.enumerate_threshold( decision_column, 0.0 );
    double class_entropy_g = enum_dc_g.entropy();
    split_ig -= split_prob_g * class_entropy_g;

    // Best split?
    if ( split_ig > best_split_ig )
    {
      best_split_ig = split_ig;
      best_split_threshold = threshold;
    }
  }

  // Finished.
  threshold = best_split_threshold;
  return best_split_ig;
}

//------------------------------------------------------------------------------

Dataset Dataset::bootstrap_sample( const unsigned int sample_size )
{
  Dataset sample( *this, sample_size );
  unsigned int rows = row_count();
  for ( unsigned int row = 0; row < sample_size; ++row )
  {
    sample(row) = static_cast<unsigned int>(rand()) % rows;
  }
  return sample;
}

//------------------------------------------------------------------------------

Dataset Dataset::out_of_bag_set( void )
{
  Dataset oob_sample( *this, data->row_count() );

  // Add OOB elements.
  unsigned int oob_elements = 0;
  for ( unsigned int row = 0; row < data->row_count(); ++row )
  {
    if ( !data_ref.contains(row) )
    {
      oob_sample(oob_elements) = row;
      ++oob_elements;
    }
  }

  // Resize data_ref.
  oob_sample.resize(oob_elements);

  // Done.
  return oob_sample;
}

//------------------------------------------------------------------------------

std::ostream & operator<<( std::ostream & stream, const Dataset::IntegerColumn & column )
{
  stream << "Row count: " << column.row_count() << "; Rows:";
  for ( unsigned int row = 0; row < column.row_count(); ++row )
  {
    stream << " " << column[row];
  }
  stream << endl;
  return stream;
}

//------------------------------------------------------------------------------

std::ostream & operator<<( std::ostream & stream, const Dataset::RealMatrix & matrix )
{
  for ( unsigned int row = 0; row < matrix.row_count(); ++row )
  {
    for ( unsigned int column = 0; column < matrix.column_count(); ++column )
    {
      stream
        << std::setiosflags(std::ios::fixed)
        << std::setprecision(2)
        << std::setw(8)
        << matrix[row][column];
    }
    stream << endl;
  }
  return stream;
}
