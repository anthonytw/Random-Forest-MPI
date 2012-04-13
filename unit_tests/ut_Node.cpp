/*
 * ut_Node.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: anthony
 */

#include <cppunit/extensions/HelperMacros.h>
#include "ut_Node.h"
#include "Node.h"
#include "RandomTree.h"

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( ut_Node );

//------------------------------------------------------------------------------

void ut_Node::testClass_LeafNode( void )
{
  const std::string action  = "Some Action";
  const bool classification = true;
  const std::string draw = "shape=ellipse,label=\"(Some Action)\\n1\"";

  // Create leaf node.
  LeafNode node( action, classification );
  CPPUNIT_ASSERT_EQUAL( action, node.get_action() );
  CPPUNIT_ASSERT_EQUAL( classification, node.get_classification() );
  CPPUNIT_ASSERT_EQUAL( true, node.is_leaf() );
  CPPUNIT_ASSERT_EQUAL( draw, node.draw() );

  // Attempt to add children.
  LeafNode newChild( action, classification );
  Node::NodeSet set = node.get_children();
  CPPUNIT_ASSERT_EQUAL( 0u, static_cast<unsigned int>(set.size()) );
  node.add_child( &newChild );
  set = node.get_children();
  CPPUNIT_ASSERT_EQUAL( 0u, static_cast<unsigned int>(set.size()) );
}

//------------------------------------------------------------------------------

void ut_Node::testClass_SplitNode( void )
{
  const std::string   action    = "Some Action";
  const unsigned int  column    =  2;
  const double        threshold = 17.5;
  const std::string   draw      = "shape=box,label=\"(Some Action)\\n2\"";

  // Create split node.
  SplitNode node( action, column, threshold );
  CPPUNIT_ASSERT_EQUAL( action, node.get_action() );
  CPPUNIT_ASSERT_EQUAL( false, node.get_classification() );
  CPPUNIT_ASSERT_EQUAL( false, node.is_leaf() );
  CPPUNIT_ASSERT_EQUAL( threshold, node.get_threshold() );
  CPPUNIT_ASSERT_EQUAL( draw, node.draw() );

  // Attempt to add children.
  SplitNode * newChild = new SplitNode( action, column, threshold );
  Node::NodeSet set = node.get_children();
  CPPUNIT_ASSERT_EQUAL( 0u, static_cast<unsigned int>(set.size()) );
  node.add_child( newChild );
  set = node.get_children();
  CPPUNIT_ASSERT_EQUAL( 1u, static_cast<unsigned int>(set.size()) );
  CPPUNIT_ASSERT_EQUAL( newChild, dynamic_cast<SplitNode*>(set[0]) );
}

//------------------------------------------------------------------------------

void ut_Node::testMethod_Serialize( void )
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

  // Serialize tree.
  ofstream stream("data/output/serialization_test.bin", ios_base::out | ios_base::binary);
  Node::serialize( stream, *(tree.root) );
  stream.close();

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

  // Deserialize tree.
  ifstream stream_in("data/output/serialization_test.bin", ios_base::in | ios_base::binary);
  Node * new_root = Node::deserialize( stream_in );
  stream_in.close();

  // Draw.
  tree.root = new_root;
  output = tree.draw();
  CPPUNIT_ASSERT_EQUAL( expectedOutput, output );
}

//------------------------------------------------------------------------------
