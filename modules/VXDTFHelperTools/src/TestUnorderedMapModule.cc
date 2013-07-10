/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2011 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler, Moritz Nadler                       *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "tracking/modules/VXDTFHelperTools/TestUnorderedMapModule.h"
#include "tracking/vxdCaTracking/LittleHelper.h"
#include "tracking/vxdCaTracking/FullSecID.h"
#include <framework/logging/Logger.h>
#include <boost/foreach.hpp>


using namespace std;
using namespace Belle2;
using namespace Tracking;

//-----------------------------------------------------------------
//                 Register the Module
//-----------------------------------------------------------------
REG_MODULE(TestUnorderedMap)

//-----------------------------------------------------------------
//                 Implementation
//-----------------------------------------------------------------

TestUnorderedMapModule::TestUnorderedMapModule() : Module()
{
  //Set module properties
  setDescription("tests boost::unordered_map for sectorMaps");
  setPropertyFlags(c_ParallelProcessingCertified | c_InitializeInProcess);

  addParam("numLayers", m_PARAMnumLayers, "sets how many layers are assumed (useful value: 4-6)", int(6));
	addParam("numSensors", m_PARAMnumSensors, "sets how many sensors per layer are assumed (useful value: ~227/numLayers)", int(38));
	addParam("numSectors", m_PARAMnumSectors, "sets how many sectors per sensor are assumed (useful value: 4-50)", int(15));
	addParam("numFriends", m_PARAMnumFriends, "sets how many friends per sector are assumed (useful value: 2-20)", int(3));
}


TestUnorderedMapModule::~TestUnorderedMapModule()
{

}


void TestUnorderedMapModule::initialize()
{
	m_sectorAdresses.clear();
	FillVector(m_sectorAdresses);
	JumbleVector(m_sectorAdresses);
	FillMap();
}


void TestUnorderedMapModule::beginRun()
{
  m_eventCounter = 0;
}


void TestUnorderedMapModule::event()
{
  m_eventCounter++;
  if (m_eventCounter % 500 == 0) {
    B2INFO("TestUnorderedMapModule - Event: " << m_eventCounter);
  }
  LittleHelper littleHelperBox = LittleHelper();
	
	int rndAccess = littleHelperBox.getRandomIntegerGauss(50,10);
	int numSectors = m_sectorAdresses.size();
	if (rndAccess < 0 ) { rndAccess = -rndAccess; }
	int rndSector, aFriend;
	vector<int> accessVariables;
	for (int i = 0; i < rndAccess; ++i ) {
		rndSector = littleHelperBox.getRandomIntegerUniform(0, numSectors);
		accessVariables.push_back(rndSector);
	}
	
	boostClock::time_point beginEvent = boostClock::now();
	BOOST_FOREACH(int sectorValue, accessVariables) {
		const vector<unsigned int>& friends = m_testMap.find(m_sectorAdresses[sectorValue])->second.getFriends();
		// 		aFriend = friends[0];
		// 		aFriend++;
	}
	boostClock::time_point stopTimer = boostClock::now();
	m_eventStuff += boost::chrono::duration_cast<boostNsec>(stopTimer - beginEvent);
}


void TestUnorderedMapModule::endRun()
{
}


void TestUnorderedMapModule::terminate()
{

	B2WARNING("boost::unordered_map-Filling took " << (m_eventStuff.count()) << " nanoseconds")
  B2WARNING("boost::unordered_map-Reading took " << (m_fillStuff.count() ) << " nanoseconds")
}

void TestUnorderedMapModule::FillMap() {
	LittleHelper littleHelperBox = LittleHelper();
	
	int numSectors = m_sectorAdresses.size();
	vector<VXDSector> sectors;
	BOOST_FOREACH(unsigned int fullSecID, m_sectorAdresses) {
		VXDSector aSector = VXDSector(fullSecID);
		for (int i = 0; i < m_PARAMnumFriends; ++i) {
			aSector.addFriend(littleHelperBox.getRandomIntegerUniform(0, numSectors));
		}
		sectors.push_back(aSector);
	} // is done in a separate loop to get better time resolution of the fillin-process
	
	vector<VXDSector>::iterator secIt = sectors.begin();
	boostClock::time_point beginEvent = boostClock::now();
	for (; secIt < sectors.end(); ++secIt) {
		m_testMap.insert(make_pair(secIt->getSecID(), (*secIt)));
	}
	boostClock::time_point stopTimer = boostClock::now();
	m_fillStuff += boost::chrono::duration_cast<boostNsec>(stopTimer - beginEvent);

	B2INFO("TestFlatMapModule::FillMap: map has " << m_testMap.size() << " entries...")
}

void TestUnorderedMapModule::FillVector(vector<unsigned int>& vec) {
	for (int lay = 0; lay < m_PARAMnumLayers; ++lay) {
		for (int slay = 0; slay < 2; ++slay) {
			for (int sen = 0; sen < m_PARAMnumSensors/2; ++sen) {
				for (int sec = 0; sec < m_PARAMnumSectors; ++sec) {
					vec.push_back(FullSecID(lay, bool(slay), sen, sec).getFullSecID());
				}
			}
		}
	}
	B2INFO("TestUnorderedMapModule::FillVector: vector has " << vec.size() << " entries...")
}

void TestUnorderedMapModule::JumbleVector(vector<unsigned int>& vec) {
	LittleHelper littleHelperBox = LittleHelper();
	int index1, index2, entry, length = vec.size();
	for (int i = 0; i < length; i++)
	{
		index1 = littleHelperBox.getRandomIntegerUniform(0, length);
		index2 = littleHelperBox.getRandomIntegerUniform(0, length);
		entry = vec[index1];
		vec[index1] = vec[index2];
		vec[index2] = entry;
	}
}