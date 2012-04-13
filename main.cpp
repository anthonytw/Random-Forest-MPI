#include <iostream>
#include <map>
#include <fstream>
#include <stdlib.h>
#include <limits>

#include <openmpi/mpi.h>
//#include <mpi.h>
#include "Comm.h"

#include "RandomForest.h"
#include "Dataset.h"

using namespace std;

namespace std
{
  vector<string> Tokenize(const string& str,const string& delimiters);
}

void execute_main( const int process_count );

void execute_child( const unsigned int parent_rank, const unsigned int rank );

int main( int argc, char ** argv )
{
  int is_initialized = 0;

  if ( argc != 4 )
  {
    cout
      << "Usage: mpirun -n <1> rf <2> <3> <4>\n"
      << "  <1> - Number of nodes to use\n"
      << "  <2> - Bootstrap divisor\n"
      << "  <3> - Split keys per node\n"
      << "  <4> - Trees per forest" << endl;
    return 0;
  }

  // Determine if MPI has been initialized already.
  MPI_Initialized(&is_initialized);
  if (!is_initialized)
  {
    // Initialize MPI.
    MPI_Init(&argc, &argv);

    // Determine if this it the master process or a child.
    int rank = 0;
    int processes = 0;
    char name[MPI_MAX_PROCESSOR_NAME];
    int name_size = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Get_processor_name(name, &name_size);
    cout
      << "Spawned [ P: " << processes << " R: "
      << rank << " N: " << name << endl;

    if (rank == processes-1)
    {
      execute_main( processes );
    }
    else
    {
      // Extract parameters.
      unsigned int bootstrap_divisor = atoi(argv[1]);
      unsigned int split_keys_per_node = atoi(argv[2]);
      unsigned int trees_per_forest = atoi(argv[3]);
      execute_child(
        processes-1,
        rank,
        bootstrap_divisor,
        split_keys_per_node,
        trees_per_forest );
    }
  }

  // Finalize MPI.
  MPI_Finalize();

  return 0;
}

void execute_main( const int process_count )
{
  unsigned int child_process_count = process_count - 1;

  // Read data.
  unsigned int col_count = 107; // Ignore first (ID) column and last (?) col.

  // Buffers.
  double row_buffer[col_count];
  string line;

  cout << "Master: Loading data..." << endl;
  ifstream file( "data/seq_val_1_2.csv", ios_base::in );
  unsigned int on_child = 0;
  while ( getline(file, line, '\n') )
  {
    // Tokenize row.
    vector<string> tokens = Tokenize(line, "\t");

    // First element is the ID. Skip.
    // Second element is the class (1 - somatic, 2 - germline, 3 - wildtype).
    row_buffer[0] = ( atof(tokens[1].c_str()) <= 1.0 ) ? 1.0 : 0.0;

    // Fetch the rest of the features.
    for ( unsigned int col = 1; col < col_count; ++col )
    {
      row_buffer[col] = atof(tokens[col + 1].c_str());
    }

    // Send to child.
    MPI_Send(
      &row_buffer,
      col_count,
      MPI_DOUBLE,
      on_child,
      MessageTag::RowBuffer,
      MPI_COMM_WORLD );
    on_child = (on_child+1) % child_process_count;
  }

  // Stop the loading.
  cout << "Master: Loading finished. Sending terminate command to children..." << endl;
  row_buffer[0] = numeric_limits<double>::quiet_NaN();
  for ( unsigned int child_rank = 0; child_rank < child_process_count; ++child_rank )
  {
    // Send to child.
    MPI_Send(
      &row_buffer,
      col_count,
      MPI_DOUBLE,
      child_rank,
      MessageTag::RowBuffer,
      MPI_COMM_WORLD );
  }

  // Start forest.
  RandomForest forest;

  // Grab the trees.
  MPI_Status status;
  for ( unsigned int child_rank = 0; child_rank < child_process_count; ++child_rank )
  {
    cout << "Master: Waiting on slave " << child_rank << "..." << endl;

    // Wait on child.
    unsigned int wait = 0;
    MPI_Recv(
      &wait,
      1,
      MPI_UNSIGNED,
      child_rank,
      MessageTag::TreeFinished,
      MPI_COMM_WORLD,
      &status );

    // Load slave file.
    stringstream filename;
    filename << "data/output/slave_" << child_rank << ".tree";
    ifstream tree_in( filename.str().c_str(), ios_base::in | ios_base::binary );

    // Append forest.
    forest.deserialize_append(tree_in);

    // Done.
    tree_in.close();
  }
  cout << "Master: Loaded all forests (" << forest.get_size() << ")! Classifying..." << endl;

  // Load testing data.

  // Load in dataset.
  vector<string> lines;
  ifstream file_tr( "data/wgss_test.csv", ios_base::in );
  while ( getline(file_tr, line, '\n') )
  {
    lines.push_back( line );
  }

  // Tokenize first line.
  vector<string> l1t;
  l1t = Tokenize(lines[0], "\t");

  // Create dataset.
  unsigned int row_count = lines.size();
  col_count = l1t.size() - 1; // Ignore first (ID) column and last (?) col.

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
  unsigned int tp = 0;
  unsigned int tn = 0;
  unsigned int fp = 0;
  unsigned int fn = 0;
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
  double accuracy = (tp + tn) * 100.0 / (tp + fp + tn + fn);
  double precision = tp * 100.0 / (tp + fp);
  double true_negative_rate = tn * 100.0 / (tn + fp);
  double recall = tp * 100.0 / (tp + fn);
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

void execute_child( const unsigned int parent_rank, const unsigned int rank )
{
  unsigned int col_count = 107; // Ignore first (ID) column and last (?) col.
  unsigned int feature_count = col_count - 1; // Class and n-1 features.

  MPI_Status status;

  cout << "Slave " << rank << ": Waiting on rows..." << endl;
  vector<double> data;
  double buffer[col_count];
  buffer[0] = 0.0;
  while ( true )
  {
    MPI_Recv(
      &buffer,
      col_count,
      MPI_DOUBLE,
      parent_rank,
      MessageTag::RowBuffer,
      MPI_COMM_WORLD,
      &status );

    // Stop on NaN.
    if ( buffer[0] != buffer[0] )
    {
      break;
    }

    // Add elements to data vector.
    for ( unsigned int col = 0; col < col_count; ++col )
    {
      data.push_back(buffer[col]);
    }
  }

  cout << "Slave " << rank << ": Finished loading data..." << endl;

  // Generate matrix.
  Dataset::RealMatrix data_matrix(&data[0], data.size()/col_count, col_count);

  // Generate dataset.
  Dataset dsr(data_matrix, data_matrix.row_count());
  for ( unsigned int row = 0; row < dsr.row_count(); ++row)
  {
    dsr(row) = row;
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
  cout << "Slave " << rank << ": Growing tree..." << endl;
  RandomForest forest;
  forest.grow_forest( dsr, 0, dsr.row_count() / 3, split_keys, 16, 5 );
  cout << "Slave " << rank << ": Grown! Seralizing and sending..." << endl;

  stringstream filename;
  filename << "data/output/slave_" << rank << ".tree";
  ofstream tree_out( filename.str().c_str(), ios_base::out | ios_base::binary );
  forest.serialize(tree_out);
  tree_out.close();

  cout << "Slave " << rank << ": Finished." << endl;

  // Send finished signal.
  unsigned int wait = 1;
  MPI_Send(
    &wait,
    1,
    MPI_UNSIGNED,
    parent_rank,
    MessageTag::TreeFinished,
    MPI_COMM_WORLD );
}

vector<string> std::Tokenize(const string& str,const string& delimiters)
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
