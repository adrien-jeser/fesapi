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
#include "witsml1_4_1_1/Log.h"

#include <stdexcept>

#include "tools/GuidTools.h"
#include "witsml1_4_1_1/Well.h"

using namespace std;
using namespace witsml1_4_1_1;
using namespace gsoap_witsml1_4_1_1;
using namespace epc;

const char* Log::XML_TAG = "logs";

Log::Log(
			Wellbore* witsmlWellbore,
			const std::string & guid,
			const std::string & title,
			witsml1__LogIndexType indexType,
			const std::string & indexCurve):resqmlWellboreFrameRepresentation(nullptr)
{
	if (witsmlWellbore == nullptr) throw invalid_argument("A log must be associated to a wellbore.");
	if (title.empty()) throw invalid_argument("A log must have a name.");

	collection = soap_new_witsml1__obj_USCORElogs(witsmlWellbore->getGSoapContext(), 1);	
	static_cast<_witsml1__logs*>(collection)->version = SCHEMA_VERSION;

	witsml1__obj_USCORElog* log = soap_new_witsml1__obj_USCORElog(collection->soap, 1);
	static_cast<_witsml1__logs*>(collection)->log.push_back(log);

	setWellbore(witsmlWellbore);

	log->uid = soap_new_std__string(collection->soap, 1);
	if (guid.empty()) {
		log->uid->assign(tools::GuidTools::generateUidAsString());
	} else {
		log->uid->assign(guid);
	}

	log->name = title;

	__witsml1__obj_USCORElog_sequence* witsmlSequence = soap_new___witsml1__obj_USCORElog_sequence(collection->soap, 1);
	witsmlSequence->indexType = indexType;
	witsmlSequence->indexCurve = indexCurve;
	log->__obj_USCORElog_sequence = witsmlSequence;
}

Log::Log(
		Wellbore* witsmlWellbore,
		const std::string & guid,
		const std::string & title,
		witsml1__LogIndexType indexType,
		const std::string & indexCurve,
		const std::string & sourceName,
		const time_t & dTimCreation,
		const time_t & dTimLastChange,
		const std::string & comments
	):resqmlWellboreFrameRepresentation(nullptr)
{
	if (witsmlWellbore == nullptr) throw invalid_argument("A log must be associated to a wellbore.");
	if (title.empty()) throw invalid_argument("A log must have a name.");

	collection = soap_new_witsml1__obj_USCORElogs(witsmlWellbore->getGSoapContext(), 1);	
	static_cast<_witsml1__logs*>(collection)->version = SCHEMA_VERSION;

	witsml1__obj_USCORElog* log = soap_new_witsml1__obj_USCORElog(collection->soap, 1);
	static_cast<_witsml1__logs*>(collection)->log.push_back(log);

	setWellbore(witsmlWellbore);

	log->uid = soap_new_std__string(collection->soap, 1);
	if (guid.empty()) {
		log->uid->assign(tools::GuidTools::generateUidAsString());
	} else {
		log->uid->assign(guid);
	}

	log->name = title;

	__witsml1__obj_USCORElog_sequence* witsmlSequence = soap_new___witsml1__obj_USCORElog_sequence(collection->soap, 1);
	witsmlSequence->indexType = indexType;
	witsmlSequence->indexCurve = indexCurve;
	log->__obj_USCORElog_sequence = witsmlSequence;

	// create the commonData attribute, if necessary
	if (!sourceName.empty() || dTimCreation>=0 || dTimLastChange>=0 || !comments.empty())
	{
		log->commonData = newCommonData(sourceName,
			dTimCreation,
			dTimLastChange,
			comments);
	}
	if (dTimCreation >= 0)
	{
		log->__obj_USCORElog_sequence->creationDate = (time_t *) soap_malloc(collection->soap, sizeof(time_t));
		*log->__obj_USCORElog_sequence->creationDate = dTimCreation;
	}
}

void Log::setWellbore(Wellbore* witsmlWellbore)
{
	wellbore = witsmlWellbore;
	witsmlWellbore->logSet.push_back(this);

	if (updateXml)
	{
		witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];
		log->nameWellbore = witsmlWellbore->getTitle();
		log->uidWellbore = soap_new_std__string(collection->soap, 1);
		log->uidWellbore->assign(witsmlWellbore->getUuid());

		log->nameWell = witsmlWellbore->getWell()->getTitle();
		log->uidWell = soap_new_std__string(collection->soap, 1);
		log->uidWell->assign(witsmlWellbore->getWell()->getUuid());
	}
}

gsoap_witsml1_4_1_1::witsml1__LogIndexType Log::getIndexType()
{
	if (static_cast<_witsml1__logs*>(collection)->log.empty())
		throw invalid_argument("The log collection is empty.");
	if (static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence == nullptr)
		throw invalid_argument("The log is empty.");

	return static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->indexType;
}

std::string Log::getIndexMnemonic()
{
	if (static_cast<_witsml1__logs*>(collection)->log.empty())
		throw invalid_argument("The log collection is empty.");
	if (static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence == nullptr)
		throw invalid_argument("The log is empty.");

	return static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->indexCurve;
}

std::string Log::getDataDelimiter() const
{
	if (static_cast<_witsml1__logs*>(collection)->log.empty())
		return ","; //Default
	if (static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence == nullptr)
		return ","; //Default
	if (static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->dataDelimiter != nullptr)
		return *(static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->dataDelimiter);
	else
		return ",";
}

const std::vector<std::string> & Log::getData() const
{
	if (static_cast<_witsml1__logs*>(collection)->log.empty())
		throw invalid_argument("The log collection is empty.");
	if (static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence == nullptr)
		throw invalid_argument("The log is empty.");
	if (static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->logData.empty())
		throw invalid_argument("The log is empty.");
	return static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->logData[0]->data;
}

std::vector<std::string> Log::getMnemonicSet()
{
	if (static_cast<_witsml1__logs*>(collection)->log.empty())
		throw invalid_argument("The log collection is empty.");

	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	std::vector<std::string> result;
	for (unsigned int i = 0; i < log->__obj_USCORElog_sequence->logCurveInfo.size(); ++i)
	{
		result.push_back(log->__obj_USCORElog_sequence->logCurveInfo[i]->mnemonic->__item);
	}
	return result;
}

void Log::setValues(
		const std::string & description,
		const std::string & dataDelimiter,
		const std::string & startEndIncrementUOM,
		const double & startIndex,
		const double & endIndex,
		const double & stepIncrement,
		gsoap_witsml1_4_1_1::witsml1__LogIndexDirection direction,
		//const std::string & nullValue,
		const std::string & mnemonicList,
		const std::string & unitList,
		std::vector<std::string> const & data)
{
	if (dataDelimiter.empty()) throw invalid_argument("Values must be associated to a delimiter.");
	if (startEndIncrementUOM.empty()) throw invalid_argument("Value indices must be associated to a uom.");

	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	if (!description.empty())
	{
		log->__obj_USCORElog_sequence->description = soap_new_std__string(collection->soap, 1);
		log->__obj_USCORElog_sequence->description->assign(description);
	}

	// if the data delimiter is the empty string, we use a default data delimiter
	log->__obj_USCORElog_sequence->dataDelimiter = soap_new_std__string(collection->soap, 1);
	log->__obj_USCORElog_sequence->dataDelimiter->assign(dataDelimiter);

	// we fill the startIndex, endIndex and stepIncrement attributes
	log->__obj_USCORElog_sequence->startIndex = soap_new_witsml1__genericMeasure(collection->soap, 1);
	log->__obj_USCORElog_sequence->endIndex = soap_new_witsml1__genericMeasure(collection->soap, 1);
	log->__obj_USCORElog_sequence->stepIncrement = soap_new_witsml1__ratioGenericMeasure(collection->soap, 1);
	log->__obj_USCORElog_sequence->startIndex->uom = startEndIncrementUOM;
	log->__obj_USCORElog_sequence->endIndex->uom = startEndIncrementUOM;
	log->__obj_USCORElog_sequence->stepIncrement->uom = startEndIncrementUOM;

	log->__obj_USCORElog_sequence->startIndex->__item = startIndex;
	log->__obj_USCORElog_sequence->endIndex->__item = endIndex;
	log->__obj_USCORElog_sequence->stepIncrement->__item = stepIncrement;

	log->__obj_USCORElog_sequence->direction = (witsml1__LogIndexDirection *) soap_malloc(collection->soap, sizeof(witsml1__LogIndexDirection));
	*log->__obj_USCORElog_sequence->direction = direction;

	// as the default nullValue is the empty string, we do not test wether the nullValue parameter is empty
	//witsmlLog->nullValue = soap_new_std__string(soap, 1);
	//witsmlLog->nullValue->assign(DEFAULT_nullptrVALUE);

	// setting up the data
	if (!mnemonicList.empty() && !unitList.empty() && !data.empty())
	{
		witsml1__cs_USCORElogData* logData = soap_new_witsml1__cs_USCORElogData(collection->soap, 1);
		logData->mnemonicList = mnemonicList;
		logData->unitList = unitList;
		logData->data = data;
		log->__obj_USCORElog_sequence->logData.push_back(logData);
	}
}

unsigned int Log::getLogCurveCount() const
{
	return static_cast<_witsml1__logs*>(collection)->log[0]->__obj_USCORElog_sequence->logCurveInfo.size();
}

std::string Log::getLogCurveMnemonic(const unsigned int & logCurveIndex) const
{
	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	if (log->__obj_USCORElog_sequence->logCurveInfo.size() > logCurveIndex)
	{
		return log->__obj_USCORElog_sequence->logCurveInfo[logCurveIndex]->mnemonic->__item;
	}
	else
		throw out_of_range("This log curve index is out of range.");
}

gsoap_witsml1_4_1_1::witsml1__LogDataType Log::getLogCurveDataType(const unsigned int & logCurveIndex) const
{
	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	if (log->__obj_USCORElog_sequence->logCurveInfo.size() > logCurveIndex)
	{
		return log->__obj_USCORElog_sequence->logCurveInfo[logCurveIndex]->typeLogData;
	}
	else
		throw out_of_range("This log curve index is out of range.");
}

std::string Log::getLogCurveUom(const unsigned int & logCurveIndex) const
{
	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	if (log->__obj_USCORElog_sequence->logCurveInfo.size() > logCurveIndex)
	{
		if (log->__obj_USCORElog_sequence->logCurveInfo[logCurveIndex]->unit)
			return *log->__obj_USCORElog_sequence->logCurveInfo[logCurveIndex]->unit;
		else
			return "";
	}
	else
		throw out_of_range("This log curve index is out of range.");
}

void Log::pushBackLogCurveInfo(const string & mnemonic, witsml1__LogDataType typeLogData)
{
	if (mnemonic.empty())  throw invalid_argument("A mnemonic must be provided");

	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	// creation of the soap log curve info object and attachement to its parent soap log object
	witsml1__cs_USCORElogCurveInfo* logCurveInfo = soap_new_witsml1__cs_USCORElogCurveInfo(collection->soap, 1);
	log->__obj_USCORElog_sequence->logCurveInfo.push_back(logCurveInfo);

	logCurveInfo->mnemonic = soap_new_witsml1__shortNameStruct(collection->soap, 1);
	logCurveInfo->mnemonic->__item = mnemonic;

	logCurveInfo->typeLogData = typeLogData;
}

void Log::pushBackLogCurveInfo(
	const string & namingSystem,
	const string & mnemonic,
	const string & classWitsml,
	const string & unit,
	//const string & nullValue,
	const int & datumIndex,
	const string & minMaxIndexUOM,
	const double & minIndex,
	const double & maxIndex,
	const string & curveDescription,
	witsml1__PerLengthUom perLengthUOM,
	const double & densData,
	witsml1__LogTraceOrigin traceOrigin,
	witsml1__LogDataType typeLogData)
{
	if (namingSystem.empty()) throw invalid_argument("A naming system must be provided.");
	if (mnemonic.empty())  throw invalid_argument("A mnemonic must be provided.");
	if (minMaxIndexUOM.empty())  throw invalid_argument("A uom for the min and max index must be provided.");

	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];

	witsml1__cs_USCOREwellDatum* datum = nullptr;
	if (datumIndex >= 0)
		wellbore->getWell()->getDatum(datumIndex);

	// creation of the soap log curve info object and attachement to its parent soap log object
	witsml1__cs_USCORElogCurveInfo* logCurveInfo = soap_new_witsml1__cs_USCORElogCurveInfo(collection->soap, 1);
	log->__obj_USCORElog_sequence->logCurveInfo.push_back(logCurveInfo);

	logCurveInfo->mnemonic = soap_new_witsml1__shortNameStruct(collection->soap, 1);
	logCurveInfo->mnemonic->namingSystem = soap_new_std__string(collection->soap, 1);
	logCurveInfo->mnemonic->namingSystem->assign(namingSystem);
	logCurveInfo->mnemonic->__item = mnemonic;
	logCurveInfo->typeLogData = typeLogData;

	if (!classWitsml.empty())
	{
		logCurveInfo->classWitsml = soap_new_std__string(collection->soap, 1);
		logCurveInfo->classWitsml->assign(classWitsml);
	}

	if (!unit.empty())
	{
		logCurveInfo->unit = soap_new_std__string(collection->soap, 1);
		logCurveInfo->unit->assign(unit);
	}

	// as the default nullValue is the empty string, we do not test wether the nullValue parameter is empty
	//logCurveInfo->nullValue = soap_new_std__string(soap, 1);
	//logCurveInfo->nullValue->assign(DEFAULT_nullptrVALUE);

	if (datum != nullptr)
	{
		logCurveInfo->wellDatum = soap_new_witsml1__refNameString(collection->soap, 1);
		logCurveInfo->wellDatum->__item = datum->name;
		logCurveInfo->wellDatum->uidRef = soap_new_std__string(collection->soap, 1);
		logCurveInfo->wellDatum->uidRef->assign(datum->uid);
	}

	// setting the min and max index
	logCurveInfo->minIndex = soap_new_witsml1__genericMeasure(collection->soap, 1);
	logCurveInfo->maxIndex = soap_new_witsml1__genericMeasure(collection->soap, 1);
	logCurveInfo->minIndex->uom = minMaxIndexUOM;
	logCurveInfo->maxIndex->uom = minMaxIndexUOM;
	logCurveInfo->minIndex->__item = minIndex;
	logCurveInfo->maxIndex->__item = maxIndex;

	// setting the curve decription
	if (!curveDescription.empty())
	{
		logCurveInfo->curveDescription = soap_new_std__string(collection->soap, 1);
		logCurveInfo->curveDescription->assign(curveDescription);
	}

	// setting the data density
	logCurveInfo->densData = soap_new_witsml1__perLengthMeasure(collection->soap, 1);
	logCurveInfo->densData->uom = perLengthUOM;
	logCurveInfo->densData->__item = densData;

	// setting the trace state
	//logCurveInfo->traceState = (witsml1__LogTraceState *) soap_malloc(soap, sizeof(witsml1__LogTraceState));
	//*logCurveInfo->traceState = witsml1__LogTraceState__raw; 

	// setting the trace origin
	logCurveInfo->traceOrigin = (witsml1__LogTraceOrigin *) soap_malloc(collection->soap, sizeof(witsml1__LogTraceOrigin));
	*logCurveInfo->traceOrigin = traceOrigin; 
}

void Log::importRelationshipSetFromEpc(common::EpcDocument* epcDoc)
{
	witsml1__obj_USCORElog* log = static_cast<_witsml1__logs*>(collection)->log[0];
	if (log && log->uidWellbore)
	{
		updateXml = false;
		setWellbore(static_cast<Wellbore*>(epcDoc->getWitsmlAbstractObjectByUuid(*log->uidWellbore)));
		updateXml = true;
	}
}

vector<Relationship> Log::getAllEpcRelationships() const
{
	vector<Relationship> result = AbstractObject::getAllEpcRelationships();

	// XML forward relationship
	Relationship relWellbore(wellbore->getPartNameInEpcDocument(), "", wellbore->getUuid());
	relWellbore.setDestinationObjectType();
	result.push_back(relWellbore);
		
	Relationship relWell(wellbore->getWell()->getPartNameInEpcDocument(), "", wellbore->getWell()->getUuid());
	relWell.setDestinationObjectType();
	result.push_back(relWell);

	// XML backward relationship
	if (resqmlWellboreFrameRepresentation)
	{
		Relationship rel(resqmlWellboreFrameRepresentation->getPartNameInEpcDocument(), "", resqmlWellboreFrameRepresentation->getUuid());
		rel.setSourceObjectType();
		result.push_back(rel);
	}

	return result;
}
