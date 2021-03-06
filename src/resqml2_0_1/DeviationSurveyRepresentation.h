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
#pragma once

#include "resqml2_0_1/WellboreTrajectoryRepresentation.h"

namespace resqml2
{
	class MdDatum;
}

namespace resqml2_0_1
{
	class WellboreTrajectoryRepresentation;

	class DLL_IMPORT_OR_EXPORT DeviationSurveyRepresentation : public resqml2::AbstractRepresentation
	{
	public:

		/**
		* Only to be used in partial transfer context
		*/
		DeviationSurveyRepresentation(gsoap_resqml2_0_1::eml20__DataObjectReference* partialObject) :AbstractRepresentation(partialObject) {}


		/**
		* Creates an instance of this class in a gsoap context.
		* @param interp					The WellboreFeature interpretation the instance represents.
		* @param guid					The guid to set to the new instance. If empty then a new guid will be generated.
		* @param title					A title for the instance to create.
		* @param isFinal				Used to indicate that this is a final version of the deviation survey, as distinct from the interim interpretations.
		* @param mdInfo					The MD information of the survey, mainly the well reference point.
		*/
		DeviationSurveyRepresentation(class WellboreInterpretation* interp, const std::string & guid, const std::string & title, const bool & isFinal, resqml2::MdDatum * mdInfo);

		/**
		* Creates an instance of this class by wrapping a gsoap instance.
		*/
		DeviationSurveyRepresentation(gsoap_resqml2_0_1::_resqml2__DeviationSurveyRepresentation* fromGsoap) : AbstractRepresentation(fromGsoap) {}

		~DeviationSurveyRepresentation() {}

		static const char* XML_TAG;
		virtual std::string getXmlTag() const {return XML_TAG;}

		/*
		*  Set the geometry of the representation by means of a parametric line without MD information.
		* @param firstStationLocation			XYZ location of the first station of the deviation survey in the local CRS. It must be three doubles. It is not created and not deleted by this method.
		* @param stationCount					Number of stations (including the first station location).
		* @param mdUom							Units of measure of the measured depths along this deviation survey.
		* @param mds							MD values for the position of the stations. Array length equals station count.
		* @param angleUom						Defines the units of measure for the azimuth and inclination
		* @param azimuths						An array of azimuth angles, one for each survey station. The rotation is relative to the projected CRS north with a positive value indicating a clockwise rotation as seen from above. Array length equals station count.
		* @param inclinations					Dip (or inclination) angle for each station. Array length equals station count.
		* @param proxy							The HDF proxy which indicates in which HDF5 file the numerical values will be stored.
		*										It must be already opened for writing and won't be closed.
		*/
		void setGeometry(double * firstStationLocation, const ULONG64 & stationCount,
			const gsoap_resqml2_0_1::eml20__LengthUom & mdUom, double * mds,
			const gsoap_resqml2_0_1::eml20__PlaneAngleUom & angleUom, double * azimuths, double * inclinations,
			common::AbstractHdfProxy* proxy);

		/**
		* Set the Md datum of this trajectory
		*/
		void setMdDatum(resqml2::MdDatum* mdDatum);

		/**
		* @return	null pointer if no md datum is associated to this representation. Otherwise return the data object reference of the associated md datum.
		*/
		gsoap_resqml2_0_1::eml20__DataObjectReference* getMdDatumDor() const;

		/**
		* Getter of the md information associated to this WellboreFeature trajectory representation.
		*/
		resqml2::MdDatum * getMdDatum() const;

		/**
		* Getter of the md information uuid associated to this WellboreFeature trajectory representation.
		*/
		std::string getMdDatumUuid() const;

		/**
		* Used to indicate that this is a final version of the deviation survey, as distinct from the interim interpretations.
		*/
		bool isFinal() const;

		/**
		* Get the xyz point count in a given patch.
		*/
		ULONG64 getXyzPointCountOfPatch(const unsigned int & patchIndex) const;

		/**
		* Get all the XYZ points of a particular patch of this representation.
		* XYZ points are given in the local CRS.
		* @param xyzPoints A linearized 2d array where the first (quickest) dimension is coordinate dimension (XYZ) and second dimension is vertex dimension. It must be pre allocated.
		*/
		void getXyzPointsOfPatch(const unsigned int & patchIndex, double * xyzPoints) const;

		/**
		* Units of measure of the measured depths along this deviation survey.
		*/
		gsoap_resqml2_0_1::eml20__LengthUom getMdUom() const;

		/**
		* Defines the units of measure for the azimuth and inclination
		*/
		gsoap_resqml2_0_1::eml20__PlaneAngleUom getAngleUom() const;

		/**
		* Getter of the md double values associated to each trajectory station of this WellboreFeature trajectory representation.
		* Uom is given by getMdUom().
		* @param values	It must preallocated with a count of getXyzPointCountOfPatch(0)
		*/
		void getMdValues(double* values);

		/**
		* Getter of the inclination double values associated to each trajectory station of this WellboreFeature trajectory representation.
		* Uom is given by getAngleUom().
		* @param values	It must preallocated with a count of getXyzPointCountOfPatch(0)
		*/
		void getInclinations(double* values);

		/**
		* Getter of the azimuth double values associated to each trajectory station of this WellboreFeature trajectory representation.
		* The rotation is relative to the projected CRS north with a positive value indicating a clockwise rotation as seen from above.
		* Uom is given by getAngleUom().
		* @param values	It must preallocated with a count of getXyzPointCountOfPatch(0)
		*/
		void getAzimuths(double* values);

		/**
		* Getter of all the wellbore frame representations of associated wellbore trajectory which share the same md datum and md uom.
		*/
		std::vector<class WellboreFrameRepresentation*> getWellboreFrameRepresentationSet() const;

		/**
		* Get the count of all the wellbore frame representations of associated wellbore trajectory which share the same md datum and md uom.
		* Necessary for now in SWIG context because I am not sure if I can always wrap a vector of polymorphic class yet.
		*/
		unsigned int getWellboreFrameRepresentationCount() const;

		/**
		* Get a particular wellbore frame representation according to its position.
		* Necessary for now in SWIG context because I am not sure if I can always wrap a vector of polymorphic class yet.
		* Throw an out of bound exception if the index is superior or equal to the count of wellbore frame representation.
		*/
		class WellboreFrameRepresentation* getWellboreFrameRepresentation(unsigned int index) const;

		/**
		* Getter (in read only mode) of all the wellbore trajectories which are associated to this deviation survey.
		*/
		const std::vector<class WellboreTrajectoryRepresentation*>& getWellboreTrajectoryRepresentationSet() const;

		/**
		* Get the count of all the wellbore trajectories which are associated to this deviation survey.
		* Necessary for now in SWIG context because I am not sure if I can always wrap a vector of polymorphic class yet.
		*/
		unsigned int getWellboreTrajectoryRepresentationCount() const;

		/**
		* Get a particular wellbore trajectory according to its position.
		* Necessary for now in SWIG context because I am not sure if I can always wrap a vector of polymorphic class yet.
		* Throw an out of bound exception if the index is superior or equal to the count of wellbore frame representation.
		*/
		class WellboreTrajectoryRepresentation* getWellboreTrajectoryRepresentation(const unsigned int & index) const;

		std::string getHdfProxyUuid() const;

		unsigned int getPatchCount() const {return 1;}

	protected:

		friend void WellboreTrajectoryRepresentation::setDeviationSurvey(DeviationSurveyRepresentation* deviationSurvey);

		void addTrajectory(class WellboreTrajectoryRepresentation* trajectory);

		std::vector<epc::Relationship> getAllEpcRelationships() const;
		void importRelationshipSetFromEpc(common::EpcDocument* epcDoc);
		
		// XML backward relationships
		std::vector<class WellboreTrajectoryRepresentation*> wbTrajectoryRepSet;
	};
}
