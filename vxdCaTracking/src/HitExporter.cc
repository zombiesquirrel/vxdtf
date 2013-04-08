/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "../include/ExporterHitInfo.h"
#include "../include/ExporterTcInfo.h"
#include "../include/ExporterEventInfo.h"
#include "../include/HitExporter.h"
#include "../include/ThreeHitFilters.h"

// #include <TMathBase.h>
#include <math.h>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <TVector3.h>
// #include <boost/lexical_cast.hpp>
#include <framework/logging/Logger.h>
#include <vxd/dataobjects/VxdID.h>
#include <pxd/dataobjects/PXDCluster.h>
#include <svd/dataobjects/SVDCluster.h>
#include <pxd/dataobjects/PXDTrueHit.h>
#include <svd/dataobjects/SVDTrueHit.h>
// #include <list>
// #include <iostream>

using namespace std;
using namespace Belle2;
using boost::format;



HitExporter::~HitExporter() {
	BOOST_FOREACH(EventMapEntry anEvent, m_storedOutput) {
		delete anEvent.second;
  }
  m_storedOutput.clear();
}



void HitExporter::storeSensorInfo(const VXD::SensorInfoBase & aSensorInfo)
{
	/** Info dumping:
	 * Radiation length:
	 *
	 * The radiation length is the mean distanc eover which a high-energy electron loses all but 1/e of its energy by bremsstrahlung.
	 * For silicon, the radiation length is 21.82g/cm² (9.36cm)
	 * The density of silicon is 2,336 g/cm³
	**/
	double radLengthX0 = 21.82; // g/cm ⁻²
	double density = 2.336; // g/cm³
	double redRadLengthX0 = radLengthX0/density; // cm
	TVector3 local(0,0,0);
	double thickness = aSensorInfo.getThickness();
// 	double thicknessInRadiationLength = thickness/radLengthX0;
// 	double thicknessInRadiationLengthNoUnits = thickness*density/radLengthX0;
	TVector3 globalSensorPos = aSensorInfo.pointToGlobal(local); // center of the sensor in global coords
	double radius = globalSensorPos.Perp();
	string formattedSensorInfo = (boost::format("%1% %2% %3%") %radius %(thickness/redRadLengthX0) %(thickness*density)).str();
	m_geometry.push_back(formattedSensorInfo);
}



void HitExporter::prepareEvent(int n)
{
	m_eventNumber = n;
	ExporterEventInfo* pEventInfo = new ExporterEventInfo(n);
	m_storedOutput.insert(make_pair(n, pEventInfo));
	m_thisEvent = pEventInfo;
	B2DEBUG(1,"HitExporter::prepareEvent: address of m_thisEvent is pointing at: " << m_thisEvent << " map containing events has " << m_storedOutput.size() << "entries and event " << m_eventNumber << " has an event pointing at " << m_storedOutput.find(m_eventNumber)->second  )
}



int HitExporter::getCurrentEventNumber() {
	return m_thisEvent->getEventNumber();
}



int HitExporter::getNumberOfHits() {
	std::vector<ExporterHitInfo>* hits = NULL;
	B2DEBUG(10,"HitExporter::getNumberOfHits, before getting hits, vector points at " << hits)
// 	m_thisEvent->getHits(hits);
	hits = m_thisEvent->getHits();
	B2DEBUG(10,"HitExporter::getNumberOfHits, after getting hits, vector points at " << hits)
	return hits->size();
}



int HitExporter::getNumberOfPXDTrueHits() {
	return m_thisEvent->getNPXDTrueHits();
}



int HitExporter::getNumberOfSVDTrueHits() {
	return m_thisEvent->getNSVDTrueHits();
}



std::string HitExporter::storePXDTrueHit(VXD::GeoCache& geometry, const PXDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost, int particleID, int pdg)
{
	if ( isPrimaryBackgroundOrGhost != 0 and isPrimaryBackgroundOrGhost != 1 )  {
		isPrimaryBackgroundOrGhost = -1;
	}
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
	B2DEBUG(10,"within HitExporter::storePXDTrueHit. Hit " << iD << " with particleID " << particleID << " and pdg " << pdg << " isPrimaryBackgroundOrGhost: " << isPrimaryBackgroundOrGhost << " has been found in sensorID " << aVxdID << " at Layer " << aLayerID)
// 	const PXD::SensorInfo& geometry = dynamic_cast<const PXD::SensorInfo&>(VXD::GeoCache::get(m_sensorID));
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	return storeTrueHit(aSensorInfo, aHit->getU(), aHit->getV(), 0, iD, isPrimaryBackgroundOrGhost, aLayerID, particleID, pdg);
}



std::string HitExporter::storeSVDTrueHit(VXD::GeoCache& geometry, const SVDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost, int particleID, int pdg)
{
	if ( isPrimaryBackgroundOrGhost != 0 and isPrimaryBackgroundOrGhost != 1 )  {
		isPrimaryBackgroundOrGhost = -1;
	}
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
	B2DEBUG(10,"within HitExporter::storeSVDDTrueHit. Hit " << iD << " with particleID " << particleID << " and pdg " << pdg << " isPrimaryBackgroundOrGhost: " << isPrimaryBackgroundOrGhost << " has been found in sensorID " << aVxdID << " at Layer " << aLayerID)
// 	const PXD::SensorInfo& geometry = dynamic_cast<const PXD::SensorInfo&>(VXD::GeoCache::get(m_sensorID));
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	return storeTrueHit(aSensorInfo, aHit->getU(), aHit->getV(), 1, iD, isPrimaryBackgroundOrGhost, aLayerID, particleID, pdg);
}



std::string HitExporter::storeTrueHit(VXD::SensorInfoBase aSensorInfo, double u, double v, int type, int iD, int isPrimaryBackgroundOrGhost, int aLayerID, int particleID, int pdg) {
	double sigmaU = aSensorInfo.getUPitch(v/sqrt(12)); // this is NOT a typo!
	double sigmaV = aSensorInfo.getVPitch(v/sqrt(12));
	TVector3 hitLocal(u, v, 0);
	TVector3 hitGlobal = aSensorInfo.pointToGlobal(hitLocal);
	TVector3 covValues(sigmaU*sigmaU, 0, sigmaV*sigmaV); // there are no good estimations for that values yet. Therefore, the classic pitch/sqrt(12) is used. The hardware guys do not expect any correlation between u and v, therefore CovUV = 0
	// determining a vector representing the global direction of the u-orientation of the sensor plane:
	TVector3 leftHit(-1, 0, 0), rightHit(1, 0, 0), xAxis(1, 0, 0), origin(0,0,0);
	leftHit = aSensorInfo.pointToGlobal(leftHit);
	rightHit = aSensorInfo.pointToGlobal(rightHit);
	TVector3 sensorUAxis = rightHit - leftHit;
	// getting angle:
	ThreeHitFilters filter;
	int signOfCurvature = filter.calcSign(xAxis, origin, sensorUAxis); // in this case, the value is positive, if the angle is below 180° (grad)
	double sensorAngle = filter.fullAngle2D(xAxis, sensorUAxis); // angle in Radians (0°-180°)
	if ( signOfCurvature < 0 ) { sensorAngle = M_PI + sensorAngle; } else if ( signOfCurvature == 0 ) { }
// 	double sensorAngle = 0.; /// WARNING TODO, wie komme ich an den ran? Es geht um den Winkel zwischen dem der x-Achse und dem orthogonal-Winkel der Sensorfläche in globalen Koords (im Uhrzeigersinn gedreht), dh reine winkel-zwischen-2-Vektoren-Rechnung tuts nicht...
	
	B2DEBUG(10, " -> this hit has got U/V " << hitLocal.X() <<"/"<< hitLocal.Y() << " and CovUU/CovVV " << covValues.X() <<"/"<< covValues.Z() << " and sensorAngle " << sensorAngle )
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, type, isPrimaryBackgroundOrGhost, particleID, pdg);
	m_thisEvent->addHit(aHitInfo);
	string result = "";
	return result;
}



std::string HitExporter::storeGFTC(VXD::GeoCache& geometry, const GFTRackCand* aTC, int tcIndex, std::vector<const PXDTrueHit*> pxdHits, std::vector<const SVDTrueHit*> svdHits)
{
	TVector3 momentum = aTC->getMomSeed();
	TVector3 vertex = aTC->getPosSeed();
	double chargeD = aTC->getChargeSeed();
	int chargeI =  (double)(chargeD);
	double pVal = momentum.Mag();
	double pTVal = momentum.Perp();
	double qP = chargeD/pVal;
	TVector3 momentumU = momentum.Unit();
	TVector3 hitMomentum, hitPosition;
	BOOST_FOREACH( const PXDTrueHit* hit, pxdHits ) {
		hitMomentum = hit->getMomentum();
		hitPosition.SetXYZ(hit->getU(), hit->getV(), 0,);
		hitPosition = aSensorInfo.pointToGlobal(hitPosition);
		double hitQP = chargeD/(hitMomentum.Mag());
	}
	BOOST_FOREACH( const PXDTrueHit* hit, svdHits ) {
		
	}
}



std::string HitExporter::exportAll(int runNumber, std::string name, float bz)
{
	string fileName;
	ofstream hitFileStream, settingFileStream;
	B2DEBUG(1," HitExporter::exportAll: got " << m_storedOutput.size() << " events, exporting to file " << (boost::format("event%1%_%2%") % m_storedOutput.find(m_eventNumber)->first % name).str())
	
	// doing the eventXX_hits.data:
	if ( name != string("") ) {
		
		BOOST_FOREACH(EventMapEntry eventInfo, m_storedOutput) {
			// generating filename
			if ( name != string("") ) {
				fileName = (boost::format("event%1%_%2%") % eventInfo.first % name).str();
			} else {
				fileName = (boost::format("run%1%_event%2%_hits") %runNumber %eventInfo.first).str();
			}
			//opening file and exporting data
			hitFileStream.open(fileName.c_str(), std::ios_base::trunc); // trunc=overwrite app=append
			vector<ExporterHitInfo>* hitsOfEvent = NULL;
// 			eventInfo.second->getHits(hitsOfEvent);
			hitsOfEvent = eventInfo.second->getHits();
			if (hitsOfEvent == NULL) {
				B2WARNING("event " << eventInfo.first <<" has no hits! Rejecting event...")
				continue;
			}
			int nHits = hitsOfEvent->size();
			hitFileStream << nHits << endl;
			BOOST_FOREACH(ExporterHitInfo hit, (*hitsOfEvent)) {
				hitFileStream << hit.getPositionFormatted() << hit.getCovValuesFormatted() << hit.getAdditionalInfoFormatted();
			}
			hitFileStream << endl;
			hitFileStream.close();
		}
	}
	
	//doing settings.data
	fileName = "settings.data";
	int nLayers = m_geometry.size();
	settingFileStream.open(fileName.c_str(), std::ios_base::trunc);
	settingFileStream << nLayers << endl << bz << endl; // TODO bz-Value should be able to be read from up-to-date source in case of changing values.
	BOOST_FOREACH(string layer, m_geometry) {
		settingFileStream << layer << endl;
	}
	settingFileStream << endl;
	settingFileStream.close();
	return "EVERYTHING IS FINE";
}
