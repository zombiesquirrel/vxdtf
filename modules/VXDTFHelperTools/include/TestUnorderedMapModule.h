/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2011 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler, Moritz Nadler                       *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/
/* Additional Info:
* This Module is in an early stage of developement. The comments are mainly for temporal purposes
* and will be changed and corrected in later stages of developement. So please ignore them.
*/

#ifndef TestUnorderedMapModule_H_
#define TestUnorderedMapModule_H_

#include <framework/core/Module.h>
#include <tracking/vxdCaTracking/VXDSector.h>
// #include <map>
#include <vector>
#ifndef __CINT__
#include <boost/chrono.hpp>
#include <boost/unordered_map.hpp>
#endif


namespace Belle2 {
	namespace Tracking {

		/** The TestUnorderedMapModule
		*
		* this module simply counts the number of events (and prints every x-th event onto the screen, where x is user-defined). Useful when executing modules which do not provide this feature themselves
		*
		*/
		class TestUnorderedMapModule : public Module {

		public:
			typedef boost::chrono::high_resolution_clock boostClock; /**< used for measuring time comsumption */ // high_resolution_clock, process_cpu_clock
//       typedef boost::chrono::microseconds boostNsec; /**< defines time resolution (currently mictroseconds) */ // microseconds, milliseconds
      typedef boost::chrono::nanoseconds boostNsec; /**< defines time resolution (currently nanoseconds) */ // microseconds, milliseconds
			typedef boost::unordered_map<unsigned int, VXDSector> MapOfSectors;

			/**
			* Constructor of the module.
			*/
			TestUnorderedMapModule();

			/** Destructor of the module. */
			virtual ~TestUnorderedMapModule();

			/** Initializes the Module.
			*/
			virtual void initialize();

			/**
			* Prints a header for each new run.
			*/
			virtual void beginRun();

			/** Prints the full information about the event, run and experiment number. */
			virtual void event();

			/**
			* Prints a footer for each run which ended.
			*/
			virtual void endRun();

			/**
			* Termination action.
			*/
			virtual void terminate();
			
			/** Fills Map with test entries */
			void FillMap();
			
			/** generates a vector filled with randomly generated sector addresses */
			void FillVector(std::vector<unsigned int>& vec);
			
			/** jumbles/randomizes the entries of given vector */
			void JumbleVector(std::vector<unsigned int>& vec);

		protected:

			boostNsec m_eventStuff; /**< time consumption of the secMap-access-step (events) */
      boostNsec m_fillStuff; /**< time consumption of the secMap-creation (initialize) */
			int m_eventCounter; /**< knows current event number */
			int m_PARAMnumLayers; /**< sets how many layers are assumed (useful value: 4-6) */
			int m_PARAMnumSensors; /**< sets how many sensors per layer are assumed (useful value: ~227/numLayers) */
			int m_PARAMnumSectors;  /**< sets how many sectors per sensor are assumed (useful value: 4-50) */
			int m_PARAMnumFriends; /**< sets how many friends per sector are assumed (useful value: 2-20) */
			MapOfSectors m_testMap; /**< .first, key coded with FullSecID, .second, VXDSector filled with some friends */
			std::vector<unsigned int> m_sectorAdresses; /**< contains all keys of the sectorMap */

		private:

		};
	}
}

#endif /* TestUnorderedMapModule_H_ */
