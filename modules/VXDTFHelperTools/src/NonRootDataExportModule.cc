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
#include <vxd/geometry/GeoCache.h>
#include <framework/dataobjects/EventMetaData.h>
#include <framework/datastore/StoreObjPtr.h>
#include <framework/datastore/StoreArray.h>
#include <generators/dataobjects/MCParticle.h>
#include <framework/datastore/RelationIndex.h>
#include <pxd/dataobjects/PXDCluster.h>
#include <svd/dataobjects/SVDCluster.h>
#include <pxd/dataobjects/PXDTrueHit.h>
#include <svd/dataobjects/SVDTrueHit.h>

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

// 	vector<string> paramGfTcArrays;
//   addParam("gfTcArrays", m_PARAMGfTcArrays, "insert a list of names for gfTrackCand-arrays if an array with no name shall be exported, please insert an empty string for it. Do not export gfTrackCands is standard", paramGfTcArrays);
	addParam("exportTrueHits", m_PARAMExportTrueHits, "allows you to export true hits. Please choose between 'real' hits, 'background' hits or 'none', which is standard. Wrong input values will set to none with an error.", string("none"));
// 	addParam("exportClusters", m_PARAMExportClusters, "allows you to export clusters (where SVD clusters will automatically grouped to 2D clusters). Please choose between 'real' hits, 'background' hits (which includes ghost hits) or 'none', which is standard. Wrong input values will set to none with an error.", ("none").str());
	addParam("detectorType", m_PARAMDetectorType, "set detectorype. Please choose between 'PXD', 'SVD' (standard) or 'VXD'. Wrong input values will set to SVD with an error.", string("SVD"));
}


NonRootDataExportModule::~NonRootDataExportModule()
{

}


void NonRootDataExportModule::initialize()
{
	
	B2INFO("NonRootDataExportModule::initialize() \n checking and setting initial parameters")
// 	stringstream chosenGFTCarrays;
// 	BOOST_FOREACH(string entry, m_PARAMGfTcArrays { // gfTcArrays
// 		StoreArray<GFTrackCand>::required(entry);
// 		chosenGFTCarrays << entry << " ";
// 	}
// 	int ngftcArrays = m_PARAMGfTcArrays.size();
// 	if ( ngftcArrays != 0 ) {
// 		std::sort(m_PARAMGfTcArrays.begin(, m_PARAMGfTcArrays.end()));
// 		std::unique(m_PARAMGfTcArrays.begin(, m_PARAMGfTcArrays.end()));
// 		ngftcArrays = m_PARAMGfTcArrays.size();
// 		BOOST_FOREACH(string entry, m_PARAMGfTcArrays) {
// 			m_exportContainer.initializeGFContainer(entry);
// 		}
// 	}
	
	if (m_PARAMDetectorType != "PXD" and m_PARAMDetectorType != "SVD" and m_PARAMDetectorType != "VXD" ) { // detectorType
		B2ERROR(" chosen value '" << m_PARAMDetectorType << "' for parameter detectorType is unknown, setting to 'SVD'")
		m_PARAMDetectorType = "SVD";
	}
	
	if (m_PARAMExportTrueHits == "real" or m_PARAMExportTrueHits == "background") { // exportTrueHits
		if (m_PARAMDetectorType == "PXD" or "VXD") {
			StoreArray<PXDTrueHit>::required();
// 			m_exportContainer.initializeTrueHitContainer("PXD");
		}
		if (m_PARAMDetectorType == "SVD" or "VXD") {
			StoreArray<SVDTrueHit>::required();
// 			m_exportContainer.initializeTrueHitContainer("SVD");
		}
	} else {
		B2ERROR(" chosen value '" << m_PARAMExportTrueHits << "' for parameter exportTrueHits is unknown, setting to 'none'")
		m_PARAMExportTrueHits = "none";
	}
	
// 	if (m_PARAMExportClusters == "real" or m_PARAMExportClusters == "background") { // exportClusters
// 		if (m_PARAMDetectorType == "PXD" or "VXD") {
// 			StoreArray<PXDCluster>::required();
// // 			m_exportContainer.initializeClusterContainer("PXD");
// 		}
// 		if (m_PARAMDetectorType == "SVD" or "VXD") {
// 			StoreArray<SVDCluster>::required();
// // 			m_exportContainer.initializeClusterContainer("SVD");
// 		}
// 	} else {
// 		B2ERROR(" chosen value '" << m_PARAMExportClusters << "' for parameter exportClusters is unknown, setting to 'none'")
// 		m_PARAMExportClusters = "none";
// 	}

	StoreArray<MCParticle>::required();
	
	// TODO import layerInfos for settings.data (bZ, layer: R X/X0, X*Dichte(Si))
	
// 	B2INFO("NonRootDataExportModule: chosen detectorType: " << m_PARAMDetectorType << ", exporting TrueHits: " << m_PARAMExportTrueHits << ", exporting clusters: " << m_PARAMExportClusters << " and " << ngftcArrays << " GFTrackCand-Arrays with following names: " << str(chosenGFTCarrays) << ", total containers to store: " << m_exportContainer.initializedContainers())
	B2INFO("NonRootDataExportModule: chosen detectorType: " << m_PARAMDetectorType << " and exporting TrueHits: " << m_PARAMExportTrueHits)
}


void NonRootDataExportModule::beginRun()
{
	m_eventCounter = 0;
	m_runCounter = 0;
}


void NonRootDataExportModule::event()
{
	StoreObjPtr<EventMetaData> eventMetaDataPtr("EventMetaData", DataStore::c_Event);
	m_eventCounter = eventMetaDataPtr->getEvent();
	m_exportContainer.prepareEvent(m_eventCounter);
	
	StoreArray<MCParticle> mcParticles;
	VXD::GeoCache& aGeometry = VXD::GeoCache::getInstance();
	
	
// 	BOOST_FOREACH(string entry, m_PARAMGfTcArrays) {
// 		StoreArray<GFTrackCand> = gfTcs(entry);
// 		m_exportContainer.storeGFTrackCands(gfTcs*, entry);
// 	}
	
	
	// looping over all mcparticles connected to each TrueHit, if there is at least one primary particle connected to current trueHit, the first found primary particle determines the state of the trueHit (means, if there is a primary particle attached to the trueHit, the trueHit is recognized as realhit, and recognized as background hit if otherwise, the particleID of the hit will be the first primary particle or the last secondary one attached to current trueHit):
	if (m_PARAMExportTrueHits != "none") { // storing trueHits
		int nPXDTrueHits = 0, nSVDTrueHits = 0, particleID = -1;
		int isPrimary = 1;
// 		vector<MCParticle*> particlesOfTrueHit; // reused for every TrueHit
		if (m_PARAMDetectorType == "PXD" or "VXD") { // storing pxd truehits
			StoreArray<PXDTrueHit> pxdTrueHits; // carries all trueHits of event
			nPXDTrueHits = pxdTrueHits.getEntries();
			B2DEBUG(1,"NonRootDataExportModule event " << m_eventCounter << ": executing " << nPXDTrueHits << " PXDTrueHits")
			RelationIndex<MCParticle, PXDTrueHit> mcParticlesToPxdTrueHits; // allows us to find out which particle caused which truehit
			typedef RelationIndex<MCParticle, PXDTrueHit>::Element RelationElement;
			
			for(int i = 0; i < nPXDTrueHits; ++i) {
				const PXDTrueHit* aTrueHit = pxdTrueHits[i];
				
				BOOST_FOREACH(const RelationElement & rel, mcParticlesToPxdTrueHits.getElementsTo(aTrueHit)) {

					const MCParticle* particle = rel.from;
					particleID = particle->getIndex(); // 1-based index of Particle. If you want to have a 0-based index, use getArrayIndex() instead
					
					if ((particle->hasStatus(MCParticle::c_PrimaryParticle) == true) and (m_PARAMExportTrueHits == "real")) {
						isPrimary = 0;
						break; // if there are any other true or background particles, they shall be ignored
					}
				}
				B2DEBUG(10," PXDTrueHit with ID " << i << " is attached to mcParticles with ID " << particleID << " and hasStatusPrimary (0 = true) " << isPrimary)
				if ((m_PARAMExportTrueHits == "real" and isPrimary == 0) or (m_PARAMExportTrueHits == "background" and isPrimary == 1) ) {
					m_exportContainer.storePXDTrueHit(aGeometry, pxdTrueHits[i], i, isPrimary, particleID);
				}
				isPrimary = 1;
				particleID = -1;
			} // looping over all pxd trueHits
		} // storing pxdTrueHits
		
		if (m_PARAMDetectorType == "SVD" or "VXD") {
			StoreArray<SVDTrueHit> svdTrueHits; // carries all trueHits of event
			nSVDTrueHits = svdTrueHits.getEntries();
			B2DEBUG(1,"NonRootDataExportModule event " << m_eventCounter << ": executing " << nSVDTrueHits << " SVTrueHits" )
			RelationIndex<MCParticle, SVDTrueHit> mcParticlesToSvdTrueHits; // allows us to find out which particle caused which truehit
			typedef RelationIndex<MCParticle, SVDTrueHit>::Element RelationElement;
			
			for(int i = 0; i < nSVDTrueHits; ++i) {
				const SVDTrueHit* aTrueHit = svdTrueHits[i];
				
				BOOST_FOREACH(const RelationElement & rel, mcParticlesToSvdTrueHits.getElementsTo(aTrueHit)) {
// 					if(!rel) {
// 						B2WARNING("no MCParticle found for SVDTrueHit " << i);
// 						continue;
// 					}
					const MCParticle* particle = rel.from;
					particleID = particle->getIndex(); // 1-based index of Particle. If you want to have a 0-based index, use getArrayIndex() instead
					if ((particle->hasStatus(MCParticle::c_PrimaryParticle) == true) and (m_PARAMExportTrueHits == "real")) {
						isPrimary = 0;
						break; // if there are any other true or background particles, they shall be ignored
					}
				}
				B2DEBUG(10," SVDTrueHit with ID " << i << " is attached to mcParticles with ID " << particleID << " and hasStatusPrimary (0 = true) " << isPrimary)
				if ((m_PARAMExportTrueHits == "real" and isPrimary == 0) or (m_PARAMExportTrueHits == "background" and isPrimary == 1) ) {
					m_exportContainer.storeSVDTrueHit(aGeometry, svdTrueHits[i], i+nPXDTrueHits, isPrimary, particleID);
				}
				isPrimary = 1;
				particleID = -1;
			} // looping over all svd trueHits
		} // storing svdTrueHits
	}
	
	B2DEBUG(1," event " << m_eventCounter << " got ID " << m_eventCounter << ", internal eventID of " << m_exportContainer.getCurrentEventNumber())
	B2DEBUG(1," event " << m_eventCounter << " got ID " << m_eventCounter << ", internal eventID of " << m_exportContainer.getCurrentEventNumber() << " and got /*" << m_exportContainer.getNumberOfHits() << "*/ hits")
	
	
// 	if (m_PARAMExportClusters != "none") {
// 		/// WARNING not working yet! Problems to solve: how to group Clusters to hits again?
// 		if (m_PARAMDetectorType == "PXD" or "VXD") {
// 			StoreArray<PXDCluster> pxdClusters;
// 			m_exportContainer.storePXDClusters(pxdClusters*);
// 		}
// 		if (m_PARAMDetectorType == "SVD" or "VXD") {
// 			StoreArray<SVDCluster> svdClusters;
// 			m_exportContainer.storeSVDClusters(svdClusters*);
// 		}
// 	}
//

}


void NonRootDataExportModule::endRun()
{
	m_runCounter++;
	string fileName = "hits.data";
	m_exportContainer.exportAll(m_runCounter, fileName);
}


void NonRootDataExportModule::terminate()
{

}
