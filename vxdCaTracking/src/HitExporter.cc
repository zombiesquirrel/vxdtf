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



// void HitExporter::initializeTrueHitContainer(std::string detectorType)
// {
// 	
// }
// 
// void HitExporter::initializeClusterContainer(std::string detectorType)
// {
// 	
// }
// 
// void void HitExporter::initializeGFContainer(std::string arrayName)
// {
// 	
// }


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

// std::string HitExporter::storeGFTrackCands(StoreArray<GFTrackCand>* pArray, std::string arrayName)
// {
// 	B2FATAL("Hitexporter::storeGFTrackCands: member function not implemented yet!")
// 	return "bleh";
// }



std::string HitExporter::storePXDTrueHit(VXD::GeoCache& geometry, PXDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost, int particleID)
{
	if ( isPrimaryBackgroundOrGhost != 0 and isPrimaryBackgroundOrGhost != 1 )  {
		isPrimaryBackgroundOrGhost = -1;
	}
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
	B2DEBUG(10,"within HitExporter::storePXDTrueHit. Hit " << iD << " with particleID " << particleID << " isPrimaryBackgroundOrGhost: " << isPrimaryBackgroundOrGhost << " has been found in sensorID " << aVxdID << " at Layer " << aLayerID)
// 	const PXD::SensorInfo& geometry = dynamic_cast<const PXD::SensorInfo&>(VXD::GeoCache::get(m_sensorID));
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	storeTrueHit(aSensorInfo, aHit->getU(), aHit->getV(), 0, iD, aLayerID, particleID, pID);
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
	if ( signOfCurvature < 0 ) { sensorAngle = 180. + sensorAngle; }
// 	double sensorAngle = 0.; /// WARNING TODO, wie komme ich an den ran? Es geht um den Winkel zwischen dem der x-Achse und dem orthogonal-Winkel der Sensorfläche in globalen Koords (im Uhrzeigersinn gedreht), dh reine winkel-zwischen-2-Vektoren-Rechnung tuts nicht...
	
	B2DEBUG(10, " -> this hit has got U/V " << hitLocal.X() <<"/"<< hitLocal.Y() << " and CovUU/CovVV " << covValues.X() <<"/"<< covValues.Z() << " and sensorAngle " << sensorAngle )
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, type, isPrimaryBackgroundOrGhost, particleID);
	m_thisEvent->addHit(aHitInfo);
	string result = "";
	return result;
}



std::string HitExporter::storeSVDTrueHit(VXD::GeoCache& geometry, SVDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost, int particleID)
{
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
	B2DEBUG(10,"within HitExporter::storeSVDTrueHit. Hit " << iD << " with particleID " << particleID << " isPrimaryBackgroundOrGhost: " << isPrimaryBackgroundOrGhost << " has been found in sensorID " << aVxdID << " at Layer " << aLayerID)
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	double sigmaU = aSensorInfo.getUPitch(aHit->getV()/sqrt(12)); // this is NOT a typo!
	double sigmaV = aSensorInfo.getVPitch(aHit->getV()/sqrt(12));
	TVector3 hitLocal(aHit->getU(), aHit->getV(), 0);
	TVector3 hitGlobal = aSensorInfo.pointToGlobal(hitLocal);
	TVector3 covValues(sigmaU*sigmaU, 0, sigmaV*sigmaV); // since these are 1D hits combined, there is definitely no correlation between u and v, CovUU & CovVV are rough estimations since there are no realistic estimations yet
	double sensorAngle = 0.; /// WARNING TODO, wie komme ich an den ran? Es geht um den Winkel zwischen dem der x-Achse und dem orthogonal-Winkel der Sensorfläche in globalen Koords (im Uhrzeigersinn gedreht), dh reine winkel-zwischen-2-Vektoren-Rechnung tuts nicht...
	B2DEBUG(10, " -> this hit has got U/V " << hitLocal.X() <<"/"<< hitLocal.Y() << " and CovUU/CovVV " << covValues.X() <<"/"<< covValues.Z() << " and sensorAngle " << sensorAngle )
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, 1, isPrimaryBackgroundOrGhost, particleID);
	m_thisEvent->addHit(aHitInfo);
	string result = "";
	return result;
}



std::string HitExporter::storeTrueHit(VXD::SensorInfoBase aSensorInfo, double u, double v, int type, int iD, int aLayerID, int particleID, int pID) {
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
	if ( signOfCurvature < 0 ) { sensorAngle = 180. + sensorAngle; }
// 	double sensorAngle = 0.; /// WARNING TODO, wie komme ich an den ran? Es geht um den Winkel zwischen dem der x-Achse und dem orthogonal-Winkel der Sensorfläche in globalen Koords (im Uhrzeigersinn gedreht), dh reine winkel-zwischen-2-Vektoren-Rechnung tuts nicht...
	
	B2DEBUG(10, " -> this hit has got U/V " << hitLocal.X() <<"/"<< hitLocal.Y() << " and CovUU/CovVV " << covValues.X() <<"/"<< covValues.Z() << " and sensorAngle " << sensorAngle )
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, type, isPrimaryBackgroundOrGhost, particleID);
	m_thisEvent->addHit(aHitInfo);
	string result = "";
	return result;
}


// std::string HitExporter::storePXDClusters(StoreArray<PXDCluster>* pArray)
// {
// 	B2FATAL("Hitexporter::storePXDClusters: member function not implemented yet!")
// 	return "bleh";
// }
// 
// std::string HitExporter::storeSVDClusters(StoreArray<SVDCluster>* pArray)
// {
// 	B2FATAL("Hitexporter::storeSVDClusters: member function not implemented yet!")
// 	/// hier haben wir wieder mal das Problem mit den 2 IDs für einen gescheiten hit. wie lösen?
// 	return "bleh";
// }



std::string HitExporter::exportAll(int runNumber, std::string name)
{
	string fileName;
	ofstream outputFileStream;
	B2DEBUG(1," HitExporter::exportAll: got " << m_storedOutput.size() << " events, exporting to file " << (boost::format("event%1%_%2%") % m_storedOutput.find(m_eventNumber)->first % name).str())
	if ( name != string("") ) {
		
		BOOST_FOREACH(EventMapEntry eventInfo, m_storedOutput) {
			// generating filename
			if ( name != string("") ) {
				fileName = (boost::format("event%1%_%2%") % eventInfo.first % name).str();
			} else {
				fileName = (boost::format("run%1%_event%2%_hits") %runNumber %eventInfo.first).str();
			}
			//opening file and exporting data
			outputFileStream.open(fileName.c_str(), std::ios_base::trunc); // trunc=overwrite app=append
			vector<ExporterHitInfo>* hitsOfEvent = NULL;
// 			eventInfo.second->getHits(hitsOfEvent);
			hitsOfEvent = eventInfo.second->getHits();
			if (hitsOfEvent == NULL) {
				B2WARNING("event " << eventInfo.first <<" has no hits! Rejecting event...")
				continue;
			}
			int nHits = hitsOfEvent->size();
			outputFileStream << nHits << endl;
			BOOST_FOREACH(ExporterHitInfo hit, (*hitsOfEvent)) {
				outputFileStream << hit.getPositionFormatted() << hit.getCovValuesFormatted() << hit.getAdditionalInfoFormatted();
			}
			outputFileStream << endl;
			outputFileStream.close();
		}
	}
	return "EVERYTHING IS FINE";
}
