/*
 * ut_RandomTree.h
 *
 *  Created on: Mar 26, 2012
 *      Author: anthony
 */

#ifndef __ut_RandomTree_h__
#define __ut_RandomTree_h__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class ut_RandomTree : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ut_RandomTree );
    CPPUNIT_TEST( testConstructor );
    CPPUNIT_TEST( testMethod_draw );
    CPPUNIT_TEST( testMethod_grow_decision_tree );
    CPPUNIT_TEST( testMethod_classify );
    CPPUNIT_TEST( testMethod_classify_oob_set );
  CPPUNIT_TEST_SUITE_END();

  public:
    void testConstructor( void );
    void testMethod_draw( void );
    void testMethod_grow_decision_tree( void );
    void testMethod_classify( void );
    void testMethod_classify_oob_set( void );
};

#endif
