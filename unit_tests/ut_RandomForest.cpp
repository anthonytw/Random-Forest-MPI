/*
 * ut_RandomTree.cpp
 *
 *  Created on: Mar 28, 2012
 *      Author: anthony
 */

#include <cppunit/extensions/HelperMacros.h>
#include "ut_RandomForest.h"
#include "RandomForest.h"
#include "Dataset.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;

//CPPUNIT_TEST_SUITE_REGISTRATION( ut_RandomForest );

//------------------------------------------------------------------------------

vector<string> Tokenize(const string& str,const string& delimiters)
{
  vector<string> tokens;
  string::size_type delimPos = 0, tokenPos = 0, pos = 0;

  if(str.length() < 1)
  {
    return tokens;
  }

  while (true)
  {
    delimPos = str.find_first_of(delimiters, pos);
    tokenPos = str.find_first_not_of(delimiters, pos);

    if(string::npos != delimPos)
    {
      if(string::npos != tokenPos)
      {
        if(tokenPos<delimPos)
        {
          tokens.push_back(str.substr(pos,delimPos-pos));
        }
        else
        {
          tokens.push_back("");
        }
      }
      else
      {
        tokens.push_back("");
      }
      pos = delimPos+1;
    }
    else
    {
      if(string::npos != tokenPos)
      {
        tokens.push_back(str.substr(pos));
      }
      else
      {
        tokens.push_back("");
      }
      break;
    }
  }
  return tokens;
}

//------------------------------------------------------------------------------

void ut_RandomForest::setUp( void )
{
  // Load in dataset.
  string line;
  vector<string> lines;
  ifstream file( "data/seq_val_1_2.csv", ios_base::in );
  while ( getline(file, line, '\n') )
  {
    lines.push_back( line );
  }

  // Tokenize first line.
  vector<string> l1t = Tokenize(lines[0], "\t");

  // Create dataset.
  const unsigned int row_count = lines.size();
  const unsigned int col_count = l1t.size() - 2; // Ignore first (ID) column and last (?) col.
  const unsigned int feature_count = col_count - 1; // Class and n-1 features.

  // Dataset exists?
  if ( ds != null(Dataset) )
  {
    delete ds;
    ds = null(Dataset);
  }
  ds = new Dataset( row_count, col_count );
  Dataset & dsr = *ds;

  // Convert data.
  for ( unsigned int row = 0; row < row_count; ++row )
  {
    // Tokenize row.
    vector<string> tokens = Tokenize(lines[row], "\t");

    // First element is the ID. Skip.
    // Second element is the class (1 - somatic, 2 - germline, 3 - wildtype).
    dsr[row][0] = ( atof(tokens[1].c_str()) <= 1.0 ) ? 1.0 : 0.0;

    // Fetch the rest of the features.
    for ( unsigned int col = 1; col < col_count; ++col )
    {
      dsr[row][col] = atof(tokens[col + 1].c_str());
    }
  }

  // Configure keys.
  Dataset::KeyList & keys = ds->get_keys();
  keys["class"] = 0;
  for ( unsigned int feature = 1; feature <= feature_count; ++feature )
  {
    stringstream feature_ident;
    feature_ident << "f" << feature;
    keys[feature_ident.str()] = feature;
  }
}

//------------------------------------------------------------------------------

void ut_RandomForest::tearDown( void )
{
  if ( ds != null(Dataset) )
  {
    delete ds;
    ds = null(Dataset);
  }
}

//------------------------------------------------------------------------------

void ut_RandomForest::testMethod_grow_forest( void )
{
  cout << endl;

  // Determine split keys. All keys except class.
  Dataset::KeyList split_keys = ds->get_keys();
  split_keys.erase("class");

  // Data should be loaded. Time to grow the forest.
  cout << "Grow..." << endl;
  RandomForest forest;
  forest.grow_forest(
    *ds, 0, ds->row_count() / 3, split_keys, 16, 100 );
  cout << "Grown!" << endl;

  // Classify the data.
  unsigned int tp = 0;
  unsigned int tn = 0;
  unsigned int fp = 0;
  unsigned int fn = 0;
  for ( unsigned int row = 0; row < ds->row_count(); ++row )
  {
    bool c = forest.classify((*ds)[row]);
    bool t = (*ds)[row][0] == 1.0;

    if ( c && t ) ++tp;
    else if ( c && !t ) ++fp;
    else if ( !c && t ) ++fn;
    else if ( !c && !t ) ++ tn;
    else cout << "????" << endl;
  }
  double accuracy = (tp + tn) * 100.0 / (tp + fp + tn + fn);
  double precision = tp * 100.0 / (tp + fp);
  double true_negative_rate = tn * 100.0 / (tn + fp);
  double recall = tp * 100.0 / (tp + fn);
  cout
    << "Accuracy          : " << accuracy << "%\n"
    << "Precision         : " << precision << "%\n"
    << "True negative rate: " << true_negative_rate << "%\n"
    << "Recall            : " << recall << endl;
}

//------------------------------------------------------------------------------

void ut_RandomForest::testMethod_classify( void )
{
  //
}

//------------------------------------------------------------------------------

void ut_RandomForest::testMethod_classify_oob_set( void )
{
  //
}

//------------------------------------------------------------------------------
