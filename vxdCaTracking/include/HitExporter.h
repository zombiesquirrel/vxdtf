/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/
#ifndef HitExporter_H
#define HitExporter_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include <vxd/geometry/GeoCache.h>
#include <vxd/geometry/SensorInfoBase.h>


namespace Belle2 {

	class PXDTrueHit;
	class PXDCluster;
	class SVDTrueHit;
	class SVDCluster;
	class ExporterEventInfo;
	
  /** Bundles information for all events to be stored by NonRootDataExportModule and writes that info into text files */
  class HitExporter {

  public:
		
		typedef std::map<int, ExporterEventInfo*> EventMap;
		typedef std::pair<int, ExporterEventInfo*> EventMapEntry;

    /** Empty constructor. */
    HitExporter():
					m_eventNumber(0) {}

    /** Destructor. */
    ~HitExporter();

//     /** initializes the internal storage for trueHits, accepts 'PXD' and 'SVD' as keys */
// 		void initializeTrueHitContainer(std::string detectorType);
// 		
// 		/** initializes the internal storage for clusters, accepts 'PXD' and 'SVD' as keys */
// 		void initializeClusterContainer(std::string detectorType);
// 		
// 		/** initializes the internal storage for gfTrackCands, expects the name of the gftc-container as key */
// 		void initializeGFContainer(std::string arrayName);
		/** a call per run expected: takes given sensorInfoBase and stores relevant information about it */
		void storeSensorInfo(const SensorInfoBase & 	aSensorInfo);
		
		/** event wise call expected: sets internal Event number needed for identifying hits */
		void prepareEvent(int n);
		
		int getCurrentEventNumber();
		
		int getNumberOfHits(); // for current Event
		
// 		/** event wise call expected: Expects pointer to the storearray to be stored and its name. It Stores its entries into track candidates. Returns string containing error message if something went wrong */
// 		std::string storeGFTrackCands(StoreArray<GFTrackCand>* pArray, std::string arrayName);
// 		
		/** event wise call expected: Expects pointer to the PXDTrueHit to be stored. It stores its entries to hits. Returns string containing error message if something went wrong */
		std::string storePXDTrueHit(VXD::GeoCache& geometry, PXDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost = -1, int particleID = -1, int pdg = -1); // default arguments are only allowed for .h-files, not in the .cc- version of the memberfunction
		
		/** event wise call expected: Expects pointer to the SVDTrueHit to be stored. It stores its entries to hits. Returns string containing error message if something went wrong */
		std::string storeSVDTrueHit(VXD::GeoCache& geometry, SVDTrueHit* aHit, int iD, int isPrimaryBackgroundOrGhost = -1, int particleID = -1, int pdg = -1);
		
// 		/** event wise call expected: Expects pointer to the storearray (StoreArray<PXDCluster>) to be stored. It stores its entries to hits. Returns string containing error message if something went wrong */
// 		std::string storePXDClusters(StoreArray<PXDCluster>* pArray);
// 		
// 		/** event wise call expected: Expects pointer to the storearray (StoreArray<SVDCluster>) to be stored. It stores its entries to hits. Returns string containing error message if something went wrong */
// 		std::string storeSVDClusters(StoreArray<SVDCluster>* pArray);
// 		
		/** expected to be called at the end of a run: exports every TC and every hit stored. the naming convention for standard output will be:
		 * run[X]_event[X]_hits for hits and rX_eX_tcs for track candidates
		 * individual output will be event[X]_[name]
		 */
		std::string exportAll(int n, std::string name = "");
  protected:
		/** feeded by storePXDTrueHit or storeSVDTrueHit and does all the calculations and storing */
		std::string storeTrueHit(VXD::SensorInfoBase aSensorInfo, double u, double v, int type, int iD, int isPrimaryBackgroundOrGhost, int aLayerID, int particleID, int pID);
// 		void exprt(std::stringstream );

    int m_eventNumber; /**< stores current event number */
		EventMap m_storedOutput; /**< stores everything which has to be stored */
		ExporterEventInfo* m_thisEvent; /**< pointer for current event for easier code */
		std::string m_exportFileName; /**< name used  for exporting data */
// 		VXD::GeoCache& m_geometry; /**< stores info about geometry */
    
  }; //end class HitExporter
} //end namespace Belle2

#endif //HitExporter


