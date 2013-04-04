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

// #include <TMathBase.h>
#include <math.h>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <TVector3.h>
// #include <boost/lexical_cast.hpp>
#include <framework/logging/Logger.h>
#include <vxd/geometry/SensorInfoBase.h>
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
// 	const PXD::SensorInfo& geometry = dynamic_cast<const PXD::SensorInfo&>(VXD::GeoCache::get(m_sensorID));
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	double sigmaU = aSensorInfo.getUPitch(aHit->getV()/sqrt(12)); // this is NOT a typo!
	double sigmaV = aSensorInfo.getVPitch(aHit->getV()/sqrt(12));
	TVector3 hitLocal(aHit->getU(), aHit->getV(), 0);
	TVector3 hitGlobal = aSensorInfo.pointToGlobal(hitLocal);
	TVector3 covValues(sigmaU*sigmaU, 0, sigmaV*sigmaV); // there are no good estimations for that values yet. Therefore, the classic pitch/sqrt(12) is used. The hardware guys do not expect any correlation between u and v, therefore CovUV = 0
	double sensorAngle = 0.; /// WARNING TODO, wie komme ich an den ran? Es geht um den Winkel zwischen dem der x-Achse und dem orthogonal-Winkel der Sensorfläche in globalen Koords (im Uhrzeigersinn gedreht), dh reine winkel-zwischen-2-Vektoren-Rechnung tuts nicht...
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, 0, isPrimaryBackgroundOrGhost, particleID);
	m_thisEvent->addHit(aHitInfo);
	string result = "";
	return result;
}



std::string HitExporter::storeSVDTrueHit(VXD::GeoCache& geometry, SVDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost, int particleID)
{
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	double sigmaU = aSensorInfo.getUPitch(aHit->getV()/sqrt(12)); // this is NOT a typo!
	double sigmaV = aSensorInfo.getVPitch(aHit->getV()/sqrt(12));
	TVector3 hitLocal(aHit->getU(), aHit->getV(), 0);
	TVector3 hitGlobal = aSensorInfo.pointToGlobal(hitLocal);
	TVector3 covValues(sigmaU*sigmaU, 0, sigmaV*sigmaV); // since these are 1D hits combined, there is definitely no correlation between u and v, CovUU & CovVV are rough estimations since there are no realistic estimations yet
	double sensorAngle = 0.; /// WARNING TODO, wie komme ich an den ran? Es geht um den Winkel zwischen dem der x-Achse und dem orthogonal-Winkel der Sensorfläche in globalen Koords (im Uhrzeigersinn gedreht), dh reine winkel-zwischen-2-Vektoren-Rechnung tuts nicht...
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, 1, isPrimaryBackgroundOrGhost, particleID);
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
			eventInfo.second->getHits(hitsOfEvent);
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
