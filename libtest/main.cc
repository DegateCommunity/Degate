/*
 
 This file is part of the IC reverse engineering tool degate.
 
 Copyright 2008, 2009 by Martin Schobert
 
 Degate is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version.
 
 Degate is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with degate. If not, see <http://www.gnu.org/licenses/>.
 
*/

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>

#include "FileSystemTest.h"
#include "ShapeTest.h"
#include "MemoryMapTest.h"
#include "ImageTest.h"
#include "QuadTreeTest.h"
#include "LMOinQTreeTest.h"
#include "ProjectImporterTest.h"
#include "LogicModelImporterTest.h"
#include "GateLibraryImporterTest.h"
#include "GateLibraryExporterTest.h"
#include "LogicModelTest.h"
#include "LogicModelExporterTest.h"
#include "ProjectExporterTest.h"
#include "LogicModelDOTExporterTest.h"
#include "ScalingManagerTest.h"
#include "ImageProcessingTest.h"

using namespace degate;

int main(void) {

  // Informiert Test-Listener ueber Testresultate
  CPPUNIT_NS :: TestResult testresult;

  // Listener zum Sammeln der Testergebnisse registrieren
  CPPUNIT_NS :: TestResultCollector collectedresults;
  testresult.addListener (&collectedresults);

  // Listener zur Ausgabe der Ergebnisse einzelner Tests
  CPPUNIT_NS :: BriefTestProgressListener progress;
  testresult.addListener (&progress);


  // Test-Suite ueber die Registry im Test-Runner einfuegen
  CPPUNIT_NS :: TestRunner testrunner;


  testrunner.addTest(FileSystemTest::suite());
  //testrunner.addTest(ShapeTest::suite());
  testrunner.addTest(MemoryMapTest::suite());
  //  testrunner.addTest(ImageTest::suite());
  testrunner.addTest(QuadTreeTest::suite());
  
  testrunner.addTest(LogicModelTest::suite());
  testrunner.addTest(LMOinQTreeTest::suite());

  /*
  testrunner.addTest(GateLibraryImporterTest::suite());
  testrunner.addTest(LogicModelImporterTest::suite());
  testrunner.addTest(ProjectImporterTest::suite());

  testrunner.addTest(GateLibraryExporterTest::suite());
  testrunner.addTest(LogicModelExporterTest::suite());
  testrunner.addTest(ProjectExporterTest::suite());
  
  testrunner.addTest(LogicModelDOTExporterTest::suite());
  //testrunner.addTest(ScalingManagerTest::suite());

  //testrunner.addTest(ImageProcessingTest::suite());
  */
  testrunner.run(testresult);

  // Resultate im Compiler-Format ausgeben
  CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults, std::cerr);
  compileroutputter.write();

  // Rueckmeldung, ob Tests erfolgreich waren
  return collectedresults.wasSuccessful() ? 0 : 1;


}
