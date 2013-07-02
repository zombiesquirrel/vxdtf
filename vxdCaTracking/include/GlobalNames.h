/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/


#ifndef GLOBALNAMES_H
#define GLOBALNAMES_H

#include <string>



namespace Belle2 {
	namespace Tracking {
		/** bundles filter methods using 2 hits.  */
		class GlobalNames {
		public:

			/** Empty constructor */
			GlobalNames();

			std::string nameAngles3D = "angles3D"; /**< string name of filter a3D */
			std::string nameAnglesRZ = "anglesRZ"; /**< string name of filter aRZ */
			std::string nameAnglesXY = "anglesXY"; /**< string name of filter aXY */
			std::string nameDistance3D = "distance3D"; /**< string name of filter d3D */
			std::string nameDistanceXY = "distanceXY"; /**< string name of filter dXY */
			std::string nameDistanceZ = "distanceZ"; /**< string name of filter dZ */
			std::string nameHelixFit = "helixFit"; /**< string name of filter hFit */
			std::string nameSlopeRZ = "slopeRZ"; /**< string name of filter slopeRZ */
			std::string nameDeltaSlopeRZ = "deltaSlopeRZ"; /**< string name of filter dslopeRZ */
			std::string namePT = "pT"; /**< string name of filter pT */
			std::string nameDeltapT = "deltaPt"; /**< string name of filter dPt */
			std::string nameNormedDistance3D = "normedDistance3D"; /**< string name of filter nd3D */
			std::string nameDistance2IP = "dist2IP"; /**< string name of filter d2IP */
			std::string nameDeltaDistance2IP = "deltaDist2IP"; /**< string name of filter dd2IP */

			// HioC means high occupancy
			std::string nameAnglesHighOccupancy3D = "HioCangles3D"; /**< string name of filter a3D high occupancy */
			std::string nameAnglesHighOccupancyXY = "HioCanglesXY";  /**< string name of filter aXY high occupancy */
			std::string nameAnglesHighOccupancyRZ = "HioCanglesRZ"; /**< string name of filter aRZ high occupancy */
			std::string nameDistanceHighOccupancy2IP = "HioCdist2IP"; /**< string name of filter d2IP high occupancy */
			std::string nameDeltaSlopeHighOccupancyRZ = "HioCdeltaSlopeRZ"; /**< string name of filter dslopeRZ high occupancy */
			std::string namePTHighOccupancy = "HioCpT"; /**< string name of filter pT high occupancy */
			std::string nameHelixHighOccupancyFit = "HioChelixFit"; /**< string name of filter hFit high occupancy */
			std::string nameDeltapTHighOccupancy = "HioCdeltaPt"; /**< string name of filter dPt high occupancy */
			std::string nameDeltaDistanceHighOccupancy2IP = "HioCdeltaDist2IP"; /**< string name of filter dd2IP high occupancy */

		}; //end class GlobalNames
  } // end namespace Tracking
} //end namespace Belle2

#endif //GLOBALNAMES


