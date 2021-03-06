/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/


#ifndef ExporterHitInfo_H
#define ExporterHitInfo_H

#include <vector>
#include <string>
#include <TVector3.h>


namespace Belle2 {

  /** Bundles information for a single hit to be stored by EventInfo (needed for HitExporter, which is needed by NonRootDataExportModule) */
	class ExporterHitInfo {
	public:
		/** Standard constructor. */
		ExporterHitInfo(TVector3 globalHitPosition, TVector3 covValues, int layerID, double angleOfSensor, int hitID, int hitType, int classification = -1, int particleID = -1, int pdg = -1):
					m_hitPos(globalHitPosition),
					m_covVal(covValues),
					m_layerID(layerID-1), // they start at layer 0!
					m_sensorAngle(angleOfSensor),
					m_hitID(hitID),
					m_hitType(hitType),
					m_classification(classification),
					m_particleID(particleID),
					m_pdg(pdg) {}
		
		
		/** Destructor. */
    ~ExporterHitInfo() {}
		
		
		/** Output of position in a string file: it is a line with Xpos[empty space]Ypos[empty space]Zpos. */
		std::string getPositionFormatted();
		
		
		/** Output of covValues in a string file: it is a line with CovUU[empty space]CovUV[empty space]CovVV */
		std::string getCovValuesFormatted();
		
		
		/** Output of additional information in a string file: it is a line with layerID[empty space]hitID[empty space]sensorAngle */
		std::string getAdditionalInfoFormatted();
		
		
		/** returns type of hit. (0=PXDTrueHit, 1=SVDTrueHit), (2=PXDCluster, 3=SVDCluster) */
		int getType() { return m_hitType; }
		
		
		/**< returns modified index number of truehit attahed to this hit (means that the iD is unique, so pxd and svd can be stored without overlap)*/
		int getHitID() { return m_hitID; }
		
		
	protected:
		
		TVector3 m_hitPos; /**< global coordinates of hit */
		TVector3 m_covVal; /**< carries relevant values of the covariance matrix for the hit. entries are (CovUU, CovUV, CoVV) */
		int m_layerID; /**< layerID, for PXD, this is normally 1 or 2, for SVD its typically 3-6 */
		double m_sensorAngle; /**< the angle between the orthogonal vector of the sensor plane of the hit and the x-axis. It is measured clockwise 0-2pi */
		int m_hitID; /**< an event wise unique ID for the hit, which is a modified index number of truehit attahed to this hit  (means that the iD is unique, so pxd and svd can be stored without overlap)*/
// 		int m_2ndHitID; /**< dummy value, eventually needed later for SVDClusters */
		int m_hitType; /**< defines whether it is a truehit (0=PXDTrueHit, 1=SVDTrueHit) or a cluster (2=PXDCluster, 3=SVDCluster) */
		int m_classification; /**< defines whether the particle is primary = 0, secondary=1 (background) or ghost=2 (only for SVDClusters), if unknown, it is -1 */
		int m_particleID; /**< attaches the hit to a particle for recognition. It's -1 if particle ID is unknown */
		int m_pdg; /**< pdgCode of particle causing hit */
	}; //end class ExporterHitInfo
} //end namespace Belle2

#endif //ExporterHitInfo
