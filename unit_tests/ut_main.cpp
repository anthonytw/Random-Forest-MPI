/*
 * ut_main.cpp
 *
 *  Created on: Mar 22, 2012
 *      Author: anthony
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/BriefTestProgressListener.h>

#include <iostream>

using namespace std;

int main( int argc, char **argv )
{
  CppUnit::TestResult controller;
  CppUnit::TestResultCollector results;
  CppUnit::BriefTestProgressListener progressListener;
  controller.addListener( &results );
  controller.addListener( &progressListener );

  CppUnit::TextOutputter consoleOutputter( &results, cout );

  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  runner.run( controller );

  consoleOutputter.write();
  consoleOutputter.printStatistics();

  return results.wasSuccessful() ? 0 : 1;
}
