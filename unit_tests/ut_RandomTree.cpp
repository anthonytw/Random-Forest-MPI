/*
 * ut_RandomTree.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: anthony
 */

#include <cppunit/extensions/HelperMacros.h>
#include "ut_RandomTree.h"
#include "RandomTree.h"
#include "Dataset.h"

#include <string>
#include <stdlib.h>

using namespace std;

//CPPUNIT_TEST_SUITE_REGISTRATION( ut_RandomTree );

namespace ut_RandomTree_ns
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

//------------------------------------------------------------------------------
void ut_RandomTree::testConstructor( void )
{
  // Create RandomTree.
  RandomTree tree;
  CPPUNIT_ASSERT_EQUAL( null(Node), tree.get_root() );
}

//------------------------------------------------------------------------------

void ut_RandomTree::testMethod_draw( void )
{
  // Create a RandomTree.
  RandomTree tree;

  // Add node heirarchy.
  Node *root = new SplitNode("[root]", 1, 0.0);

  // First branch.
  Node *branchA = new SplitNode("[2] >= 5.0", 2, 5.0);

  Node *branchA_1 = new SplitNode("[3] >= 2.3", 3, 2.3);
  Node *branchA_1a = new LeafNode("[4] >= 6.4", true);
  Node *branchA_1b = new LeafNode("[4] < 6.4", false);
  branchA_1->add_child(branchA_1a);
  branchA_1->add_child(branchA_1b);

  Node *branchA_2 = new LeafNode("[3] < 2.3", true);

  branchA->add_child(branchA_1);
  branchA->add_child(branchA_2);

  // Second branch.
  Node *branchB = new LeafNode("[2] < 5.0", false);

  root->add_child(branchA);
  root->add_child(branchB);

  // Set root node.
  tree.root = root;

  // Draw.
  string output = tree.draw();

  // Test.
  string expectedOutput = "graph G{\n\
N0[shape=box,label=\"([root])\\n1\"];\n\
N0 -- N1;\n\
N0 -- N2;\n\
N1[shape=box,label=\"([2] >= 5.0)\\n2\"];\n\
N1 -- N3;\n\
N1 -- N4;\n\
N2[shape=ellipse,label=\"([2] < 5.0)\\n0\"];\n\
N3[shape=box,label=\"([3] >= 2.3)\\n3\"];\n\
N3 -- N5;\n\
N3 -- N6;\n\
N4[shape=ellipse,label=\"([3] < 2.3)\\n1\"];\n\
N5[shape=ellipse,label=\"([4] >= 6.4)\\n1\"];\n\
N6[shape=ellipse,label=\"([4] < 6.4)\\n0\"];\n\
}";
  CPPUNIT_ASSERT_EQUAL( expectedOutput, output );
}

//------------------------------------------------------------------------------

void ut_RandomTree::testMethod_grow_decision_tree( void )
{
  // Build some sample dataset.
  /*
   *    x A B C
   *    0 1 9 7
   *    0 2 9 3
   *    0 3 9 4
   *    0 4 9 2
   *    1 5 9 6
   *    1 6 1 1
   *    1 7 1 5
   */
  Dataset ds( 7, 4 );
  ds[0][0] = 0; ds[0][1] = 1; ds[0][2] = 9; ds[0][3] = 7;
  ds[1][0] = 0; ds[1][1] = 2; ds[1][2] = 9; ds[1][3] = 3;
  ds[2][0] = 0; ds[2][1] = 3; ds[2][2] = 9; ds[2][3] = 4;
  ds[3][0] = 0; ds[3][1] = 4; ds[3][2] = 9; ds[3][3] = 2;
  ds[4][0] = 1; ds[4][1] = 5; ds[4][2] = 9; ds[4][3] = 6;
  ds[5][0] = 1; ds[5][1] = 6; ds[5][2] = 1; ds[5][3] = 1;
  ds[6][0] = 1; ds[6][1] = 7; ds[6][2] = 1; ds[6][3] = 5;
  Dataset::KeyList & keys = ds.get_keys();
  keys["x"] = 0;
  keys["A"] = 1;
  keys["B"] = 2;
  keys["C"] = 3;

  // Build a random tree.
  Dataset::KeyList split_keys;
  split_keys["A"] = 1;
  split_keys["B"] = 2;
  split_keys["C"] = 3;
  RandomTree tree;
  tree.grow_decision_tree( ds, split_keys, 2u, 0u );

  // Draw tree.
  //cout << "Draw tree:" << endl;
  //cout << tree.draw() << endl;
}

//------------------------------------------------------------------------------

void ut_RandomTree::testMethod_classify( void )
{
  // Build some sample dataset.
  /*
   *    x A B C
   *    0 1 9 7
   *    0 2 9 3
   *    0 3 9 4
   *    0 4 9 2
   *    1 5 9 6
   *    1 6 1 1
   *    1 7 1 5
   */
  Dataset ds( 7, 4 );
  ds[0][0] = 0; ds[0][1] = 1; ds[0][2] = 9; ds[0][3] = 7;
  ds[1][0] = 0; ds[1][1] = 2; ds[1][2] = 9; ds[1][3] = 3;
  ds[2][0] = 0; ds[2][1] = 3; ds[2][2] = 9; ds[2][3] = 4;
  ds[3][0] = 0; ds[3][1] = 4; ds[3][2] = 9; ds[3][3] = 2;
  ds[4][0] = 1; ds[4][1] = 5; ds[4][2] = 9; ds[4][3] = 6;
  ds[5][0] = 1; ds[5][1] = 6; ds[5][2] = 1; ds[5][3] = 1;
  ds[6][0] = 1; ds[6][1] = 7; ds[6][2] = 1; ds[6][3] = 5;
  Dataset::KeyList & keys = ds.get_keys();
  keys["x"] = 0;
  keys["A"] = 1;
  keys["B"] = 2;
  keys["C"] = 3;

  // Build a random tree.
  Dataset::KeyList split_keys;
  split_keys["A"] = 1;
  split_keys["B"] = 2;
  split_keys["C"] = 3;
  RandomTree tree;
  tree.grow_decision_tree( ds, split_keys, 2u, 0u );

  // Classify data. All should pass.
  for ( unsigned int row = 0; row < ds.row_count(); ++row )
  {
    bool correct_class = ds[row][0];
    bool classifier_return = tree.classify( ds[row] );
    CPPUNIT_ASSERT_EQUAL( correct_class, classifier_return );
  }
}

//------------------------------------------------------------------------------

void ut_RandomTree::testMethod_classify_oob_set( void )
{
}

//------------------------------------------------------------------------------
