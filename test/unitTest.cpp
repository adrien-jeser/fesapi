// unitTest.cpp�: d�finit le point d'entr�e pour l'application console.
//

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#endif

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"

#include "EpcDocumentTest.h"
#include "resqml2_0_1test/LocalDepth3dCrsTest.h"
#include "resqml2_0_1test/HorizonTest.h"
#include "resqml2_0_1test/HorizonInterpretationTest.h"
#include "resqml2_0_1test/FaultSinglePatchTriangulatedSetRepresentationTest.h"
#include "resqml2_0_1test/FaultMultiPatchTriangulatedSetRepresentationTest.h"
#include "resqml2_0_1test/FaultSingleAndMultiPatchTriangulatedSetRepresentationTest.h"
#include "resqml2_0_1test/ActivityTemplateGenericCreationTest.h"
#include "resqml2_0_1test/ActivityCreationTest.h"
#include "resqml2_0_1test/OneTetrahedronUnstructuredGridRepresentationTest.h"
#include "resqml2_0_1test/UnstructuredFromIjkGridRepresentationTest.h"
#include "resqml2_0_1test/TimeSeriesTest.h"
#include "resqml2_0_1test/CommentProperty.h"
#include "resqml2_0_1test/ContinuousPropertyOnWellFrameTest.h"
#include "resqml2_0_1test/ContinuousPropertySeriesTest.h"
#include "resqml2_0_1test/GridConnectionSetOnPartialGridSet.h"
#include "resqml2_0_1test/WellboreMarkerFrameRepresentationTest.h"
#include "resqml2_0_1test/WellboreFrameRepresentationTest.h"
#include "resqml2_0_1test/WellboreRegularFrameRepresentationTest.h"
#include "resqml2_0_1test/DiscretePropertyUsingLocalKindOnWellFrameTest.h"
#include "resqml2_0_1test/HorizonOnSeismicLine.h"
#include "resqml2_0_1test/RightHanded4x3x2ExplicitIjkGrid.h"
#include "resqml2_0_1test/SubRepresentationOnPartialGridConnectionSet.h"
#include "resqml2_0_1test/LgrOnRightHanded4x3x2ExplicitIjkGrid.h"
#include "resqml2_0_1test/InterpretationDomain.h"

using namespace commontest;
using namespace resqml2_0_1test;

#define FESAPI_TEST(name, tags, classTest)  TEST_CASE(name, tags)\
{\
	classTest* test = new classTest("../../" #classTest ".epc");\
	test->serialize();\
	test->deserialize();\
	delete test;\
}
/*
TEST_CASE( "Deserialize an EPC document", "[epc]")
{
	EpcDocumentTest* test = new EpcDocumentTest("../../testingPackageCpp.epc");
	test->deserialize();
	delete test;
}
*/
FESAPI_TEST("Export and import a local depth 3d crs", "[crs]", LocalDepth3dCrsTest)

FESAPI_TEST("Export and import an horizon", "[feature]", HorizonTest)

FESAPI_TEST("Export and import an horizon interpretation", "[interpretation]", HorizonInterpretationTest)

FESAPI_TEST("Export and import a single patch triangulated set fault representation", "[representation]", FaultSinglePatchTriangulatedSetRepresentationTest)

FESAPI_TEST("Export and import a multi patch triangulated set fault representation", "[representation]", FaultMultiPatchTriangulatedSetRepresentationTest)

FESAPI_TEST("Export and import a single and multi patch triangulated set fault representation", "[representation]", FaultSingleAndMultiPatchTriangulatedSetRepresentationTest)

FESAPI_TEST("Export a multi domains interpreation", "[interpretation]", InterpretationDomain)

FESAPI_TEST("Export and import a generic creation activity template", "[activity]", ActivityTemplateGenericCreationTest)

FESAPI_TEST("Export and import an activity", "[activity]", ActivityCreationTest)

FESAPI_TEST("Export and import a 4*3*2 explicit right handed ijk grid", "[grid]", RightHanded4x3x2ExplicitIjkGrid)

FESAPI_TEST("Export and import a LGR on a 4*3*2 explicit right handed ijk grid", "[grid]", LgrOnRightHanded4x3x2ExplicitIjkGrid)

FESAPI_TEST("Export and import an unstructured grid", "[grid]", OneTetrahedronUnstructuredGridRepresentationTest)

FESAPI_TEST("Export and import a subrepresenation on a partial grid connection set", "[grid]", SubRepresentationOnPartialGridConnectionSet)

// fail
//FESAPI_TEST("Ijk to unstructured grid", "[grid]", UnstructuredFromIjkGridRepresentationTest)

FESAPI_TEST("Export and import a time series", "[property]", TimeSeriesTest)

FESAPI_TEST("Export and import continuous property series", "[property]", ContinuousPropertySeriesTest)

// fail
//FESAPI_TEST("Export and import grid connection set on partial grid", "[property]", GridConnectionSetOnPartialGridSet)

FESAPI_TEST("Export and import a wellbore marker frame", "[well][stratigraphy]", WellboreMarkerFrameRepresentationTest)

FESAPI_TEST("Export and import a wellbore frame (regular md spacings)", "[well]", WellboreRegularFrameRepresentationTest)

FESAPI_TEST("Export and import a wellbore frame (irregular md spacings) with a single continuous property on node", "[well][property]", ContinuousPropertyOnWellFrameTest)
FESAPI_TEST("Export and import a wellbore frame (irregular md spacings) with a single comment property on node", "[well][property]", CommentProperty)

FESAPI_TEST("Export and import a wellbore frame (irregular md spacings) with a single discrete property on node using a local property kind", "[well][property]", DiscretePropertyUsingLocalKindOnWellFrameTest)

FESAPI_TEST("Export and import an horizon on a seismic line", "[seismic]", HorizonOnSeismicLine)
