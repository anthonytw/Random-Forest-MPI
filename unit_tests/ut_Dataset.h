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

class ut_Dataset : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( ut_Dataset );
    // For ProbabilityMap subclass.
    CPPUNIT_TEST( ProbabilityMap_testMethod_entropy );

    // For IntegerColumn subclass.
    CPPUNIT_TEST( IntegerColumn_testClass );

    // For RealMatrix subclass.
    CPPUNIT_TEST( RealMatrix_testClass );

    // For Dataset main class.
    CPPUNIT_TEST( testConstructors );
    CPPUNIT_TEST( testOperator_array );
    CPPUNIT_TEST( testMethod_enumerate_threshold );
    CPPUNIT_TEST( testMethod_get_thresholds );
    CPPUNIT_TEST( testMethod_split );
    CPPUNIT_TEST( testMethod_information_gain );
    CPPUNIT_TEST( testMethod_bootstrap_sample );
    CPPUNIT_TEST( testMethod_out_of_bag_set );
  CPPUNIT_TEST_SUITE_END();

  public:
    // For ProbabilityMap subclass.
    void ProbabilityMap_testMethod_entropy( void );

    // For IntegerColumn subclass.
    void IntegerColumn_testClass( void );

    // For RealMatrix subclass.
    void RealMatrix_testClass( void );

    // For Dataset main class.
    void testConstructors( void );
    void testOperator_array( void );
    void testMethod_enumerate_threshold( void );
    void testMethod_get_thresholds( void );
    void testMethod_split( void );
    void testMethod_information_gain( void );
    void testMethod_bootstrap_sample( void );
    void testMethod_out_of_bag_set( void );
};

#endif
