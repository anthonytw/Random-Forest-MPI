/*
 * ut_Dataset.cpp
 *
 *  Created on: Mar 22, 2012
 *      Author: anthony
 */

#include <cppunit/extensions/HelperMacros.h>
#include "ut_Dataset.h"
#include "Dataset.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( ut_Dataset );

//------------------------------------------------------------------------------

void ut_Dataset::ProbabilityMap_testMethod_entropy( void )
{
  // Check entropy calculation.
  Dataset::ProbabilityMap pmap;

  // No entries.
  CPPUNIT_ASSERT_DOUBLES_EQUAL(
    0.0, pmap.entropy(), 1e-9 );

  // One entry.
  pmap["e1"] = 1.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(
    0.0, pmap.entropy(), 1e-9 );

  // A few entries, one with value zero.
  pmap["e1"] = 0.5;
  pmap["e2"] = 0.5;
  pmap["e3"] = 0.0;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(
    0.301029995663981, pmap.entropy(), 1e-9 );

  // A few more entries.
  pmap["e1"] = 0.1;
  pmap["e2"] = 0.2;
  pmap["e3"] = 0.3;
  pmap["e4"] = 0.4;
  CPPUNIT_ASSERT_DOUBLES_EQUAL(
    0.55583362792012, pmap.entropy(), 1e-9 );
}

//------------------------------------------------------------------------------

void ut_Dataset::IntegerColumn_testClass( void )
{
  // Initialize class.
  const unsigned int original_row_count = 10;
  const unsigned int shrink_row_count   = 5;
  unsigned int *data_pointer            = null(unsigned int);
  Dataset::IntegerColumn icol( original_row_count );
  CPPUNIT_ASSERT_EQUAL( original_row_count, icol.row_count() );

  // Set elements.
  for ( unsigned int row = 0; row < icol.row_count(); ++row )
  {
    if ( row > 0 )
    {
      CPPUNIT_ASSERT_EQUAL( false, icol.contains(row) );
    }
    icol[row] = row;
    CPPUNIT_ASSERT_EQUAL( row, icol[row] );
    CPPUNIT_ASSERT_EQUAL( true, icol.contains(row) );
  }

  // Test "contains" method.
  CPPUNIT_ASSERT_EQUAL( false, icol.contains(original_row_count) );

  // Shrink.
  data_pointer = &icol[0];
  icol.resize( shrink_row_count );
  CPPUNIT_ASSERT( data_pointer != &icol[0] );
  CPPUNIT_ASSERT_EQUAL( shrink_row_count, icol.row_count() );
  for ( unsigned int row = 0; row < icol.row_count(); ++row )
  {
    CPPUNIT_ASSERT_EQUAL( row, icol[row] );
  }

  // Expand.
  data_pointer = &icol[0];
  icol.resize( original_row_count );
  CPPUNIT_ASSERT( data_pointer != &icol[0] );
  CPPUNIT_ASSERT_EQUAL( original_row_count, icol.row_count() );
  for ( unsigned int row = 0; row < shrink_row_count; ++row )
  {
    CPPUNIT_ASSERT_EQUAL( row, icol[row] );
  }
  for ( unsigned int row = shrink_row_count; row < icol.row_count(); ++row )
  {
    CPPUNIT_ASSERT_EQUAL( 0u, icol[row] );
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::RealMatrix_testClass( void )
{
  for ( unsigned int rowSpan = 1; rowSpan < 10; ++rowSpan )
  {
    for ( unsigned int columnSpan = 1; columnSpan < 10; ++columnSpan )
    {
      Dataset::RealMatrix matrix( rowSpan, columnSpan );

      for ( unsigned int row = 0; row < matrix.row_count(); ++row )
      {
        double * writeable = matrix[row];
        const double * const unwriteable = matrix[row];
        for ( unsigned int column = 0; column < matrix.column_count(); ++column )
        {
          CPPUNIT_ASSERT_EQUAL( 0.0, writeable[column] );
          CPPUNIT_ASSERT_EQUAL( 0.0, unwriteable[column] );
          double index = row * matrix.column_count() + column;
          writeable[column] = index;
          CPPUNIT_ASSERT_EQUAL( index, writeable[column] );
          CPPUNIT_ASSERT_EQUAL( index, unwriteable[column] );
        }
      }
    }
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::testConstructors( void )
{
  const unsigned int rows = 10;
  const unsigned int cols = 10;

  // Test allocation constructor.
  Dataset ds1( rows, cols );
  CPPUNIT_ASSERT_EQUAL( false, ds1.usingRef );
  CPPUNIT_ASSERT_EQUAL( rows, ds1.data_ref.row_count() );
  for ( unsigned int r = 0; r < ds1.data_ref.row_count(); ++r )
  {
    CPPUNIT_ASSERT_EQUAL( r, ds1(r) );
  }
  CPPUNIT_ASSERT_EQUAL( rows, ds1.data->row_count() );
  CPPUNIT_ASSERT_EQUAL( cols, ds1.data->column_count() );

  // Test reference constructor.
  unsigned int rows2 = 5;
  Dataset ds2( ds1, rows2 );
  CPPUNIT_ASSERT_EQUAL( true, ds2.usingRef );
  CPPUNIT_ASSERT_EQUAL( rows2, ds2.data_ref.row_count() );
  for ( unsigned int r = 0; r < ds2.data_ref.row_count(); ++r )
  {
    CPPUNIT_ASSERT_EQUAL( 0u, ds2(r) );
  }
  CPPUNIT_ASSERT_EQUAL( rows, ds2.data->row_count() );
  CPPUNIT_ASSERT_EQUAL( cols, ds2.data->column_count() );
  CPPUNIT_ASSERT_EQUAL( ds1.data, ds2.data );
}

//------------------------------------------------------------------------------

void ut_Dataset::testOperator_array( void )
{
  // Create dataset.
  const unsigned int rows = 10;
  const unsigned int cols = 1;
  Dataset ds( rows, cols );

  // Set data.
  for ( unsigned int r = 0; r < rows; ++r )
  {
    ds[r][0] = r + 1;
    CPPUNIT_ASSERT_EQUAL(
      r + 1, static_cast<unsigned int>((*ds.data)[r][0]) );
  }

  // Reverse row references.
  for ( unsigned int r = 0; r < rows; ++r )
  {
    ds(rows - r - 1) = r;
  }
  for ( unsigned int r = 0; r < rows; ++r )
  {
    ds[r][0] = r + 10;
    CPPUNIT_ASSERT_EQUAL(
      r + 10, static_cast<unsigned int>((*ds.data)[rows - r - 1][0]) );
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::testMethod_enumerate_threshold( void )
{
  // Create dataset.
  const unsigned int rows = 10;
  const unsigned int cols = 1;
  Dataset::ProbabilityMap pmap;
  Dataset ds1( rows, cols );

  // Populate dataset.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    ds1[row][0] = row + 1;
  }

  // Enumerate.
  pmap = ds1.enumerate_threshold( 0, 5 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, pmap["G"], 1e-9 );
  pmap = ds1.enumerate_threshold( 0, 10 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["G"], 1e-9 );
  pmap = ds1.enumerate_threshold( 0, 0 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["G"], 1e-9 );

  // New dataset.
  Dataset ds2( ds1, rows );

  // All elements are mapped to element 0 by default, so they all point to the
  // value of the first data element (which is 1).

  // Enumerate.
  pmap = ds2.enumerate_threshold( 0, 5 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["G"], 1e-9 );
  pmap = ds2.enumerate_threshold( 0, 10 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["G"], 1e-9 );
  pmap = ds2.enumerate_threshold( 0, 1 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["G"], 1e-9 );
  pmap = ds2.enumerate_threshold( 0, 0 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["G"], 1e-9 );

  // Map elements.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    ds2(row) = row;
  }

  // Enumerate.
  pmap = ds2.enumerate_threshold( 0, 5 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.5, pmap["G"], 1e-9 );
  pmap = ds2.enumerate_threshold( 0, 10 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["G"], 1e-9 );
  pmap = ds2.enumerate_threshold( 0, 0 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0, pmap["LE"], 1e-9 );
  CPPUNIT_ASSERT_DOUBLES_EQUAL( 1.0, pmap["G"], 1e-9 );
}

//------------------------------------------------------------------------------

void ut_Dataset::testMethod_get_thresholds( void )
{
  // Create dataset.
  const unsigned int rows = 10;
  const unsigned int cols = 1;
  Dataset ds( rows, cols );

  // Populate dataset.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    ds[row][0] = row + 1;
  }

  // Get thresholds.
  Dataset::ThresholdVector thresholds = ds.get_thresholds(0);
  for ( unsigned int row = 0; row < rows; ++row )
  {
    CPPUNIT_ASSERT_DOUBLES_EQUAL( static_cast<double>(row + 1), thresholds[row], 1e-9 );
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::testMethod_split( void )
{
  // Create dataset.
  const unsigned int rows = 10;
  const unsigned int cols = 1;
  Dataset ds( rows, cols );

  // Populate dataset.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    ds[row][0] = row + 1;
  }

  // Split.
  DatasetSplitPair split = ds.split( 0, 5.0 );
  CPPUNIT_ASSERT_EQUAL( 5u, split.ds_le.row_count() );
  CPPUNIT_ASSERT_EQUAL( 5u, split.ds_g.row_count() );
  for ( unsigned int row = 0; row < split.ds_le.row_count(); ++row )
  {
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
      static_cast<double>(row + 1),
      split.ds_le[row][0], 1e-9 );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
      static_cast<double>(row + 1 + 5),
      split.ds_g[row][0], 1e-9 );
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::testMethod_information_gain( void )
{
  // Generate example dataset.
  const unsigned int rows       = 10;
  const unsigned int cols       = 2;
  Dataset ds( rows, cols );

  // Generate data.
  for ( double class_threshold = 1.0; class_threshold < 10.0; class_threshold += 1.0 )
  {
    for ( unsigned int row = 0; row < ds.row_count(); ++row )
    {
      ds[row][0] = row + 1;
      if ( ds[row][0] > class_threshold )
      {
        ds[row][1] = 1.0;
      }
      else
      {
        ds[row][1] = 0.0;
      }
    }

    // Find best information gain.
    double threshold = 0.0;
    // TODO: Check IG.
    ds.information_gain( 1, 0, threshold );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
      class_threshold, threshold, 1e-9 );
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::testMethod_bootstrap_sample( void )
{
  // Create dataset.
  const unsigned int rows = 10;
  const unsigned int cols = 1;
  Dataset ds( rows, cols );

  // Populate dataset.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    ds[row][0] = row + 1;
  }

  // Create a bootstrap sample.
  for ( unsigned int sample_size = 0; sample_size < 20; ++sample_size )
  {
    Dataset sample = ds.bootstrap_sample(sample_size);
    CPPUNIT_ASSERT_EQUAL( sample_size, sample.row_count() );
    for ( unsigned int row = 0; row < sample_size; ++row )
    {
      CPPUNIT_ASSERT( (sample(row) >= 0) && (sample(row) < ds.row_count()) );
    }
  }
}

//------------------------------------------------------------------------------

void ut_Dataset::testMethod_out_of_bag_set( void )
{
  // Create dataset.
  const unsigned int rows = 10;
  const unsigned int cols = 1;
  Dataset ds( rows, cols );

  // Populate dataset.
  for ( unsigned int row = 0; row < rows; ++row )
  {
    ds[row][0] = row + 1;
  }

  // Split.
  DatasetSplitPair split = ds.split( 0, 5.0 );

  // Get OOB sets.
  Dataset oob_le = split.ds_le.out_of_bag_set();
  Dataset oob_g = split.ds_g.out_of_bag_set();

  // Check OOB sets.
  for ( unsigned int row = 0; row < oob_le.row_count(); ++row )
  {
    CPPUNIT_ASSERT_EQUAL( row + 5, oob_le(row) );
    CPPUNIT_ASSERT_EQUAL( row, oob_g(row) );
  }
}

//------------------------------------------------------------------------------
