/*-----------------------------------------------------------------------
Copyright F2I-CONSULTING, (2014-2016)

philippe.verney@f2i-consulting.com

This software is a computer program whose purpose is to access to data formatted using Energistics standards.

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
-----------------------------------------------------------------------*/
#include "resqml2_0_1/IjkGridLatticeRepresentation.h"

#include <stdexcept>

#include "resqml2/AbstractFeature.h"
#include "resqml2/AbstractFeatureInterpretation.h"
#include "resqml2/AbstractLocal3dCrs.h"
#include "resqml2/AbstractValuesProperty.h"
#include "common/AbstractHdfProxy.h"

using namespace std;
using namespace gsoap_resqml2_0_1;
using namespace resqml2_0_1;

IjkGridLatticeRepresentation::IjkGridLatticeRepresentation(soap* soapContext, resqml2::AbstractLocal3dCrs * crs,
			const std::string & guid, const std::string & title,
			const unsigned int & iCount, const unsigned int & jCount, const unsigned int & kCount):
			AbstractIjkGridRepresentation(soapContext, crs, guid, title, iCount, jCount, kCount)
{
}

IjkGridLatticeRepresentation::IjkGridLatticeRepresentation(resqml2::AbstractFeatureInterpretation* interp, resqml2::AbstractLocal3dCrs * crs,
		const std::string & guid, const std::string & title,
		const unsigned int & iCount, const unsigned int & jCount, const unsigned int & kCount):
	AbstractIjkGridRepresentation(interp, crs, guid, title, iCount, jCount, kCount)
{
}

bool IjkGridLatticeRepresentation::isASeismicCube() const
{
	// A Seismic cube is defined by an IjkGridRepresentation that has a feature of type SeismicLatticeFeature and that
	// has at least one continuous property (amplitude).
	bool atLeastOneContProp = false;
	vector<resqml2::AbstractValuesProperty*> allValuesProperty = getValuesPropertySet();
    for (unsigned int propIndex = 0; propIndex < allValuesProperty.size(); ++propIndex)
    {
        if (allValuesProperty[propIndex]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCOREContinuousProperty)
        {
            atLeastOneContProp = true;
            break;
        }
    }
    if (!atLeastOneContProp)
        return false;

	return getInterpretation() && getInterpretation()->getInterpretedFeature()->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORESeismicLatticeFeature;
}

bool IjkGridLatticeRepresentation::isAFaciesCube() const
{
	// A Facies cube is defined by an IjkGridRepresentation that has a feature of type SeismicLatticeFeature and that
	// has at least one categorical property (facies).
	bool atLeastOneCateProp = false;
	vector<resqml2::AbstractValuesProperty*> allValuesProperty = getValuesPropertySet();
    for (unsigned int propIndex = 0; propIndex < allValuesProperty.size(); ++propIndex)
    {
        if (allValuesProperty[propIndex]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORECategoricalProperty)
        {
            atLeastOneCateProp = true;
            break;
        }
    }
    if (!atLeastOneCateProp)
        return false;

	return getInterpretation() && getInterpretation()->getInterpretedFeature()->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORESeismicLatticeFeature;
}

string IjkGridLatticeRepresentation::getHdfProxyUuid() const
{
	return getHdfProxyUuidFromPointGeometryPatch(getPointGeometry2_0_1(0));
}

ULONG64 IjkGridLatticeRepresentation::getXyzPointCountOfPatch(const unsigned int & patchIndex) const
{
	if (patchIndex < getPatchCount())
	{
		return (getICellCount()+1) * (getJCellCount()+1) * (getKCellCount()+1);
	}
	else
		throw range_error("An ijk grid has a maximum of one patch.");
}

void IjkGridLatticeRepresentation::getXyzPointsOfPatch(const unsigned int & patchIndex, double * xyzPoints) const
{
	if (patchIndex < getPatchCount())
	{
		throw logic_error("Not yet implemented. Please use lattice information.");
	}
	else
		throw range_error("An ijk grid has a maximum of one patch.");
}

resqml2__Point3dLatticeArray* IjkGridLatticeRepresentation::getArrayLatticeOfPoints3d() const
{
    resqml2__Point3dLatticeArray* result = nullptr;

    resqml2__PointGeometry* ptGeom = getPointGeometry2_0_1(0);
    if (ptGeom->Points->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Point3dLatticeArray)
    {
        result = static_cast<resqml2__Point3dLatticeArray*>(ptGeom->Points);
    }

    return result;
}

double IjkGridLatticeRepresentation::getXOrigin() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Origin->Coordinate1;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getXOriginInGlobalCrs() const
{
	double result[] = {getXOrigin(), getYOrigin(), .0};
	if (result[0] != result[0])
		return result[0];

	localCrs->convertXyzPointsToGlobalCrs(result, 1);

	return result[0];
}

double IjkGridLatticeRepresentation::getYOrigin() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Origin->Coordinate2;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getYOriginInGlobalCrs() const
{
	double result[] = {getXOrigin(), getYOrigin(), .0};
	if (result[0] != result[0])
		return result[0];

	localCrs->convertXyzPointsToGlobalCrs(result, 1);

	return result[1];
}

double IjkGridLatticeRepresentation::getZOrigin() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Origin->Coordinate3;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getZOriginInGlobalCrs() const
{
	double result = getZOrigin();
	if (result != result || localCrs->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORELocalTime3dCrs)
		return result;

	return result + localCrs->getOriginDepthOrElevation();
}

double IjkGridLatticeRepresentation::getXIOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[0]->Offset->Coordinate1;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getYIOffset() const
{
    resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[0]->Offset->Coordinate2;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getZIOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[0]->Offset->Coordinate3;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getXJOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[1]->Offset->Coordinate1;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getYJOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[1]->Offset->Coordinate2;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getZJOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[1]->Offset->Coordinate3;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getXKOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[2]->Offset->Coordinate1;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getYKOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[2]->Offset->Coordinate2;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getZKOffset() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoint3d = getArrayLatticeOfPoints3d();
	if (arrayLatticeOfPoint3d)
		return arrayLatticeOfPoint3d->Offset[2]->Offset->Coordinate3;
	else
		return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getISpacing() const
{
    resqml2__Point3dLatticeArray* arrayLatticeOfPoints3d = getArrayLatticeOfPoints3d();

    if (arrayLatticeOfPoints3d && arrayLatticeOfPoints3d->Offset[0]->Spacing->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
        return static_cast<resqml2__DoubleConstantArray*>(arrayLatticeOfPoints3d->Offset[0]->Spacing)->Value;
    else
        return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getJSpacing() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoints3d = getArrayLatticeOfPoints3d();

    if (arrayLatticeOfPoints3d && arrayLatticeOfPoints3d->Offset[1]->Spacing->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
        return static_cast<resqml2__DoubleConstantArray*>(arrayLatticeOfPoints3d->Offset[1]->Spacing)->Value;
    else
        return std::numeric_limits<double>::signaling_NaN();
}

double IjkGridLatticeRepresentation::getKSpacing() const
{
	resqml2__Point3dLatticeArray* arrayLatticeOfPoints3d = getArrayLatticeOfPoints3d();

    if (arrayLatticeOfPoints3d && arrayLatticeOfPoints3d->Offset[2]->Spacing->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
        return static_cast<resqml2__DoubleConstantArray*>(arrayLatticeOfPoints3d->Offset[2]->Spacing)->Value;
    else
        return std::numeric_limits<double>::signaling_NaN();
}

int IjkGridLatticeRepresentation::getOriginInline() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->InlineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
		return (int)static_cast<resqml2__DoubleLatticeArray*>(s3c->InlineCoordinates)->StartValue;
	else
		return 0;
}

int IjkGridLatticeRepresentation::getOriginCrossline() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->CrosslineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
		return (int)static_cast<resqml2__DoubleLatticeArray*>(s3c->CrosslineCoordinates)->StartValue;
	else
		return 0;
}

int IjkGridLatticeRepresentation::getInlineIOffset() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->InlineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		resqml2__DoubleLatticeArray* dla = static_cast<resqml2__DoubleLatticeArray*>(s3c->InlineCoordinates);
	    if (dla && dla->Offset[0]->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
		{
			return (int)static_cast<resqml2__DoubleConstantArray*>(dla->Offset[0])->Value;
		}
	}

	return 0;
}

int IjkGridLatticeRepresentation::getInlineJOffset() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->InlineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		resqml2__DoubleLatticeArray* dla = static_cast<resqml2__DoubleLatticeArray*>(s3c->InlineCoordinates);
	    if (dla && dla->Offset[1]->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
		{
			return (int)static_cast<resqml2__DoubleConstantArray*>(dla->Offset[1])->Value;
		}
	}

	return 0;
}

int IjkGridLatticeRepresentation::getInlineKOffset() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->InlineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		resqml2__DoubleLatticeArray* dla = static_cast<resqml2__DoubleLatticeArray*>(s3c->InlineCoordinates);
	    if (dla && dla->Offset[2]->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
		{
			return (int)static_cast<resqml2__DoubleConstantArray*>(dla->Offset[2])->Value;
		}
	}

	return 0;
}

int IjkGridLatticeRepresentation::getCrosslineIOffset() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->CrosslineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		resqml2__DoubleLatticeArray* dla = static_cast<resqml2__DoubleLatticeArray*>(s3c->CrosslineCoordinates);
	    if (dla && dla->Offset[0]->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
		{
			return (int)static_cast<resqml2__DoubleConstantArray*>(dla->Offset[0])->Value;
		}
	}

	return 0;
}

int IjkGridLatticeRepresentation::getCrosslineJOffset() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->CrosslineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		resqml2__DoubleLatticeArray* dla = static_cast<resqml2__DoubleLatticeArray*>(s3c->CrosslineCoordinates);
	    if (dla && dla->Offset[1]->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
		{
			return (int)static_cast<resqml2__DoubleConstantArray*>(dla->Offset[1])->Value;
		}
	}

	return 0;
}

int IjkGridLatticeRepresentation::getCrosslineKOffset() const
{
	resqml2__Seismic3dCoordinates* s3c = getSeismic3dCoordinates(0);

	if (s3c && s3c->CrosslineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleLatticeArray)
	{
		resqml2__DoubleLatticeArray* dla = static_cast<resqml2__DoubleLatticeArray*>(s3c->CrosslineCoordinates);
	    if (dla && dla->Offset[2]->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleConstantArray)
		{
			return (int)static_cast<resqml2__DoubleConstantArray*>(dla->Offset[2])->Value;
		}
	}

	return 0;
}

void IjkGridLatticeRepresentation::setGeometryAsCoordinateLineNodes(
	const resqml2__PillarShape & mostComplexPillarGeometry,
	const resqml2__KDirection & kDirectionKind,
	const bool & isRightHanded,
	const double & originX, const double & originY, const double & originZ,
	const double & directionIX, const double & directionIY, const double & directionIZ, const double & spacingI,
	const double & directionJX, const double & directionJY, const double & directionJZ, const double & spacingJ,
	const double & directionKX, const double & directionKY, const double & directionKZ, const double & spacingK)
{
	resqml2__IjkGridGeometry* geom = soap_new_resqml2__IjkGridGeometry(gsoapProxy2_0_1->soap, 1);
	geom->LocalCrs = localCrs->newResqmlReference();
	if (!isTruncated()) {
		getSpecializedGsoapProxy()->Geometry = geom;
	}
	else {
		getSpecializedTruncatedGsoapProxy()->Geometry = geom;
	}
	geom->GridIsRighthanded = isRightHanded;
	geom->PillarShape = mostComplexPillarGeometry;
	geom->KDirection = kDirectionKind;

	// Pillar defined
	resqml2__BooleanConstantArray* definedPillars = soap_new_resqml2__BooleanConstantArray(gsoapProxy2_0_1->soap, 1);
	geom->PillarGeometryIsDefined = definedPillars;
	definedPillars->Count = (getICellCount() + 1) * (getJCellCount() + 1);
	definedPillars->Value = true;

	// XML coordinate lines
	resqml2__Point3dLatticeArray* xmlPoints = soap_new_resqml2__Point3dLatticeArray(gsoapProxy2_0_1->soap, 1);
	geom->Points = xmlPoints;

	xmlPoints->AllDimensionsAreOrthogonal = (bool*)soap_malloc(gsoapProxy2_0_1->soap, sizeof(bool));
	*xmlPoints->AllDimensionsAreOrthogonal = true;
	xmlPoints->Origin = soap_new_resqml2__Point3d(gsoapProxy2_0_1->soap, 1);
	xmlPoints->Origin->Coordinate1 = originX;
	xmlPoints->Origin->Coordinate2 = originY;
	xmlPoints->Origin->Coordinate3 = originZ;

	// slowest axis to fastest axis so k,j,i 
	resqml2__Point3dOffset * dimK = soap_new_resqml2__Point3dOffset (gsoapProxy2_0_1->soap, 1);
	xmlPoints->Offset.push_back(dimK);
	// the dimension is the index of the axis in the collection. here we start from 0 and goes up by 1
	dimK->Offset = soap_new_resqml2__Point3d(gsoapProxy2_0_1->soap, 1);
	dimK->Offset->Coordinate1 = directionKX;
	dimK->Offset->Coordinate2 = directionKY;
	dimK->Offset->Coordinate3 = directionKZ;
	resqml2__DoubleConstantArray * xmlSpacingK = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	dimK->Spacing = xmlSpacingK;
	xmlSpacingK->Count = getKCellCount(); // number of cells on K axis
	xmlSpacingK->Value = spacingK;
	
	resqml2__Point3dOffset * dimJ = soap_new_resqml2__Point3dOffset (gsoapProxy2_0_1->soap, 1);
	xmlPoints->Offset.push_back(dimJ);
	// the dimension is the index of the axis in the collection
	dimJ->Offset = soap_new_resqml2__Point3d(gsoapProxy2_0_1->soap, 1);
	dimJ->Offset->Coordinate1 = directionJX;
	dimJ->Offset->Coordinate2 = directionJY;
	dimJ->Offset->Coordinate3 = directionJZ;
	resqml2__DoubleConstantArray * xmlSpacingJ = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	dimJ->Spacing = xmlSpacingJ;
	xmlSpacingJ->Count = getJCellCount(); // number of cells on J axis
	xmlSpacingJ->Value = spacingJ;

	resqml2__Point3dOffset * dimI = soap_new_resqml2__Point3dOffset (gsoapProxy2_0_1->soap, 1);
	xmlPoints->Offset.push_back(dimI);
	// the dimension is the index of the axis in the collection
	dimI->Offset = soap_new_resqml2__Point3d(gsoapProxy2_0_1->soap, 1);
	dimI->Offset->Coordinate1 = directionIX;
	dimI->Offset->Coordinate2 = directionIY;
	dimI->Offset->Coordinate3 = directionIZ;
	resqml2__DoubleConstantArray * xmlSpacingI = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	dimI->Spacing = xmlSpacingI;
	xmlSpacingI->Count = getICellCount(); // number of cells on I axis
	xmlSpacingI->Value = spacingI;

	return;
}

void IjkGridLatticeRepresentation::addSeismic3dCoordinatesToPatch(
								const unsigned int patchIndex,
								const double & startInline, const double & incrInline, const unsigned int & countInline,
								const double & startCrossline, const double & incrCrossline, const unsigned int & countCrossline,
								const unsigned int & countSample, AbstractRepresentation * seismicSupport)
{
	resqml2__PointGeometry* geom = getPointGeometry2_0_1(patchIndex);
	if (!geom)
		throw invalid_argument("The patchIndex does not identify a point geometry.");

	if (geom->SeismicCoordinates == nullptr)
		geom->SeismicCoordinates = soap_new_resqml2__Seismic3dCoordinates(gsoapProxy2_0_1->soap, 1);
	else if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic2dCoordinates)
		throw invalid_argument("It already exists some seismic 2d coordinates for this patch.");
	resqml2__Seismic3dCoordinates* patch = static_cast<resqml2__Seismic3dCoordinates*>(geom->SeismicCoordinates);

	patch->SeismicSupport = seismicSupport->newResqmlReference();
	pushBackSeismicSupport(seismicSupport);

	// inlines XML
	resqml2__DoubleLatticeArray* inlineValues = soap_new_resqml2__DoubleLatticeArray(gsoapProxy2_0_1->soap, 1);
	inlineValues->StartValue = startInline;

	resqml2__DoubleConstantArray * IoffsetInline = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	IoffsetInline->Count = countInline - 1;
	IoffsetInline->Value = incrInline;
	inlineValues->Offset.push_back(IoffsetInline);

	resqml2__DoubleConstantArray * IoffsetCrossline = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	IoffsetCrossline->Count = countCrossline -1;
	IoffsetCrossline->Value = 0;
	inlineValues->Offset.push_back(IoffsetCrossline);

	resqml2__DoubleConstantArray * IoffsetSample = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	IoffsetSample->Count = countSample -1;
	IoffsetSample->Value = 0;
	inlineValues->Offset.push_back(IoffsetSample);

	patch->InlineCoordinates = inlineValues;

	// crosslines XML
	resqml2__DoubleLatticeArray* crosslineValues = soap_new_resqml2__DoubleLatticeArray(gsoapProxy2_0_1->soap, 1);
	crosslineValues->StartValue = startCrossline;

	resqml2__DoubleConstantArray * CoffsetInline = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	CoffsetInline->Count = countInline - 1;
	CoffsetInline->Value = 0;
	crosslineValues->Offset.push_back(CoffsetInline);
	resqml2__DoubleConstantArray * CoffsetCrossline = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	CoffsetCrossline->Count = countCrossline - 1;
	CoffsetCrossline->Value = incrCrossline;
	crosslineValues->Offset.push_back(CoffsetCrossline);
	resqml2__DoubleConstantArray * CoffsetSample = soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
	CoffsetSample->Count = countSample - 1;
	CoffsetSample->Value = 0;
	crosslineValues->Offset.push_back(CoffsetSample);

	patch->CrosslineCoordinates = crosslineValues;
}

AbstractIjkGridRepresentation::geometryKind IjkGridLatticeRepresentation::getGeometryKind() const
{
	return LATTICE;
}
