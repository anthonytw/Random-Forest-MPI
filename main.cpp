#include <iostream>
#include <map>
#include <fstream>
#include <stdlib.h>

#include "RandomForest.h"
#include "Dataset.h"

using namespace std;

namespace std
{
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
}

int main( const int argc, const char ** argv )
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
  unsigned int row_count = lines.size();
  unsigned int col_count = l1t.size() - 2; // Ignore first (ID) column and last (?) col.
  unsigned int feature_count = col_count - 1; // Class and n-1 features.

  // Dataset.
  Dataset dsr( row_count, col_count );

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
  Dataset::KeyList & keys = dsr.get_keys();
  keys["class"] = 0;
  for ( unsigned int feature = 1; feature <= feature_count; ++feature )
  {
    stringstream feature_ident;
    feature_ident << "f" << feature;
    keys[feature_ident.str()] = feature;
  }
  // Determine split keys. All keys except class.
  Dataset::KeyList split_keys = dsr.get_keys();
  split_keys.erase("class");

  // Data should be loaded. Time to grow the forest.
  cout << "Grow..." << endl;
  RandomForest forest;
  forest.grow_forest(
    dsr, 0, dsr.row_count() / 3, split_keys, 32, 1000 );
  cout << "Grown!" << endl;

  // Classify the training data.
  unsigned int tp = 0;
  unsigned int tn = 0;
  unsigned int fp = 0;
  unsigned int fn = 0;
  for ( unsigned int row = 0; row < dsr.row_count(); ++row )
  {
    bool c = forest.classify(dsr[row]);
    bool t = dsr[row][0] == 1.0;

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
    << "-----------------------------------------------------------------------\n"
    << "Training Set Classification:\n"
    << "-----------------------------------------------------------------------\n"
    << "tp,fp,tn,fn       : " << tp << ", " << fp << ", " << tn << ", " << fn << "\n"
    << "Accuracy          : " << accuracy << "%\n"
    << "Precision         : " << precision << "%\n"
    << "True negative rate: " << true_negative_rate << "%\n"
    << "Recall            : " << recall << "%" << endl;

  // Load testing data.

  // Load in dataset.
  lines.clear();
  ifstream file_tr( "data/wgss_test.csv", ios_base::in );
  while ( getline(file_tr, line, '\n') )
  {
    lines.push_back( line );
  }

  // Tokenize first line.
  l1t.clear();
  l1t = Tokenize(lines[0], "\t");

  // Create dataset.
  row_count = lines.size();
  col_count = l1t.size() - 1; // Ignore first (ID) column and last (?) col.
  feature_count = col_count - 1; // Class and n-1 features.

  // Dataset.
  Dataset dsr_test( row_count, col_count );

  // Convert data.
  for ( unsigned int row = 0; row < row_count; ++row )
  {
    // Tokenize row.
    vector<string> tokens = Tokenize(lines[row], "\t");

    // First element is the ID. Skip.
    // Second element is the class (1 - somatic, 2 - germline, 3 - wildtype).
    dsr_test[row][0] = ( atof(tokens[1].c_str()) <= 1.0 ) ? 1.0 : 0.0;

    // Fetch the rest of the features.
    for ( unsigned int col = 1; col < col_count; ++col )
    {
      dsr_test[row][col] = atof(tokens[col + 1].c_str());
    }
  }

  // Classify the training data.
  tp = 0;
  tn = 0;
  fp = 0;
  fn = 0;
  for ( unsigned int row = 0; row < dsr_test.row_count(); ++row )
  {
    bool c = forest.classify(dsr_test[row]);
    bool t = dsr_test[row][0] == 1.0;

    if ( c && t ) ++tp;
    else if ( c && !t ) ++fp;
    else if ( !c && t ) ++fn;
    else if ( !c && !t ) ++ tn;
    else cout << "????" << endl;
  }
  accuracy = (tp + tn) * 100.0 / (tp + fp + tn + fn);
  precision = tp * 100.0 / (tp + fp);
  true_negative_rate = tn * 100.0 / (tn + fp);
  recall = tp * 100.0 / (tp + fn);
  cout
    << "-----------------------------------------------------------------------\n"
    << "Testing Set Classification:\n"
    << "-----------------------------------------------------------------------\n"
    << "tp,fp,tn,fn       : " << tp << ", " << fp << ", " << tn << ", " << fn << "\n"
    << "Accuracy          : " << accuracy << "%\n"
    << "Precision         : " << precision << "%\n"
    << "True negative rate: " << true_negative_rate << "%\n"
    << "Recall            : " << recall << "%" << endl;
}
