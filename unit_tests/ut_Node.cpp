/*
 * ut_Node.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: anthony
 */

#include <cppunit/extensions/HelperMacros.h>
#include "ut_Node.h"
#include "Node.h"

#include <iostream>
#include <string>

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
