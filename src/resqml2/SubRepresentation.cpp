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
#include "resqml2/SubRepresentation.h"

#include <sstream>
#include <stdexcept>

#include "hdf5.h"

#include "resqml2/AbstractFeatureInterpretation.h"
#include "common/AbstractHdfProxy.h"
#include "resqml2_0_1/UnstructuredGridRepresentation.h"
#include "resqml2_0_1/AbstractIjkGridRepresentation.h"

using namespace std;
using namespace epc;
using namespace resqml2;

const char* SubRepresentation::XML_TAG = "SubRepresentation";

vector<Relationship> SubRepresentation::getAllEpcRelationships() const
{
	vector<Relationship> result = AbstractRepresentation::getAllEpcRelationships();

	const unsigned int supRepCount = getSupportingRepresentationCount();
	for (unsigned int supRepIndex = 0; supRepIndex < supRepCount; ++supRepIndex) {
		AbstractRepresentation* supportingRep = getSupportingRepresentation(supRepIndex);
		if (!supportingRep->isPartial())
		{
			Relationship rel(supportingRep->getPartNameInEpcDocument(), "", supportingRep->getUuid());
			rel.setDestinationObjectType();
			result.push_back(rel);
		}
	}
    
	return result;
}

void SubRepresentation::importRelationshipSetFromEpc(common::EpcDocument* epcDoc)
{
	AbstractRepresentation::importRelationshipSetFromEpc(epcDoc);

	// Supporting representation
	const unsigned int supRepCount = getSupportingRepresentationCount();
	for (unsigned int supRepIndex = 0; supRepIndex < supRepCount; ++supRepIndex) {
		gsoap_resqml2_0_1::eml20__DataObjectReference* dor = getSupportingRepresentationDor(supRepIndex);
		resqml2::AbstractRepresentation* supportingRep = epcDoc->getResqmlAbstractObjectByUuid<resqml2::AbstractRepresentation>(dor->UUID);
		if (supportingRep == nullptr) { // partial transfer
			getEpcDocument()->createPartial(dor);
			supportingRep = getEpcDocument()->getResqmlAbstractObjectByUuid<resqml2::AbstractRepresentation>(dor->UUID);
		}
		if (supportingRep == nullptr) {
			throw invalid_argument("The DOR looks invalid.");
		}
		updateXml = false;
		supportingRep->pushBackSubRepresentation(this);
		updateXml = true;
	}
}

ULONG64 SubRepresentation::getXyzPointCountOfPatch(const unsigned int & patchIndex) const
{
	if (patchIndex >= getPatchCount()) {
		throw range_error("The index of the patch is not in the allowed range of patch.");
	}

	if (getElementKindOfPatch(patchIndex, 0) == NODE) {
		return getElementCountOfPatch(patchIndex);
	}
	else {
		throw logic_error("Not implemented yet.");
	}
}

void SubRepresentation::getXyzPointsOfPatch(const unsigned int & patchIndex, double * xyzPoints) const
{
	if (patchIndex >= getPatchCount()) {
		throw range_error("The index of the patch is not in the allowed range of patch.");
	}

	throw logic_error("Not implemented yet");
}

void SubRepresentation::pushBackSupportingRepresentation(AbstractRepresentation * supportingRep)
{
	if (supportingRep == nullptr) {
		throw invalid_argument("The supporting Representation cannot be null.");
	}

	// EPC
	supportingRep->pushBackSubRepresentation(this);

	// XML
	if (updateXml) {
		pushBackXmlSupportingRepresentation(supportingRep);
	}
}

AbstractRepresentation* SubRepresentation::getSupportingRepresentation(unsigned int index) const
{
	return static_cast<AbstractRepresentation*>(epcDocument->getResqmlAbstractObjectByUuid(getSupportingRepresentationUuid(index)));
}

std::string SubRepresentation::getSupportingRepresentationUuid(unsigned int index) const
{
	return getSupportingRepresentationDor(index)->UUID;
}

std::string SubRepresentation::getSupportingRepresentationTitle(unsigned int index) const
{
	return getSupportingRepresentationDor(index)->Title;
}

std::string SubRepresentation::getSupportingRepresentationContentType() const
{
	return getSupportingRepresentationDor(0)->ContentType;
}
