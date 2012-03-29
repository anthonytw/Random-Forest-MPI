/*
 * ut_RandomForest.h
 *
 *  Created on: Mar 28, 2012
 *      Author: anthony
 */

#ifndef __ut_RandomForest_h__
#define __ut_RandomForest_h__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class ut_RandomForest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ut_RandomForest );
    CPPUNIT_TEST( testMethod_burn );
    CPPUNIT_TEST( testMethod_grow_forest );
    CPPUNIT_TEST( testMethod_classify );
    CPPUNIT_TEST( testMethod_classify_oob_set );
  CPPUNIT_TEST_SUITE_END();

  public:
    void testMethod_burn( void );
    void testMethod_grow_forest( void );
    void testMethod_classify( void );
    void testMethod_classify_oob_set( void );
};

#endif
