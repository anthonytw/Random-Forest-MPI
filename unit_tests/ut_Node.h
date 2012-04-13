/*
 * ut_Dataset.h
 *
 *  Created on: Mar 22, 2012
 *      Author: anthony
 */

#ifndef __ut_Dataset_h__
#define __ut_Dataset_h__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class ut_Node : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ut_Node );
    CPPUNIT_TEST( testClass_LeafNode );
    CPPUNIT_TEST( testClass_SplitNode );
    CPPUNIT_TEST( testMethod_Serialize );
  CPPUNIT_TEST_SUITE_END();

  public:
    void testClass_LeafNode( void );
    void testClass_SplitNode( void );
    void testMethod_Serialize( void );
};

#endif
