/*-----------------------------------------------------------------------
Copyright F2I-CONSULTING, (2014-2017)

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
#include "resqml2/AbstractRepresentation.h"

#include <sstream>
#include <algorithm>
#include <stdexcept>

#include "H5public.h"

#include "resqml2_0_1/AbstractTechnicalFeature.h"
#include "resqml2/AbstractFeatureInterpretation.h"
#include "resqml2/RepresentationSetRepresentation.h"
#include "resqml2/AbstractValuesProperty.h"
#include "resqml2/SubRepresentation.h"
#include "common/AbstractHdfProxy.h"
#include "resqml2/AbstractLocal3dCrs.h"

using namespace resqml2;
using namespace std;
using namespace epc;

const char* AbstractRepresentation::XML_TAG = "AbstractRepresentation";

AbstractRepresentation::AbstractRepresentation(AbstractFeatureInterpretation* interp, AbstractLocal3dCrs * crs): interpretation(nullptr), hdfProxy(nullptr), localCrs(nullptr)
{
}

void AbstractRepresentation::pushBackSeismicSupport(AbstractRepresentation * seismicSupport)
{
	seismicSupportSet.push_back(seismicSupport);
	if (std::find(seismicSupportedRepSet.begin(), seismicSupportedRepSet.end(), this) == seismicSupportedRepSet.end())
		seismicSupport->seismicSupportedRepSet.push_back(this);
}

void AbstractRepresentation::setXmlInterpretation(AbstractFeatureInterpretation * interp)
{
	if (gsoapProxy2_0_1 != nullptr) {
		static_cast<gsoap_resqml2_0_1::resqml2__AbstractRepresentation*>(gsoapProxy2_0_1)->RepresentedInterpretation = interp->newResqmlReference();
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

std::string AbstractRepresentation::getHdfProxyUuidFromPointGeometryPatch(gsoap_resqml2_0_1::resqml2__PointGeometry* patch) const
{
	string result = "";
	if (patch != nullptr) {
		if (patch->Points->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Point3dZValueArray) {
			gsoap_resqml2_0_1::resqml2__Point3dZValueArray* const tmp = static_cast<gsoap_resqml2_0_1::resqml2__Point3dZValueArray* const>(patch->Points);
			if (tmp->ZValues->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array) {
				return static_cast<gsoap_resqml2_0_1::resqml2__DoubleHdf5Array* const>(tmp->ZValues)->Values->HdfProxy->UUID;
			}
		}
		else if (patch->Points->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Point3dHdf5Array) {
			return static_cast<gsoap_resqml2_0_1::resqml2__Point3dHdf5Array* const>(patch->Points)->Coordinates->HdfProxy->UUID;
		}
	}

	return result;
}

gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates* AbstractRepresentation::getSeismic2dCoordinates(const unsigned int & patchIndex) const
{
	if (gsoapProxy2_0_1 != nullptr) {
		gsoap_resqml2_0_1::resqml2__PointGeometry* const geom = getPointGeometry2_0_1(patchIndex);
		if (geom == nullptr) {
			return nullptr;
		}

		if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic2dCoordinates) {
			return static_cast<gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates*>(geom->SeismicCoordinates);
		}

		return nullptr;
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates* AbstractRepresentation::getSeismic3dCoordinates(const unsigned int & patchIndex) const
{
	if (gsoapProxy2_0_1 != nullptr) {
		gsoap_resqml2_0_1::resqml2__PointGeometry* const geom = getPointGeometry2_0_1(patchIndex);
		if (geom == nullptr) {
			return nullptr;
		}

		if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic3dCoordinates) {
			return static_cast<gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates*>(geom->SeismicCoordinates);
		}
		
		return nullptr;
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

gsoap_resqml2_0_1::resqml2__PointGeometry* AbstractRepresentation::createPointGeometryPatch2_0_1(const unsigned int & patchIndex,
	double * points, hsize_t * numPoints, const unsigned int & numDimensionsInArray, common::AbstractHdfProxy * proxy)
{
	if (gsoapProxy2_0_1 != nullptr) {
		setHdfProxy(proxy);

		gsoap_resqml2_0_1::resqml2__PointGeometry* const geom = gsoap_resqml2_0_1::soap_new_resqml2__PointGeometry(gsoapProxy2_0_1->soap, 1);
		geom->LocalCrs = localCrs->newResqmlReference();

		// XML
		gsoap_resqml2_0_1::resqml2__Point3dHdf5Array* xmlPts = gsoap_resqml2_0_1::soap_new_resqml2__Point3dHdf5Array(gsoapProxy2_0_1->soap, 1);
		xmlPts->Coordinates = gsoap_resqml2_0_1::soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
		xmlPts->Coordinates->HdfProxy = hdfProxy->newResqmlReference();
		ostringstream oss;
		oss << "points_patch" << patchIndex;
		xmlPts->Coordinates->PathInHdfFile = "/RESQML/" + getUuid() + "/" + oss.str();
		geom->Points = xmlPts;

		// HDF
		hsize_t * const numValues = new hsize_t[numDimensionsInArray + 1];
		for (hsize_t i = 0; i < numDimensionsInArray; ++i) {
			numValues[i] = numPoints[i];
		}
		numValues[numDimensionsInArray] = 3; // 3 for X, Y and Z

		hdfProxy->writeArrayNdOfDoubleValues(getUuid(), oss.str(), points, numValues, numDimensionsInArray + 1);

		delete[] numValues;

		return geom;
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

AbstractLocal3dCrs * AbstractRepresentation::getLocalCrs() const
{
	return localCrs;
}

gsoap_resqml2_0_1::eml20__DataObjectReference* AbstractRepresentation::getLocalCrsDor() const
{
	if (gsoapProxy2_0_1 != nullptr) {
		gsoap_resqml2_0_1::resqml2__PointGeometry* pointGeom = getPointGeometry2_0_1(0);
		if (pointGeom != nullptr) {
			return pointGeom->LocalCrs;
		}
		else {
			return nullptr;
		}
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

std::string AbstractRepresentation::getLocalCrsUuid() const
{
	gsoap_resqml2_0_1::eml20__DataObjectReference* dor = getLocalCrsDor();
	return dor == nullptr ? string() : dor->UUID;
}

common::AbstractHdfProxy * AbstractRepresentation::getHdfProxy() const
{
	return hdfProxy;
}

const std::vector<AbstractProperty*> & AbstractRepresentation::getPropertySet() const
{
	return propertySet;
}

std::vector<AbstractValuesProperty*> AbstractRepresentation::getValuesPropertySet() const
{
	std::vector<AbstractValuesProperty*> result;

	for (size_t i = 0; i < propertySet.size(); ++i)
	{
		if (propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCOREContinuousProperty ||
			propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORECategoricalProperty ||
			propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCOREDiscreteProperty ||
			propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORECommentProperty
#ifdef WITH_RESQML2_1
			|| propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCOREContinuousProperty ||
			propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORECategoricalProperty ||
			propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCOREDiscreteProperty ||
			propertySet[i]->getGsoapType() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__obj_USCORECommentProperty
#endif
			) {
			result.push_back(static_cast<AbstractValuesProperty*>(propertySet[i]));
		}
	}

	return result;
}

unsigned int AbstractRepresentation::getValuesPropertyCount() const
{
	return getValuesPropertySet().size();
}

AbstractValuesProperty* AbstractRepresentation::getValuesProperty(const unsigned int & index) const
{
	std::vector<AbstractValuesProperty*> set = getValuesPropertySet();

	if (set.size() > index) {
		return getValuesPropertySet()[index];
	}
	else {
		throw out_of_range("The index of the property to get is out of range of the array of properties for this representation.");
	}
}

void AbstractRepresentation::setInterpretation(AbstractFeatureInterpretation * interp)
{
	if (interp == nullptr) {
		throw invalid_argument("Cannot set a null interpretation to a representation");
	}

	// EPC
	interpretation = interp;
	interpretation->representationSet.push_back(this);

	// XML
	if (updateXml) {
		setXmlInterpretation(interp);
		interpretation->initDomain(gsoap_resqml2_0_1::resqml2__Domain__mixed);
	}
}

AbstractFeatureInterpretation* AbstractRepresentation::getInterpretation() const
{
	return interpretation;
}

gsoap_resqml2_0_1::eml20__DataObjectReference* AbstractRepresentation::getInterpretationDor() const
{
	if (gsoapProxy2_0_1 != nullptr) {
		return static_cast<gsoap_resqml2_0_1::resqml2__AbstractRepresentation*>(gsoapProxy2_0_1)->RepresentedInterpretation != nullptr ?
			static_cast<gsoap_resqml2_0_1::resqml2__AbstractRepresentation*>(gsoapProxy2_0_1)->RepresentedInterpretation : nullptr;
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

std::string AbstractRepresentation::getInterpretationUuid() const
{
	gsoap_resqml2_0_1::eml20__DataObjectReference* dor = getInterpretationDor();
	return dor == nullptr ? string() : dor->UUID;
}

std::string AbstractRepresentation::getInterpretationContentType() const
{
	gsoap_resqml2_0_1::eml20__DataObjectReference* dor = getInterpretationDor();
	return dor == nullptr ? string() : dor->ContentType;
}

void AbstractRepresentation::pushBackSubRepresentation(SubRepresentation* subRep)
{
	if (subRep == nullptr) {
		throw invalid_argument("Cannot add a null subrepresentation to a representation");
	}

	subRepresentationSet.push_back(subRep);
}

std::vector<SubRepresentation*> AbstractRepresentation::getSubRepresentationSet() const
{
	return subRepresentationSet;
}

unsigned int AbstractRepresentation::getSubRepresentationCount() const
{
	return subRepresentationSet.size();
}

SubRepresentation* AbstractRepresentation::getSubRepresentation(const unsigned int & index) const
{
	if (index >= subRepresentationSet.size()) {
		throw range_error("The subrepresentation at the specified index is out of range.");
	}

	return subRepresentationSet[index];
}

std::vector<SubRepresentation*> AbstractRepresentation::getFaultSubRepresentationSet() const
{
	std::vector<SubRepresentation*> result;

	for (size_t i = 0; i <subRepresentationSet.size(); ++i) {
		if (subRepresentationSet[i]->getInterpretation() != nullptr && subRepresentationSet[i]->getInterpretation()->getXmlTag() == "FaultInterpretation") {
			result.push_back(subRepresentationSet[i]);
		}
	}

	return result;
}

unsigned int AbstractRepresentation::getFaultSubRepresentationCount() const
{
	return getFaultSubRepresentationSet().size();
}

SubRepresentation* AbstractRepresentation::getFaultSubRepresentation(const unsigned int & index) const
{
	std::vector<resqml2::SubRepresentation*> tmp = getFaultSubRepresentationSet();

	if (index >= tmp.size()) {
		throw range_error("The fault subrepresentation at the specified index is out of range.");
	}
	
	return tmp[index];
}

ULONG64 AbstractRepresentation::getXyzPointCountOfAllPatches() const
{
	ULONG64 result = 0;

	unsigned int patchCount = getPatchCount();
	for (unsigned int patchIndex = 0; patchIndex < patchCount; ++patchIndex)
	{
		result += getXyzPointCountOfPatch(patchIndex);
	}

	return result;
}

void AbstractRepresentation::getXyzPointsOfPatchInGlobalCrs(const unsigned int & patchIndex, double * xyzPoints) const
{
	getXyzPointsOfPatch(patchIndex, xyzPoints);

	localCrs->convertXyzPointsToGlobalCrs(xyzPoints, getXyzPointCountOfPatch(patchIndex));
}

void AbstractRepresentation::getXyzPointsOfAllPatches(double * xyzPoints) const
{
	unsigned int patchCount = getPatchCount();
	getXyzPointsOfPatch(0, xyzPoints);
	for (unsigned int patchIndex = 1; patchIndex < patchCount; patchIndex++)
	{
		xyzPoints += getXyzPointCountOfPatch(patchIndex - 1) * 3;
		getXyzPointsOfPatch(patchIndex, xyzPoints);
	}
}

void AbstractRepresentation::getXyzPointsOfAllPatchesInGlobalCrs(double * xyzPoints) const
{
	getXyzPointsOfAllPatches(xyzPoints);

	localCrs->convertXyzPointsToGlobalCrs(xyzPoints, getXyzPointCountOfAllPatches());
}

/*
void AbstractRepresentation::getInlinesOfPointsOfPatch(const unsigned int & patchIndex, double * values)
{
	gsoap_resqml2_0_1::resqml2__cs_USCOREseismic3dCoordinatesInfo* seisInfo = getSeismic3dCoordinatesInfo();
	if (!seisInfo)
		return;

	if (seisInfo->patch[patchIndex]->inlineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array)
	{
		resqml2__DoubleHdf5Array* dblValues = static_cast<resqml2__DoubleHdf5Array*>(seisInfo->patch[patchIndex]->inlineCoordinates);
		hdfProxy->readArrayNdOfDoubleValues(dblValues->Values->PathInHdfFile, values);
	}
}

void AbstractRepresentation::getCrosslinesOfPointsOfPatch(const unsigned int & patchIndex, double * values)
{
	gsoap_resqml2_0_1::resqml2__cs_USCOREseismic3dCoordinatesInfo* seisInfo = getSeismic3dCoordinatesInfo();
	if (!seisInfo)
		return;

	if (seisInfo->patch[patchIndex]->crosslineCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array)
	{
		resqml2__DoubleHdf5Array* dblValues = static_cast<resqml2__DoubleHdf5Array*>(seisInfo->patch[patchIndex]->crosslineCoordinates);
		hdfProxy->readArrayNdOfDoubleValues(dblValues->Values->PathInHdfFile, values);
	}
}
*/

AbstractRepresentation* AbstractRepresentation::getSeismicSupportOfPatch(const unsigned int & patchIndex)
{
	if (patchIndex >= seismicSupportSet.size()) {
		throw range_error("The seismic support of the patch at the specified index is out of range.");
	}

	return seismicSupportSet[patchIndex];
}

void AbstractRepresentation::pushBackIntoRepresentationSet(RepresentationSetRepresentation * repSet, bool xml)
{
	if (repSet == nullptr) {
		throw invalid_argument("The representation set representation which should contain this representation cannot be null.");
	}

	// Add the relationship
	representationSetRepresentationSet.push_back(repSet);

	// XML
	if (xml) {
		repSet->pushBackXmlRepresentation(this);
	}
}

ULONG64 AbstractRepresentation::getRepresentationSetRepresentationCount() const
{
	return representationSetRepresentationSet.size();
}

RepresentationSetRepresentation* AbstractRepresentation::getRepresentationSetRepresentation(const ULONG64  & index) const
{
	if (index >= getRepresentationSetRepresentationCount()) {
		throw range_error("The index of the representation set representation is out of range.");
	}

	return representationSetRepresentationSet[index];
}

void AbstractRepresentation::setHdfProxy(common::AbstractHdfProxy * proxy)
{
	if (proxy == nullptr) {
		throw invalid_argument("The hdf proxy to associate to the representation cannot be null");
	}

	hdfProxy = proxy;
	proxy->representationSourceObject.push_back(this);
}

void AbstractRepresentation::importRelationshipSetFromEpc(common::EpcDocument* epcDoc)
{
	gsoap_resqml2_0_1::eml20__DataObjectReference* dor = getInterpretationDor();
	if (dor != nullptr) {
		resqml2::AbstractFeatureInterpretation* interp = epcDoc->getResqmlAbstractObjectByUuid<resqml2::AbstractFeatureInterpretation>(dor->UUID);
		if (interp == nullptr) { // partial transfer
			getEpcDocument()->createPartial(dor);
			interp = getEpcDocument()->getResqmlAbstractObjectByUuid<resqml2::AbstractFeatureInterpretation>(dor->UUID);
		}
		if (interp == nullptr) {
			throw invalid_argument("The DOR looks invalid.");
		}
		updateXml = false;
		setInterpretation(interp);
		updateXml = true;
	}

	// Local CRS
	dor = getLocalCrsDor();
	if (dor != nullptr) {
		localCrs = epcDoc->getResqmlAbstractObjectByUuid<AbstractLocal3dCrs>(dor->UUID);
		if (localCrs == nullptr) { // partial transfer
			getEpcDocument()->createPartial(dor);
			localCrs = getEpcDocument()->getResqmlAbstractObjectByUuid<AbstractLocal3dCrs>(dor->UUID);
		}
		if (localCrs == nullptr) {
			throw invalid_argument("The DOR looks invalid.");
		}
		updateXml = false;
		localCrs->addRepresentation(this);
		updateXml = true;
	}

	const std::string uuid = getHdfProxyUuid();
	if (!uuid.empty()) {
		common::AbstractHdfProxy* const hdfProxy = epcDoc->getResqmlAbstractObjectByUuid<common::AbstractHdfProxy>(uuid);
		if (hdfProxy != nullptr) {
			setHdfProxy(hdfProxy);
		}
		else {
			getEpcDocument()->addWarning("The HDF proxy " + uuid + " of the representation " + getUuid() + " is missing");
		}
	}

	// Seismic support
	if (gsoapProxy2_0_1 != nullptr) {
		gsoap_resqml2_0_1::resqml2__AbstractRepresentation* const rep = static_cast<gsoap_resqml2_0_1::resqml2__AbstractRepresentation* const>(gsoapProxy2_0_1);

		// Seismic support
		for (unsigned int patchIndex = 0; patchIndex < getPatchCount(); ++patchIndex) {
			gsoap_resqml2_0_1::resqml2__PointGeometry* geom = getPointGeometry2_0_1(patchIndex);
			if (geom && geom->SeismicCoordinates) {
				if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic3dCoordinates) {
					gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates* const seis3dInfo = static_cast<gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates* const>(geom->SeismicCoordinates);
					pushBackSeismicSupport(epcDoc->getResqmlAbstractObjectByUuid<AbstractRepresentation>(seis3dInfo->SeismicSupport->UUID));
				}
				else if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic2dCoordinates) {
					gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates* const seis2dInfo = static_cast<gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates* const>(geom->SeismicCoordinates);
					pushBackSeismicSupport(epcDoc->getResqmlAbstractObjectByUuid<AbstractRepresentation>(seis2dInfo->SeismicSupport->UUID));
				}
			}
		}
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

vector<Relationship> AbstractRepresentation::getAllEpcRelationships() const
{
	vector<Relationship> result;

	if (interpretation != nullptr)
	{
		Relationship relInterp(interpretation->getPartNameInEpcDocument(), "", interpretation->getUuid());
		relInterp.setDestinationObjectType();
		result.push_back(relInterp);
	}

	if (localCrs != nullptr)
	{
		Relationship relCrs(localCrs->getPartNameInEpcDocument(), "", localCrs->getUuid());
		relCrs.setDestinationObjectType();
		result.push_back(relCrs);
	}

	if (hdfProxy != nullptr)
	{
		Relationship relHdf(hdfProxy->getPartNameInEpcDocument(), "", hdfProxy->getUuid());
		relHdf.setMlToExternalPartProxyType();
		result.push_back(relHdf);
	}

	for (size_t i = 0; i < subRepresentationSet.size(); ++i)
	{
		Relationship relSubRep(subRepresentationSet[i]->getPartNameInEpcDocument(), "", subRepresentationSet[i]->getUuid());
		relSubRep.setSourceObjectType();
		result.push_back(relSubRep);
	}

	for (size_t i = 0; i < propertySet.size(); ++i)
	{
		Relationship relValues(propertySet[i]->getPartNameInEpcDocument(), "", propertySet[i]->getUuid());
		relValues.setSourceObjectType();
		result.push_back(relValues);
	}

	// Seismic support
	for (vector<AbstractRepresentation*>::const_iterator it = seismicSupportSet.begin(); it != seismicSupportSet.end(); ++it)
	{
		if (it == seismicSupportSet.begin() || std::find(seismicSupportSet.begin(), it, *it) != seismicSupportSet.end()) //  No need to add the rel twice
		{
			Relationship relSeisSupport((*it)->getPartNameInEpcDocument(), "", (*it)->getUuid());
			relSeisSupport.setDestinationObjectType();
			result.push_back(relSeisSupport);
		}
	}

	for (size_t i = 0; i < seismicSupportedRepSet.size(); ++i)
	{
		Relationship relSeisSupportedRep(seismicSupportedRepSet[i]->getPartNameInEpcDocument(), "", seismicSupportedRepSet[i]->getUuid());
		relSeisSupportedRep.setSourceObjectType();
		result.push_back(relSeisSupportedRep);
	}

	for (size_t i = 0; i < representationSetRepresentationSet.size(); ++i)
	{
		Relationship relOrg(representationSetRepresentationSet[i]->getPartNameInEpcDocument(), "", representationSetRepresentationSet[i]->getUuid());
		relOrg.setSourceObjectType();
		result.push_back(relOrg);
	}

	return result;
}

void AbstractRepresentation::addSeismic3dCoordinatesToPatch(const unsigned int patchIndex, double * inlines, double * crosslines, const unsigned int & pointCount,
	resqml2::AbstractRepresentation * seismicSupport, common::AbstractHdfProxy * proxy)
{
	if (gsoapProxy2_0_1 != nullptr) {
		setHdfProxy(proxy);

		gsoap_resqml2_0_1::resqml2__PointGeometry* geom = getPointGeometry2_0_1(patchIndex);
		if (geom == nullptr) {
			throw invalid_argument("The patchIndex does not identify a point geometry.");
		}

		if (geom->SeismicCoordinates == nullptr) {
			geom->SeismicCoordinates = gsoap_resqml2_0_1::soap_new_resqml2__Seismic3dCoordinates(gsoapProxy2_0_1->soap, 1);
		}
		else if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic2dCoordinates) {
			throw invalid_argument("It already exists some seismic 2d coordinates for this patch.");
		}
		else {
			throw logic_error("Not implemented yet");
		}
		gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates* patch = static_cast<gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates*>(geom->SeismicCoordinates);

		patch->SeismicSupport = seismicSupport->newResqmlReference();
		pushBackSeismicSupport(seismicSupport);

		// inlines XML
		gsoap_resqml2_0_1::resqml2__DoubleHdf5Array* inlineValues = gsoap_resqml2_0_1::soap_new_resqml2__DoubleHdf5Array(gsoapProxy2_0_1->soap, 1);
		inlineValues->Values = gsoap_resqml2_0_1::soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
		inlineValues->Values->HdfProxy = proxy->newResqmlReference();
		ostringstream oss;
		oss << "inlineCoordinates_patch" << patchIndex;
		inlineValues->Values->PathInHdfFile = "/RESQML/" + getUuid() + "/" + oss.str();
		patch->InlineCoordinates = inlineValues;

		// inlines HDF
		hsize_t dim[] = { pointCount };
		hdfProxy->writeArrayNdOfDoubleValues(getUuid(), oss.str(), inlines, dim, 1);

		// crosslines XML
		gsoap_resqml2_0_1::resqml2__DoubleHdf5Array* crosslineValues = gsoap_resqml2_0_1::soap_new_resqml2__DoubleHdf5Array(gsoapProxy2_0_1->soap, 1);
		crosslineValues->Values = gsoap_resqml2_0_1::soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
		crosslineValues->Values->HdfProxy = proxy->newResqmlReference();
		ostringstream oss2;
		oss2 << "crosslineCoordinates_patch" << patchIndex;
		crosslineValues->Values->PathInHdfFile = "/RESQML/" + getUuid() + "/" + oss2.str();
		patch->CrosslineCoordinates = crosslineValues;

		// crosslines HDF
		hdfProxy->writeArrayNdOfDoubleValues(getUuid(), oss2.str(), crosslines, dim, 1);
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

void AbstractRepresentation::addSeismic3dCoordinatesToPatch(const unsigned int patchIndex, const double & startInline, const double & incrInline, const unsigned int & countInline,
	const double & startCrossline, const double & incrCrossline, const unsigned int & countCrossline,
	resqml2::AbstractRepresentation * seismicSupport)
{
	if (gsoapProxy2_0_1 != nullptr) {
		gsoap_resqml2_0_1::resqml2__PointGeometry* geom = getPointGeometry2_0_1(patchIndex);
		if (!geom)
			throw invalid_argument("The patchIndex does not identify a point geometry.");

		if (geom->SeismicCoordinates == nullptr) {
			geom->SeismicCoordinates = gsoap_resqml2_0_1::soap_new_resqml2__Seismic3dCoordinates(gsoapProxy2_0_1->soap, 1);
		}
		else if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic2dCoordinates) {
			throw invalid_argument("It already exists some seismic 2d coordinates for this patch.");
		}
		else {
			throw logic_error("Not implemented yet");
		}
		gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates* patch = static_cast<gsoap_resqml2_0_1::resqml2__Seismic3dCoordinates*>(geom->SeismicCoordinates);

		patch->SeismicSupport = seismicSupport->newResqmlReference();
		pushBackSeismicSupport(seismicSupport);

		// inlines XML
		gsoap_resqml2_0_1::resqml2__DoubleLatticeArray* inlineValues = gsoap_resqml2_0_1::soap_new_resqml2__DoubleLatticeArray(gsoapProxy2_0_1->soap, 1);
		patch->InlineCoordinates = inlineValues;
		inlineValues->StartValue = startInline;
		gsoap_resqml2_0_1::resqml2__DoubleConstantArray * spacInline = gsoap_resqml2_0_1::soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
		spacInline->Count = countInline - 1;
		spacInline->Value = incrInline;
		inlineValues->Offset.push_back(spacInline);

		// crosslines XML
		gsoap_resqml2_0_1::resqml2__DoubleLatticeArray* crosslineValues = gsoap_resqml2_0_1::soap_new_resqml2__DoubleLatticeArray(gsoapProxy2_0_1->soap, 1);
		patch->CrosslineCoordinates = crosslineValues;
		crosslineValues->StartValue = startCrossline;
		gsoap_resqml2_0_1::resqml2__DoubleConstantArray * spacCrossline = gsoap_resqml2_0_1::soap_new_resqml2__DoubleConstantArray(gsoapProxy2_0_1->soap, 1);
		spacCrossline->Count = countCrossline - 1;
		spacCrossline->Value = incrCrossline;
		crosslineValues->Offset.push_back(spacCrossline);
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

void AbstractRepresentation::addSeismic2dCoordinatesToPatch(const unsigned int patchIndex, double * lineAbscissa, const unsigned int & pointCount,
	resqml2::AbstractRepresentation * seismicSupport, common::AbstractHdfProxy * proxy)
{
	if (gsoapProxy2_0_1 != nullptr) {
		setHdfProxy(proxy);

		gsoap_resqml2_0_1::resqml2__PointGeometry* geom = getPointGeometry2_0_1(patchIndex);
		if (!geom)
			throw invalid_argument("The patchIndex does not identify a point geometry.");

		if (geom->SeismicCoordinates == nullptr) {
			geom->SeismicCoordinates = gsoap_resqml2_0_1::soap_new_resqml2__Seismic2dCoordinates(gsoapProxy2_0_1->soap, 1);
		}
		else if (geom->SeismicCoordinates->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__Seismic3dCoordinates) {
			throw invalid_argument("It already exists some seismic 3d coordinates for this patch.");
		}
		else {
			throw logic_error("Not implemented yet");
		}
		gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates* patch = static_cast<gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates*>(geom->SeismicCoordinates);

		patch->SeismicSupport = seismicSupport->newResqmlReference();
		pushBackSeismicSupport(seismicSupport);

		// abscissa XML
		gsoap_resqml2_0_1::resqml2__DoubleHdf5Array* abscissaValues = gsoap_resqml2_0_1::soap_new_resqml2__DoubleHdf5Array(gsoapProxy2_0_1->soap, 1);
		abscissaValues->Values = gsoap_resqml2_0_1::soap_new_eml20__Hdf5Dataset(gsoapProxy2_0_1->soap, 1);
		abscissaValues->Values->HdfProxy = proxy->newResqmlReference();
		ostringstream oss;
		oss << "lineAbscissa_patch" << patchIndex;
		abscissaValues->Values->PathInHdfFile = "/RESQML/" + getUuid() + "/" + oss.str();
		patch->LineAbscissa = abscissaValues;

		// inlines HDF
		hsize_t dim[] = { pointCount };
		hdfProxy->writeArrayNdOfDoubleValues(getUuid(), oss.str(), lineAbscissa, dim, 1);
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

void AbstractRepresentation::getSeismicLineAbscissaOfPointsOfPatch(const unsigned int & patchIndex, double * values)
{
	if (gsoapProxy2_0_1 != nullptr) {
		gsoap_resqml2_0_1::resqml2__Seismic2dCoordinates* seisInfo = getSeismic2dCoordinates(patchIndex);
		if (seisInfo == nullptr) {
			return;
		}

		if (seisInfo->LineAbscissa->soap_type() == SOAP_TYPE_gsoap_resqml2_0_1_resqml2__DoubleHdf5Array) {
			gsoap_resqml2_0_1::resqml2__DoubleHdf5Array* dblValues = static_cast<gsoap_resqml2_0_1::resqml2__DoubleHdf5Array*>(seisInfo->LineAbscissa);
			hdfProxy->readArrayNdOfDoubleValues(dblValues->Values->PathInHdfFile, values);
		}
	}
	else {
		throw logic_error("Not implemented yet");
	}
}

