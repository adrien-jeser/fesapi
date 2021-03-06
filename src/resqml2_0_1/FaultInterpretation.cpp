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
#include "resqml2_0_1/FaultInterpretation.h"

#include <limits>
#include <stdexcept>

#include "resqml2_0_1/GeneticBoundaryFeature.h"
#include "resqml2_0_1/TectonicBoundaryFeature.h"
#include "resqml2_0_1/StructuralOrganizationInterpretation.h"

using namespace std;
using namespace resqml2_0_1;
using namespace gsoap_resqml2_0_1;
using namespace epc;

const char* FaultInterpretation::XML_TAG = "FaultInterpretation";

FaultInterpretation::FaultInterpretation(TectonicBoundaryFeature * fault, const string & guid, const string & title)
{
	if (fault == nullptr) {
		throw invalid_argument("The interpreted fault cannot be null.");
	}
	if (fault->isAFracture()) {
		throw invalid_argument("The interpreted fault cannot be a fracture.");
	}

	gsoapProxy2_0_1 = soap_new_resqml2__obj_USCOREFaultInterpretation(fault->getGsoapContext(), 1);
	_resqml2__FaultInterpretation* interp = static_cast<_resqml2__FaultInterpretation*>(gsoapProxy2_0_1);
	interp->Domain = resqml2__Domain__mixed;
	setInterpretedFeature(fault);

	initMandatoryMetadata();
	setMetadata(guid, title, "", -1, "", "", -1, "", "");
}

FaultInterpretation::FaultInterpretation(TectonicBoundaryFeature * fault, const string & guid, const string & title,
										GeneticBoundaryFeature * chronoTop, GeneticBoundaryFeature * chronoBtm)
{
	if (fault == nullptr) {
		throw invalid_argument("The interpreted fault cannot be null.");
	}
	if (fault->isAFracture()) {
		throw invalid_argument("The interpreted fault cannot be a fracture.");
	}

	gsoapProxy2_0_1 = soap_new_resqml2__obj_USCOREFaultInterpretation(fault->getGsoapContext(), 1);	
	_resqml2__FaultInterpretation* interp = static_cast<_resqml2__FaultInterpretation*>(gsoapProxy2_0_1);
	interp->Domain = resqml2__Domain__mixed;
	interp->InterpretedFeature = fault->newResqmlReference();

	interp->HasOccuredDuring = soap_new_resqml2__TimeInterval(interp->soap, 1);
	interp->HasOccuredDuring->ChronoBottom = chronoBtm->newResqmlReference();
	interp->HasOccuredDuring->ChronoTop = chronoTop->newResqmlReference();

	initMandatoryMetadata();
	setMetadata(guid, title, "", -1, "", "", -1, "", "");

	setInterpretedFeature(fault);
}

vector<Relationship> FaultInterpretation::getAllEpcRelationships() const
{
	vector<Relationship> result = BoundaryFeatureInterpretation::getAllEpcRelationships();

	for (size_t i = 0; i < structuralOrganizationInterpretationSet.size(); ++i) {
		if (structuralOrganizationInterpretationSet[i] != nullptr) {
			Relationship rel(structuralOrganizationInterpretationSet[i]->getPartNameInEpcDocument(), "", structuralOrganizationInterpretationSet[i]->getUuid());
			rel.setSourceObjectType();
			result.push_back(rel);
		}
		else {
			throw domain_error("The structural Organization Interpretation associated to the fault interpretation cannot be nullptr.");
		}
	}

	return result;
}

void FaultInterpretation::pushBackThrowInterpretation(const gsoap_resqml2_0_1::resqml2__ThrowKind & throwKind)
{
	_resqml2__FaultInterpretation* interp = static_cast<_resqml2__FaultInterpretation*>(gsoapProxy2_0_1);

	resqml2__FaultThrow * throwInterp = soap_new_resqml2__FaultThrow(gsoapProxy2_0_1->soap, 1);
	throwInterp->Throw.push_back(throwKind);

	interp->ThrowInterpretation.push_back(throwInterp);
}
