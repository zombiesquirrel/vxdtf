/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/
#ifndef ExporterEventInfo_H
#define ExporterEventInfo_H

#include <vector>

#include "../include/ExporterHitInfo.h"
#include "../include/ExporterTcInfo.h"


namespace Belle2 {
	
  /** Bundles information for a single event to be stored by NonRootDataExportModule */
  class ExporterEventInfo {

  public:

    /** Empty constructor. */
    ExporterEventInfo(int evtNum):
					m_eventNumber(evtNum) {}

    /** Destructor. */
    ~ExporterEventInfo() {}

    /** adds Hit to internal storage.  */
		void addHit(ExporterHitInfo aHit);
		
		/** adds Tc to internal storage.  */
		void addTc(ExporterTcInfo aTc);
		
		/** returns a pointer to internal vector of hits */
		void getHits(std::vector<ExporterHitInfo>* hits);
		
		/** returns pointer to fully formatted string */
// 		std::string* getHitsFormatted();
		
		int getEventNumber() { return m_eventNumber; }
		
		/** returns a pointer to internal vector of tcs */
		void getTcs(std::vector<ExporterTcInfo>* tcs);
		

  protected:
		int m_eventNumber; /**< knows number of current event */
		std::vector<ExporterHitInfo> m_hits; /**< stores all hits of event */
		std::vector<ExporterTcInfo> m_tcs; /**< stores all tcs of event */
		int m_nPXDTrueHits;
		int m_nSVDTrueHits;
		int m_nPXDClusters;
		int m_nSVDClusters;
  }; //end class EventInfo
} //end namespace Belle2

#endif //ExporterEventInfo

