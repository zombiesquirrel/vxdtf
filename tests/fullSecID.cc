#include <framework/gearbox/Const.h>
#include <framework/logging/Logger.h>
#include <tracking/vxdCaTracking/FullSecID.h>
// #include <TMatrixF.h>
// #include <RKTrackRep.h>
#include <gtest/gtest.h>

using namespace std;

namespace Belle2 {
#define EXPECT_FATAL(x) EXPECT_EXIT(x,::testing::KilledBySignal(SIGABRT),"");

  /** Set up a few arrays and objects in the datastore */
  class FullSecIDTest : public ::testing::Test {
  protected:
  };

  /** Test simple Setters and Getters. */
  TEST_F(FullSecIDTest, simpleTest)
  {
		// first, we need a usefull vxdID:
		VxdID vxdID = VxdID(34624); // this should be a sensor on layer 3
		int vxdIDInt = vxdID;
		
		EXPECT_EQ(4, vxdID.getLayerNumber());
		
		// now we are using the constructor with a VxdID, a subLayerID and a sectorID
		bool subLayerID = true;
		unsigned short sectorID = 15;
    FullSecID aFullSecID = FullSecID(vxdID, subLayerID, sectorID);

    EXPECT_EQ(4, aFullSecID.getLayerID());

    EXPECT_EQ(subLayerID, aFullSecID.getSubLayerID());

    EXPECT_EQ(vxdID, aFullSecID.getVxdID());
		
		EXPECT_EQ(vxdIDInt, aFullSecID.getUniID());
		
		EXPECT_EQ(sectorID, aFullSecID.getSecID());
		
		// now we are using the second constructor using an encoded fullSecID as input:
		
		FullSecID anotherFullSecID = FullSecID(aFullSecID.getFullSecID());
		
		EXPECT_EQ(4, anotherFullSecID.getLayerID());

    EXPECT_EQ(subLayerID, anotherFullSecID.getSubLayerID());

    EXPECT_EQ(vxdID, anotherFullSecID.getVxdID());
		
		EXPECT_EQ(vxdIDInt, anotherFullSecID.getUniID());
		
		EXPECT_EQ(aFullSecID.getSecID(), anotherFullSecID.getSecID());
		
		EXPECT_EQ(aFullSecID.getFullSecID(), anotherFullSecID.getFullSecID());

  }
}  // namespace
