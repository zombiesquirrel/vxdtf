/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2011 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler, Moritz Nadler                       *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "tracking/modules/VXDTFHelperTools/NonRootDataExportModule.h"
#include <boost/foreach.hpp>

#include <framework/gearbox/Const.h>
#include <vxd/geometry/GeoCache.h>
#include <vxd/geometry/SensorInfoBase.h>
#include <framework/dataobjects/EventMetaData.h>
#include <framework/datastore/StoreObjPtr.h>
#include <framework/datastore/StoreArray.h>
#include <generators/dataobjects/MCParticle.h>
#include <framework/datastore/RelationIndex.h>
#include <pxd/dataobjects/PXDCluster.h>
#include <svd/dataobjects/SVDCluster.h>
#include <pxd/dataobjects/PXDTrueHit.h>
#include <svd/dataobjects/SVDTrueHit.h>
#include <GFTrackCand.h>
#include "tracking/vxdCaTracking/HitExporter.h"


using namespace std;
using namespace Belle2;

//-----------------------------------------------------------------
//                 Register the Module
//-----------------------------------------------------------------
REG_MODULE(NonRootDataExport)

//-----------------------------------------------------------------
//                 Implementation
//-----------------------------------------------------------------



NonRootDataExportModule::NonRootDataExportModule() : Module()
{
  //Set module properties
  setDescription("allows export of various data objects into files");
  setPropertyFlags(c_ParallelProcessingCertified | c_InitializeInProcess);
	int minTCLength = 1;
	
	addParam("exportTrueHits", m_PARAMExportTrueHits, "allows you to export true hits. Please choose between 'all' hits, 'real' hits, 'background' hits or 'none', which is standard. Wrong input values will set to none with an error.", string("none"));
	addParam("exportGFTCs", m_PARAMExportGFTCs, "allows you to export mcInformation about whole tracks, set true for tcOutput", bool(false));
	addParam("detectorType", m_PARAMDetectorType, "set detectorype. Please choose between 'PXD', 'SVD' (standard) or 'VXD'. Wrong input values will set to SVD with an error.", string("SVD"));
	addParam("minTCLength", m_PARAMminTCLength, "tracks with less than minTCLength hits will be neglected", minTCLength);
	addParam("smearTrueHits", m_PARAMsmearTrueHits, "when using trueHits, hits and mcPoints have got exactly the same position. If you activate the smearing, the hits will be smeared using the sensor resolution", bool(false));
	
}



NonRootDataExportModule::~NonRootDataExportModule()
{

}



void NonRootDataExportModule::initialize()
{
	
	B2INFO("NonRootDataExportModule::initialize() \n checking and setting initial parameters")
	
	if (m_PARAMDetectorType != "PXD" and m_PARAMDetectorType != "SVD" and m_PARAMDetectorType != "VXD" ) { // detectorType
		B2ERROR(" chosen value '" << m_PARAMDetectorType << "' for parameter detectorType is unknown, setting to 'SVD'")
		m_PARAMDetectorType = "SVD";
	}
	
	if (m_PARAMExportTrueHits == "real" or m_PARAMExportTrueHits == "background" or m_PARAMExportTrueHits == "all") { // exportTrueHits
		if (m_PARAMDetectorType == "PXD" or "VXD") {
			StoreArray<PXDTrueHit>::required();
		}
		if (m_PARAMDetectorType == "SVD" or "VXD") {
			StoreArray<SVDTrueHit>::required();
		}
	} else {
		B2ERROR(" chosen value '" << m_PARAMExportTrueHits << "' for parameter exportTrueHits is unknown, setting to 'none'")
		m_PARAMExportTrueHits = "none";
	}
	
	if (m_PARAMExportGFTCs == true ) {
		StoreArray<GFTrackCand>::required();
	}

	StoreArray<MCParticle>::required();
	B2INFO("NonRootDataExportModule: chosen detectorType: " << m_PARAMDetectorType << ", exporting TrueHits: " << m_PARAMExportTrueHits << " and exporting GFTrackCands: " << m_PARAMExportGFTCs << ", minimal ndf number of hits for tcs: " << m_PARAMminTCLength)
}



void NonRootDataExportModule::beginRun()
{
	m_eventCounter = 0;
	m_runCounter = 0;
	
	// importing whole geometry information:
	// we don't need the whole geometry information, but only a guess for each layer, therefore we only store one sensor per layer. Since the info is stored within a set which needs a key to reveal its info, I have to loop over all entries until I find the right entries:
	VXD::GeoCache& aGeometry = VXD::GeoCache::getInstance();
	set< VxdID > layers = aGeometry.getLayers(); // SensorInfoBase::SensorType sensortype=SensorInfoBase::VXD
	double forwardWidth, backwardWidth, diff;
	BOOST_FOREACH(VxdID layer, layers) {
		const set<VxdID>& ladders = aGeometry.getLadders(layer);
		bool stopLoop = false;
		BOOST_FOREACH(VxdID ladder, ladders) {
			const set<VxdID>& sensors = aGeometry.getSensors(ladder);
			BOOST_FOREACH(VxdID sensor, sensors) {
				const VXD::SensorInfoBase& aSensorInfo = aGeometry.getSensorInfo(sensor);
				if ( (aSensorInfo.getType() == 0 and m_PARAMDetectorType == "SVD") or (aSensorInfo.getType() == 1 and m_PARAMDetectorType == "PXD")) {
					stopLoop = true;
					break;
				}
				
				forwardWidth = aSensorInfo.getForwardWidth();
				backwardWidth = aSensorInfo.getBackwardWidth();
				diff = forwardWidth - backwardWidth;
				if ( diff < 0.0001  and diff > -0.0001) { // to prevent rounding errora, we don't want to store wedge/slanted sensors
					m_exportContainer.storeSensorInfo(aSensorInfo);
					stopLoop = true;
					break;
				}
			}
			if ( stopLoop == true ) { break; }
		}
	}
}



void NonRootDataExportModule::event()
{
	StoreObjPtr<EventMetaData> eventMetaDataPtr("EventMetaData", DataStore::c_Event);
	m_eventCounter = eventMetaDataPtr->getEvent();
	m_exportContainer.prepareEvent(m_eventCounter);
	
	StoreArray<MCParticle> mcParticles;
	VXD::GeoCache& aGeometry = VXD::GeoCache::getInstance();
	/// TODO statt Verlinkungen mit RelationIndex -> einfach folgende Zeilen nehmen!
// 	RelationVector<MCParticle> mcRelations = hit->getRelationsFrom<MCParticle>();
// 		B2DEBUG(10, " within HitExporter::storeGFTC: produced mcRelations-Vector with size: " << mcRelations.size() << " containing all McParticles responsible for a pxdHit")

	
	// looping over all mcparticles connected to each TrueHit, if there is at least one primary particle connected to current trueHit, the first found primary particle determines the state of the trueHit (means, if there is a primary particle attached to the trueHit, the trueHit is recognized as realhit, and recognized as background hit if otherwise, the particleID of the hit will be the first primary particle or the last secondary one attached to current trueHit):
	int nPXDTrueHits = 0, nSVDTrueHits = 0, particleID = -1, pdg, isPrimary = 1;
	if (m_PARAMExportTrueHits != "none") { // storing trueHits
		
		if (m_PARAMDetectorType == "PXD" or m_PARAMDetectorType == "VXD") { // storing pxd truehits
			StoreArray<PXDTrueHit> pxdTrueHits; // carries all trueHits of event
			nPXDTrueHits = pxdTrueHits.getEntries();
			B2DEBUG(1,"NonRootDataExportModule event " << m_eventCounter << ": executing " << nPXDTrueHits << " PXDTrueHits")
			RelationIndex<MCParticle, PXDTrueHit> mcParticlesToPxdTrueHits; // allows us to find out which particle caused which truehit
			typedef RelationIndex<MCParticle, PXDTrueHit>::Element RelationElement;
			
			for( int i = 0; i < nPXDTrueHits; ++i ) {
				const PXDTrueHit* aTrueHit = pxdTrueHits[i];
				
				BOOST_FOREACH(const RelationElement & rel, mcParticlesToPxdTrueHits.getElementsTo(aTrueHit)) {
					const MCParticle* particle = rel.from;
					particleID = particle->getIndex(); // 1-based index of Particle. If you want to have a 0-based index, use getArrayIndex() instead
					pdg = particle->getPDG();
					
					if ((particle->hasStatus(MCParticle::c_PrimaryParticle) == true) and (m_PARAMExportTrueHits == "real")) {
						isPrimary = 0;
						break; // if there are any other true or background particles, they shall be ignored
					}
				}
				B2DEBUG(10," PXDTrueHit with ID " << i << " is attached to mcParticles with ID " << particleID << " and hasStatusPrimary (0 = true) " << isPrimary)
				
				if ((m_PARAMExportTrueHits == "real" and isPrimary != 0) or (m_PARAMExportTrueHits == "background" and isPrimary != 1) or (m_PARAMExportTrueHits == "all" and isPrimary == -1)) {
					isPrimary = 1;
					particleID = -1;
					 B2DEBUG(10, " PXD hit " << i << " neglected")
					continue;
				}
				m_exportContainer.storePXDTrueHit(aGeometry, aTrueHit, i, m_PARAMsmearTrueHits, isPrimary, particleID, pdg);
				isPrimary = 1;
				particleID = -1;
			} // looping over all pxd trueHits
		} // storing pxdTrueHits
		
		
		if (m_PARAMDetectorType == "SVD" or m_PARAMDetectorType == "VXD") {
			StoreArray<SVDTrueHit> svdTrueHits; // carries all trueHits of event
			nSVDTrueHits = svdTrueHits.getEntries();
			B2DEBUG(1,"NonRootDataExportModule event " << m_eventCounter << ": executing " << nSVDTrueHits << " SVDTrueHits" )
			RelationIndex<MCParticle, SVDTrueHit> mcParticlesToSvdTrueHits; // allows us to find out which particle caused which truehit
			typedef RelationIndex<MCParticle, SVDTrueHit>::Element RelationElement;
			
			for( int i = 0; i < nSVDTrueHits; ++i ) {
				const SVDTrueHit* aTrueHit = svdTrueHits[i];
				
				BOOST_FOREACH(const RelationElement & rel, mcParticlesToSvdTrueHits.getElementsTo(aTrueHit)) {
					const MCParticle* particle = rel.from;
					particleID = particle->getIndex(); // 1-based index of Particle. If you want to have a 0-based index, use getArrayIndex() instead
					pdg = particle->getPDG();
					
					if ((particle->hasStatus(MCParticle::c_PrimaryParticle) == true) and (m_PARAMExportTrueHits == "real")) {
						isPrimary = 0;
						break; // if there are any other true or background particles, they shall be ignored
					}
				}
				B2DEBUG(10," SVDTrueHit with ID " << i << " is attached to mcParticles with ID " << particleID << " and hasStatusPrimary (0 = true) " << isPrimary)
				
				if ((m_PARAMExportTrueHits == "real" and isPrimary != 0) or (m_PARAMExportTrueHits == "background" and isPrimary != 1) or (m_PARAMExportTrueHits == "all" and isPrimary == -1)) {
					isPrimary = 1;
					particleID = -1;
					 B2DEBUG(10, " PXD hit " << i << " neglected")
					continue;
				}
				
				m_exportContainer.storeSVDTrueHit(aGeometry, aTrueHit, i+nPXDTrueHits, m_PARAMsmearTrueHits, isPrimary, particleID, pdg); // nPXDTrueHits is 0 if detectorType is SVD, therefore no check here
				isPrimary = 1;
				particleID = -1;
			} // looping over all svd trueHits
		} // storing svdTrueHits
	}
	
	
	// exporting GFTrackCands
	if (m_PARAMExportGFTCs == true ) {
		StoreArray<GFTrackCand> gftcs; // carries all trueHits of event
		StoreArray<PXDTrueHit> pxdTrueHits;
		StoreArray<SVDTrueHit> svdTrueHits;
		vector<const PXDTrueHit*> pxdHits;
		vector<const SVDTrueHit*> svdHits;
		vector<int> hitIDs; // pxd and svd can use the same vector since modified hitIDs are stored which are unique for each hit
		int nTCs = gftcs.getEntries(), detID, hitID, countedTCs = 0;
		B2DEBUG(1,"NonRootDataExportModule event " << m_eventCounter << ": executing " << nTCs << " GFTrackCands")
		
		for ( int i = 0; i < nTCs; ++i ) {
			hitIDs.clear(); pxdHits.clear(); svdHits.clear();
			GFTrackCand* aTC = gftcs[i];
			int nHits = aTC->getNHits();
			B2DEBUG(10,"event " << m_eventCounter << ": TC " << i << " has got " << nHits << " hits")
			
			if (int(aTC->getNHits()) < m_PARAMminTCLength ) {
				B2WARNING("NonRootDataExportModule - event " << m_eventCounter << ": GfTrackcand " << i << " has only " << nHits << " hits (threshold is " << m_PARAMminTCLength << "), neglecting tc...")
				continue;
			}
			
			for ( int j = 0; j < nHits; ++j ) {
				detID = 0;
				hitID = 0;
				aTC->getHit(j, detID, hitID); // sets detId and hitId for given hitIndex
				B2DEBUG(100, "----got Hitinfo. detID: " << detID << ", hitID: " << hitID)
				if (detID == Const::PXD and m_PARAMDetectorType != "SVD") { // pxd
					hitIDs.push_back(hitID);
					const PXDTrueHit* hit = pxdTrueHits[hitID];
					pxdHits.push_back(hit);
				} else if (detID == Const::SVD and m_PARAMDetectorType != "PXD") {
					hitIDs.push_back(hitID+nPXDTrueHits); // nPXDTrueHits is 0 if detectorType is SVD, therefore no check here
					const SVDTrueHit* hit = svdTrueHits[hitID];
					svdHits.push_back(hit);
				}
			}
			B2DEBUG(10,"storing GFTC " << i << " with " << pxdHits.size() << " pxd hits and " << svdHits.size() << " svd hits")
			m_exportContainer.storeGFTC(aGeometry, aTC, countedTCs, i, pxdHits, svdHits, hitIDs);
			++countedTCs;
			hitIDs.clear();
		}
	}

	B2DEBUG(1," event " << m_eventCounter << " got ID " << m_eventCounter << ", internal eventID of " << m_exportContainer.getCurrentEventNumber() << " and got internal hits: " << m_exportContainer.getNumberOfHits() << " hits, of these were " << m_exportContainer.getNumberOfPXDTrueHits() << " PXDTrueHits, and " << m_exportContainer.getNumberOfSVDTrueHits() << " were SVDTrueHits")
}



void NonRootDataExportModule::endRun()
{
	m_runCounter++;
	string fileName = "hits.data";
	m_exportContainer.exportAll(m_runCounter, 15.0); // second entry is the strength of the magnetic field
}



void NonRootDataExportModule::terminate()
{

}
