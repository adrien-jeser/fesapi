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
#include "witsml1_4_1_1/Wellbore.h"

#include <stdexcept>

#include "tools/GuidTools.h"
#include "witsml1_4_1_1/Well.h"

using namespace std;
using namespace witsml1_4_1_1;
using namespace gsoap_witsml1_4_1_1;
using namespace epc;

const char* Wellbore::XML_TAG = "wellbores";

Wellbore::Wellbore(
			Well* witsmlWell,
			const std::string & guid,
			const std::string & title):resqmlWellboreFeature(nullptr)
{
	if (witsmlWell == nullptr) throw invalid_argument("A wellbore must be associated to a well.");
	if (title.empty()) throw invalid_argument("A wellbore must have a name.");

	collection = soap_new_witsml1__obj_USCOREwellbores(witsmlWell->getGSoapContext(), 1);	
	static_cast<_witsml1__wellbores*>(collection)->version = SCHEMA_VERSION;

	witsml1__obj_USCOREwellbore* wellbore = soap_new_witsml1__obj_USCOREwellbore(collection->soap, 1);
	static_cast<_witsml1__wellbores*>(collection)->wellbore.push_back(wellbore);

	setWell(witsmlWell);

	wellbore->uid = soap_new_std__string(collection->soap, 1);
	if (guid.empty()) {
		wellbore->uid->assign(tools::GuidTools::generateUidAsString());
	} else {
		wellbore->uid->assign(guid);
	}

	wellbore->name = title;
}

Wellbore::Wellbore(
		Well* witsmlWell,
		const std::string & guid,
		const std::string & title,
		gsoap_witsml1_4_1_1::witsml1__WellStatus statusWellbore,
		const bool & isActive,
		gsoap_witsml1_4_1_1::witsml1__WellPurpose purposeWellbore,
		gsoap_witsml1_4_1_1::witsml1__WellboreType typeWellbore,
		const bool & achievedTD,
		const std::string & sourceName,
		const time_t & dTimCreation,
		const time_t & dTimLastChange,
		const std::string & comments
	):resqmlWellboreFeature(nullptr)
{
	if (witsmlWell == nullptr) throw invalid_argument("A wellbore must be associated to a well.");
	if (title.empty()) throw invalid_argument("A wellbore must have a name.");

	if (witsmlWell == nullptr) throw invalid_argument("A wellbore must be associated to a well.");
	if (title.empty()) throw invalid_argument("A wellbore must have a name.");

	collection = soap_new_witsml1__obj_USCOREwellbores(witsmlWell->getGSoapContext(), 1);	
	static_cast<_witsml1__wellbores*>(collection)->version = SCHEMA_VERSION;

	witsml1__obj_USCOREwellbore* wellbore = soap_new_witsml1__obj_USCOREwellbore(collection->soap, 1);
	static_cast<_witsml1__wellbores*>(collection)->wellbore.push_back(wellbore);

	setWell(witsmlWell);

	wellbore->uid = soap_new_std__string(collection->soap, 1);
	if (guid.empty()) {
		wellbore->uid->assign(tools::GuidTools::generateUidAsString());
	} else {
		wellbore->uid->assign(guid);
	}

	wellbore->name = title;

	__witsml1__obj_USCOREwellbore_sequence* wellboreSequence = soap_new___witsml1__obj_USCOREwellbore_sequence(collection->soap, 1);

	wellboreSequence->statusWellbore = (witsml1__WellStatus *) soap_malloc(collection->soap, sizeof(witsml1__WellStatus));
	*wellboreSequence->statusWellbore = statusWellbore;

	wellboreSequence->isActive = (bool *) soap_malloc(collection->soap, sizeof(bool));
	*wellboreSequence->isActive = isActive;

	wellboreSequence->purposeWellbore = (witsml1__WellPurpose *) soap_malloc(collection->soap, sizeof(witsml1__WellPurpose));
	*wellboreSequence->purposeWellbore = purposeWellbore;

	wellboreSequence->typeWellbore = (witsml1__WellboreType *) soap_malloc(collection->soap, sizeof(witsml1__WellboreType));
	*wellboreSequence->typeWellbore = typeWellbore;

	wellboreSequence->achievedTD = (bool *) soap_malloc(collection->soap, sizeof(bool));
	*wellboreSequence->achievedTD = achievedTD;

	wellbore->__obj_USCOREwellbore_sequence = wellboreSequence;

	// create the commonData attribute, if necessary
	if (!sourceName.empty() || dTimCreation>=0 || dTimLastChange>=0 || !comments.empty())
	{
		wellbore->commonData = newCommonData(sourceName,
			dTimCreation,
			dTimLastChange,
			comments);
	}
}

Trajectory* Wellbore::createTrajectory(
	const std::string & guid,
	const std::string & title)
{
	Trajectory* result = new Trajectory(this, guid, title);
	if (getEpcDocument() != nullptr)
	{
		try {
			getEpcDocument()->addGsoapProxy(result);
		}
		catch (const exception & e)
		{
			std::cerr << e.what() << endl;
			std::cerr << "The proxy is going to be deleted but deletion is not yet guaranteed. You should close your application." << endl;
			delete result;
			throw;
		}
	}
	return result;
}

Trajectory* Wellbore::createTrajectory(
	const std::string & guid,
	const std::string & title,
	const std::string & sourceName,
	const time_t & dTimCreation,
	const time_t & dTimLastChange,
	const std::string & comments)
{
	Trajectory* result = new Trajectory(this, guid, title, sourceName, dTimCreation, dTimLastChange, comments);
	if (getEpcDocument() != nullptr)
	{
		try {
			getEpcDocument()->addGsoapProxy(result);
		}
		catch (const exception & e)
		{
			std::cerr << e.what() << endl;
			std::cerr << "The proxy is going to be deleted but deletion is not yet guaranteed. You should close your application." << endl;
			delete result;
			throw;
		}
	}
	return result;
}

Log* Wellbore::createLog(
	const std::string & guid,
	const std::string & title,
	gsoap_witsml1_4_1_1::witsml1__LogIndexType indexType,
	const std::string & indexCurve)
{
	Log* result = new Log(this, guid, title, indexType, indexCurve);
	if (getEpcDocument() != nullptr)
	{
		try {
			getEpcDocument()->addGsoapProxy(result);
		}
		catch (const exception & e)
		{
			std::cerr << e.what() << endl;
			std::cerr << "The proxy is going to be deleted but deletion is not yet guaranteed. You should close your application." << endl;
			delete result;
			throw;
		}
	}
	return result;
}

Log* Wellbore::createLog(
	const std::string & guid,
	const std::string & title,
	gsoap_witsml1_4_1_1::witsml1__LogIndexType indexType,
	const std::string & indexCurve,
	const std::string & sourceName,
	const time_t & dTimCreation,
	const time_t & dTimLastChange,
	const std::string & comments)
{
	Log* result = new Log(this, guid, title, indexType, indexCurve, sourceName, dTimCreation, dTimLastChange, comments);
	if (getEpcDocument() != nullptr)
	{
		try {
			getEpcDocument()->addGsoapProxy(result);
		}
		catch (const exception & e)
		{
			std::cerr << e.what() << endl;
			std::cerr << "The proxy is going to be deleted but deletion is not yet guaranteed. You should close your application." << endl;
			delete result;
			throw;
		}
	}
	return result;
}
		
FormationMarker* Wellbore::createFormationMarker(
	const std::string & guid,
	const std::string & title,
	const unsigned int & mdDatumIndex,
	gsoap_witsml1_4_1_1::witsml1__MeasuredDepthUom mdUom,
	const double & mdTopSample)
{
	FormationMarker* result = new FormationMarker(this, guid, title, mdDatumIndex, mdUom, mdTopSample);
	if (getEpcDocument() != nullptr)
	{
		try {
			getEpcDocument()->addGsoapProxy(result);
		}
		catch (const exception & e)
		{
			std::cerr << e.what() << endl;
			std::cerr << "The proxy is going to be deleted but deletion is not yet guaranteed. You should close your application." << endl;
			delete result;
			throw;
		}
	}
	return result;
}

FormationMarker* Wellbore::createFormationMarker(
	const std::string & guid,
	const std::string & title,
	const unsigned int & mdDatumIndex,
	gsoap_witsml1_4_1_1::witsml1__MeasuredDepthUom mdUom,
	const double & mdTopSample,
	const unsigned int & tvdDatumIndex,
	gsoap_witsml1_4_1_1::witsml1__WellVerticalCoordinateUom tvdUom,
	const double & tvdTopSample,
	gsoap_witsml1_4_1_1::witsml1__PlaneAngleUom dipUom,
	const double & dip,
	gsoap_witsml1_4_1_1::witsml1__PlaneAngleUom dipDirectionUom,
	const double & dipDirection,
	const std::string & sourceName,
	const time_t & dTimCreation,
	const time_t & dTimLastChange,
	const std::string & comments)
{
	FormationMarker* result = new FormationMarker(this, guid, title, mdDatumIndex, mdUom, mdTopSample, tvdDatumIndex, tvdUom, tvdTopSample, dipUom, dip, dipDirectionUom, dipDirection, sourceName, dTimCreation, dTimLastChange, comments);
	if (getEpcDocument() != nullptr)
	{
		try {
			getEpcDocument()->addGsoapProxy(result);
		}
		catch (const exception & e)
		{
			std::cerr << e.what() << endl;
			std::cerr << "The proxy is going to be deleted but deletion is not yet guaranteed. You should close your application." << endl;
			delete result;
			throw;
		}
	}
	return result;
}

void Wellbore::setWell(Well* witsmlWell)
{
	well = witsmlWell;
	witsmlWell->wellboreSet.push_back(this);

	if (updateXml)
	{
		witsml1__obj_USCOREwellbore* wellbore = static_cast<_witsml1__wellbores*>(collection)->wellbore[0];
		wellbore->nameWell = witsmlWell->getTitle();
		wellbore->uidWell = soap_new_std__string(collection->soap, 1);
		wellbore->uidWell->assign(witsmlWell->getUuid());
	}
}

void Wellbore::setShape(const witsml1__WellboreShape & shape)
{
	if (static_cast<_witsml1__wellbores*>(collection)->wellbore[0]->__obj_USCOREwellbore_sequence == nullptr)
		static_cast<_witsml1__wellbores*>(collection)->wellbore[0]->__obj_USCOREwellbore_sequence = soap_new___witsml1__obj_USCOREwellbore_sequence(collection->soap, 1);

	static_cast<_witsml1__wellbores*>(collection)->wellbore[0]->__obj_USCOREwellbore_sequence->shape = (witsml1__WellboreShape *)soap_malloc(collection->soap, sizeof( witsml1__WellboreShape));
	*static_cast<_witsml1__wellbores*>(collection)->wellbore[0]->__obj_USCOREwellbore_sequence->shape = shape;
}

void Wellbore::importRelationshipSetFromEpc(common::EpcDocument* epcDoc)
{
	witsml1__obj_USCOREwellbore* wellbore = static_cast<_witsml1__wellbores*>(collection)->wellbore[0];
	if (wellbore && wellbore->uidWell)
	{
		updateXml = false;
		setWell(static_cast<Well*>(epcDoc->getWitsmlAbstractObjectByUuid(*wellbore->uidWell)));
		updateXml = true;
	}
}

vector<Relationship> Wellbore::getAllEpcRelationships() const
{
	vector<Relationship> result = AbstractObject::getAllEpcRelationships();

	// XML forward relationship
	if (well) {
		Relationship relWell(well->getPartNameInEpcDocument(), "", well->getUuid());
		relWell.setDestinationObjectType();
		result.push_back(relWell);
	}

	// XML backward relationship
	if (resqmlWellboreFeature)
	{
		Relationship rel(resqmlWellboreFeature->getPartNameInEpcDocument(), "", resqmlWellboreFeature->getUuid());
		rel.setSourceObjectType();
		result.push_back(rel);
	}

	return result;
}
