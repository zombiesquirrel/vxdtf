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
#include "../include/LittleHelper.h"

// #include <TMathBase.h>
#include <math.h>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <TVector3.h>
// #include <boost/lexical_cast.hpp>
#include <framework/logging/Logger.h>
#include <framework/datastore/StoreArray.h>
#include <generators/dataobjects/MCParticle.h>
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
	B2DEBUG(1, "StoreSensorinfo: radius " << radius<<", thickness "<< thickness<<", redRadLengthX0 "<< redRadLengthX0)
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



std::string HitExporter::storePXDTrueHit(VXD::GeoCache& geometry, const PXDTrueHit* aHit, int iD, bool doSmear, int isPrimaryBackgroundOrGhost, int particleID, int pdg)
{
	if ( isPrimaryBackgroundOrGhost != 0 and isPrimaryBackgroundOrGhost != 1 )  {
		isPrimaryBackgroundOrGhost = -1;
	}
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
	B2DEBUG(10,"within HitExporter::storePXDTrueHit. Hit " << iD << " with particleID " << particleID << " and pdg " << pdg << " isPrimaryBackgroundOrGhost: " << isPrimaryBackgroundOrGhost << " has been found in sensorID " << aVxdID << " at Layer " << aLayerID)
// 	const PXD::SensorInfo& geometry = dynamic_cast<const PXD::SensorInfo&>(VXD::GeoCache::get(m_sensorID));
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	return storeTrueHit(aSensorInfo, aHit->getU(), aHit->getV(), 0, iD, isPrimaryBackgroundOrGhost, aLayerID, particleID, pdg, doSmear);
}



std::string HitExporter::storeSVDTrueHit(VXD::GeoCache& geometry, const SVDTrueHit* aHit, int iD, bool doSmear, int isPrimaryBackgroundOrGhost, int particleID, int pdg)
{
	if ( isPrimaryBackgroundOrGhost != 0 and isPrimaryBackgroundOrGhost != 1 )  {
		isPrimaryBackgroundOrGhost = -1;
	}
	VxdID aVxdID = aHit->getSensorID();
  int aLayerID = aVxdID.getLayerNumber();
	B2DEBUG(10,"within HitExporter::storeSVDDTrueHit. Hit " << iD << " with particleID " << particleID << " and pdg " << pdg << " isPrimaryBackgroundOrGhost: " << isPrimaryBackgroundOrGhost << " has been found in sensorID " << aVxdID << " at Layer " << aLayerID)
// 	const PXD::SensorInfo& geometry = dynamic_cast<const PXD::SensorInfo&>(VXD::GeoCache::get(m_sensorID));
  VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
	return storeTrueHit(aSensorInfo, aHit->getU(), aHit->getV(), 1, iD, isPrimaryBackgroundOrGhost, aLayerID, particleID, pdg, doSmear);
}



std::string HitExporter::storeTrueHit(VXD::SensorInfoBase aSensorInfo, double u, double v, int type, int iD, int isPrimaryBackgroundOrGhost, int aLayerID, int particleID, int pdg, bool doSmear) {
	/// smear!!!
	LittleHelper smearBox = LittleHelper();
	double sigmaU = aSensorInfo.getUPitch(v)/sqrt(12.); // this is NOT a typo!
	double sigmaV = aSensorInfo.getVPitch(v)/sqrt(12.);
	if ( doSmear == true ) {
		B2DEBUG(100," u/v before smearing: " << u << "/" << v )
		u = smearBox.smearValueGauss(-1000., +1000., u, sigmaU);
		v = smearBox.smearValueGauss(-1000., +1000., v, sigmaV);
		B2DEBUG(100," u/v after smearing: " << u << "/" << v )
	}
	TVector3 hitLocal(u, v, 0.);
	TVector3 hitGlobal = aSensorInfo.pointToGlobal(hitLocal);
	TVector3 covValues(sigmaU*sigmaU, 0., sigmaV*sigmaV); // there are no good estimations for that values yet. Therefore, the classic pitch/sqrt(12) is used. The hardware guys do not expect any correlation between u and v, therefore CovUV = 0
	// determining a vector representing the global direction of the u-orientation of the sensor plane:
	TVector3 leftHit(-1., 0., 0.), rightHit(1., 0., 0.), xAxis(5., 0., 0.), origin(0.,0.,0.);
	leftHit = aSensorInfo.pointToGlobal(leftHit);
	rightHit = aSensorInfo.pointToGlobal(rightHit);
	TVector3 sensorUAxis = rightHit - leftHit;
	// getting angle:
	ThreeHitFilters filter;
// 	int signOfCurvature = filter.calcSign(xAxis, origin, sensorUAxis); // in this case, the value is positive, if the angle is below 180° (grad)
	double sensorAngle = sensorUAxis.	Phi() - 0.5*M_PI; // filter.fullAngle2D(xAxis, sensorUAxis); // angle in Radians (0°-180°)
// 	if ( signOfCurvature < 0 ) { sensorAngle = M_PI + sensorAngle; } else if ( signOfCurvature == 0 ) { }
	if ( sensorAngle > M_PI ) { sensorAngle -= 2.*M_PI; } else if ( sensorAngle < -M_PI ) { sensorAngle += 2.*M_PI; }

	B2DEBUG(10, " -> this hit has got U/V " << hitLocal.X() <<"/"<< hitLocal.Y() << " and CovUU/CovVV " << covValues.X() <<"/"<< covValues.Z() << " and sensorAngle " << sensorAngle )
	ExporterHitInfo aHitInfo(hitGlobal, covValues, aLayerID, sensorAngle, iD, type, isPrimaryBackgroundOrGhost, particleID, pdg);
	m_thisEvent->addHit(aHitInfo);
	string result = "";
	return result;
}



std::string HitExporter::storeGFTC(VXD::GeoCache& geometry, const GFTrackCand* aTC, int tcFileIndex, int tcSimIndex, std::vector<const PXDTrueHit*> pxdHits, std::vector<const SVDTrueHit*> svdHits, std::vector<int> hitIDs)
{
	StoreArray<MCParticle> mcParticles;
	TVector3 momentum = aTC->getMomSeed();
	TVector3 vertex = aTC->getPosSeed();
	double chargeD = aTC->getChargeSeed();
	int motherID = aTC->getMcTrackId();
	if ( mcParticles[motherID]->hasStatus(MCParticle::c_PrimaryParticle) == true ) { motherID = -1; } // primary particles have -1 here...
	int pdg = aTC->getPdgCode();
	double pVal = momentum.Mag();
	double pTVal = momentum.Perp();
	double qP = chargeD/pVal;
	momentum = momentum.Unit();
	B2DEBUG(1,"storeGFTC: momentum: " << pVal << ", pt: " << pTVal << ", qP: " << qP << ", pdg: " << pdg<< " and theta of vertex: " << momentum.Theta())

	ExporterTcInfo newTC(pdg, motherID);
	B2DEBUG(1," HitExporter::storeGFTC, executing tc with FileIndex/simIndex/motherID: " << tcFileIndex <<"/"<< tcSimIndex <<"/"<< motherID << " which has got " << pxdHits.size() <<"/"<< svdHits.size() << " hits")
	
	TVector3 hitMomentum, hitPosition;
	
	BOOST_FOREACH( const PXDTrueHit* hit, pxdHits ) {
		// collecting data:
		VxdID aVxdID = hit->getSensorID();
		VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
		int layerID = aVxdID.getLayerNumber();
		hitMomentum = hit->getMomentum();
		hitPosition.SetXYZ(hit->getU(), hit->getV(), 0);
		hitPosition = aSensorInfo.pointToGlobal(hitPosition);
		double hitQP = chargeD/(hitMomentum.Mag());
		double radius = hitPosition.Perp();
		
		// converting to string:
		stringstream hitInfo;
		hitInfo << hitPosition.X() << " " << hitPosition.Y() << " " << hitPosition.Z() << endl;
		hitInfo << hitMomentum.X() << " " << hitMomentum.Y() << " " << hitMomentum.Z() << " " << hitQP << endl;
		hitInfo << 0 << " " << layerID-1 << " " << tcFileIndex << " " << motherID << " " << endl;
		newTC.addHit(make_pair(radius, (hitInfo).str()));
	}
	
	BOOST_FOREACH( const SVDTrueHit* hit, svdHits ) {
		// collecting data:
		VxdID aVxdID = hit->getSensorID();
		VXD::SensorInfoBase aSensorInfo = geometry.getSensorInfo(aVxdID);
		int layerID = aVxdID.getLayerNumber();
		hitMomentum = hit->getMomentum();
		hitMomentum = aSensorInfo.vectorToGlobal(hitMomentum);
		hitPosition.SetXYZ(hit->getU(), hit->getV(), 0);
		hitPosition = aSensorInfo.pointToGlobal(hitPosition);
		double hitQP = chargeD/(hitMomentum.Mag());
		double radius = hitPosition.Perp();
		
		// converting to string:
		stringstream hitInfo;
		hitInfo << hitPosition.X() << " " << hitPosition.Y() << " " << hitPosition.Z() << endl;
		hitInfo << hitMomentum.X() << " " << hitMomentum.Y() << " " << hitMomentum.Z() << " " << hitQP << endl;
		hitInfo << 0 << " " << layerID-1 << " " << tcFileIndex << " " << motherID << " " << endl;
		newTC.addHit(make_pair(radius, (hitInfo).str()));
	}
	int nHits = newTC.getNHits();
	
	stringstream tcInfo;
	tcInfo << vertex.X() <<" "<< vertex.Y() <<" "<< vertex.Z() <<" "<< momentum.X() <<" "<< momentum.Y() <<" "<< momentum.Z() <<" "<< qP << endl;
	tcInfo << 0 <<" "<< 0 <<" "<< 0 <<" "<< 0 <<" "<< 0 <<" "<< 0 <<" "<< 0 << endl;
	tcInfo << pVal <<" "<< pTVal << endl;
	tcInfo << nHits <<" "<< nHits <<" "; // at this point, the Index of the first mcPoint is not known yet, therefore tcInfo is incomplete until export
	newTC.addInfo(tcInfo.str());
	newTC.addHitIDs(hitIDs);
	m_thisEvent->addTc(newTC);
	
	return "EVERYTHING IS FINE";
}



std::string HitExporter::exportAll(int runNumber, float bz)
{
	string hitsFileName, mcPointsFileName, mcTracksFileName, settingsFileName, hitLabesFileName;
	ofstream hitsFileStream, settingsFileStream, mcTracksFileStream, mcPointsFileStream, hitLabesFileStream;
	B2DEBUG(1," HitExporter::exportAll: got " << m_storedOutput.size() << " events, exporting to files ")
	
	// doing the eventXX_hits.data & eventXX_MCPoints.data & eventXX_MCTracks.data:
	int mcPointCounter, mcTrackCounter;
	BOOST_FOREACH(EventMapEntry eventInfo, m_storedOutput) {
		mcPointCounter = 0, mcTrackCounter = 0; // counting them manually
		typedef pair <int, int> HitIDRelation;
		vector< HitIDRelation > hitIDRelations; // hitIDRelations: stores hitIDs of all hits (real=mcPoints and background) in .first, and their connected tc in .second. If they are not connected with a tc, they are linked with -1
		// generating filenames
		hitsFileName = (boost::format("event%1%_hits.data") % eventInfo.first).str();
		mcPointsFileName = (boost::format("event%1%_MCPoints.data") %eventInfo.first).str();
		mcTracksFileName = (boost::format("event%1%_MCTracks.data") %eventInfo.first).str();
		hitLabesFileName = (boost::format("event%1%_hitLabels.data") % eventInfo.first).str();
		//opening files:
		hitsFileStream.open(hitsFileName.c_str(), std::ios_base::trunc); // trunc=overwrite app=append
		mcPointsFileStream.open(mcPointsFileName.c_str(), std::ios_base::trunc);
		mcTracksFileStream.open(mcTracksFileName.c_str(), std::ios_base::trunc);
		hitLabesFileStream.open(hitLabesFileName.c_str(), std::ios_base::trunc);
		// importing data:
		vector<ExporterHitInfo>* hitsOfEvent = NULL;
		hitsOfEvent = eventInfo.second->getHits();
		int nHits = hitsOfEvent->size();
		vector<ExporterTcInfo>* tcsOfEvent = NULL;
		tcsOfEvent = eventInfo.second->getTcs();
		int nMCtcs = tcsOfEvent->size();
		int nMCHits = eventInfo.second->getNMCHits();
		
		if (hitsOfEvent == NULL) {
			B2WARNING("event " << eventInfo.first <<" has no hits! Rejecting event...")
			continue;
		}
		if (tcsOfEvent == NULL) {
			B2WARNING("event " << eventInfo.first <<" has no tcs! Rejecting event...")
			continue;
		}
		B2DEBUG(1, " HitExporter::exportAll: executing event " << eventInfo.first << " and storing " << nHits << " hits, " << nMCHits << " mcPoints and " << nMCHits << " mcTracks in files" << hitsFileName << ", " << mcPointsFileName << " and " << mcTracksFileName)
		
		// writing first line of files:
		hitsFileStream << nHits << endl;
		mcPointsFileStream << nMCHits << endl;
		mcTracksFileStream << nMCtcs << endl;
		hitLabesFileStream << nHits << endl;
		
		// executing hits
		BOOST_FOREACH(ExporterHitInfo hit, (*hitsOfEvent)) {
			hitIDRelations.push_back(make_pair(hit.getHitID(), -1)); // stores the hitIDs in the order of which they were written into the file. This order should not be destroyed, .second is -1 until a TC is found
			hitsFileStream << hit.getPositionFormatted() << hit.getCovValuesFormatted() << hit.getAdditionalInfoFormatted();
		} // looping over hits
		
		// executing tcs and mcPoints
		BOOST_FOREACH(ExporterTcInfo tc, (*tcsOfEvent)) {
			// importing data
			vector<int>* tcHitIDs = tc.getHitIDs(); // tcHitIDs stores hitIDs of mcPoints attached to tc
			vector< pair<double, string> >* hits = tc.getHits();
			std::sort(hits->begin(), hits->end()); // to assure, that the innermost hit is the first one
			int nMChits = hits->size();
			int pdg = tc.getPdgCode();
			int motherID = tc.getMotherID();
			
			// doing mcTracks
			mcTracksFileStream << motherID << " " << pdg << endl;
			mcTracksFileStream << tc.getInfo() << " " << mcPointCounter << endl;
			mcTracksFileStream << 0 << " " << 0 << " " << 1 << endl;
			
			B2DEBUG(10, "->HitExporter::exportAll: executing tc with pdgCode/motherID " << pdg <<"/"<< motherID << " and adding " << nMChits << " hits to " << mcTracksFileName)
			
			// doing mcPoints
			BOOST_FOREACH(TcHitEntry hit, (*hits)) {
				mcPointsFileStream << hit.second;
				++mcPointCounter;
			}
			
			// doing hitLabels, atm I am using truehits, which are caused by only one tc. when changing to clusters, this method has to be rewritten suiting for the cases where more than one tc is using the same hit (or at least one of its strips)
			BOOST_FOREACH(HitIDRelation &hitRelation, hitIDRelations) { /// the '&' is important since I want to add information. Without the '&' I am only writing into a local copy of hitIDRelations
				BOOST_FOREACH(int tcHitID, (*tcHitIDs)) {
					if (hitRelation.first == tcHitID) { hitRelation.second = mcTrackCounter; break; }
				}
			}
			
			++mcTrackCounter;
		} // looping over tcs
		
		// storing hitLabes, has to be done out of the tc-loop to be independent of tc-behavior (if there are no tcs, rel.second will be -1):
		BOOST_FOREACH(HitIDRelation rel, hitIDRelations) {
			hitLabesFileStream << rel.second << " " << -1 << " " << -1 << endl;
		}
		
		if ( mcPointCounter != nMCHits) {
			B2ERROR("HitExporter::exportAll:: num of MC hits is wrong! is: " << nMCHits << ", should: " << mcPointCounter)
		}
		
		mcTracksFileStream << endl;
		mcPointsFileStream << endl;
		hitLabesFileStream << endl;
		hitsFileStream.close();
		mcTracksFileStream.close();
		mcPointsFileStream.close();
		hitLabesFileStream.close();
	} // looping over events
	
	//doing settings.data
	settingsFileName = "settings.data";
	int nLayers = m_geometry.size();
	B2DEBUG(5, " HitExporter::exportAll: generating settings-file and storing " << nLayers << " layers in " << settingsFileName)
	settingsFileStream.open(settingsFileName.c_str(), std::ios_base::trunc);
	settingsFileStream << nLayers << endl << bz << endl; // TODO bz-Value should be able to be read from up-to-date source in case of changing values.
	BOOST_FOREACH(string layer, m_geometry) {
		settingsFileStream << layer << endl;
	}
	settingsFileStream << endl;
	settingsFileStream.close();


	return "EVERYTHING IS FINE";
}
