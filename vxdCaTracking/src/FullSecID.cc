/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "../include/FullSecID.h"
#include <assert.h>

using namespace std;
using namespace Belle2;

const int FullSecID::LayerBits = 4;
const int FullSecID::SubLayerBits = 1;
const int FullSecID::VxdIDBits = 16;
const int FullSecID::SectorBits = 11;
const int FullSecID::Bits = LayerBits + SubLayerBits + VxdIDBits + SectorBits;
const int FullSecID::MaxLayer = (1 << LayerBits) - 1;
const int FullSecID::MaxSubLayer = (1 << SubLayerBits) - 1;
const int FullSecID::MaxVxdID = (1 << VxdIDBits) - 1;
const int FullSecID::MaxSector = (1 << SectorBits) - 1;
const int FullSecID::MaxID = (1 << Bits) - 1;
const int FullSecID::LayerBitShift   = SubLayerBits + VxdIDBits + SectorBits;
const int FullSecID::SubLayerBitShift   = VxdIDBits + SectorBits;
const int FullSecID::VxdIDBitShift  = SectorBits;
const int FullSecID::SubLayerMask = MaxSubLayer << SubLayerBitShift;
const int FullSecID::VxdIDMask = MaxVxdID << SubLayerBitShift;
const int FullSecID::SectorMask = MaxSector;


FullSecID::FullSecID(VxdID vxdID, bool subLayerID, unsigned int sectorNumber):
					m_fullSecID(0) { // setting to 0 to be shure that value is 0 when using it within the brackets
	unsigned int LayerID = vxdID.getLayerNumber();
	unsigned int SubLayerID = subLayerID; // converting to int
	unsigned int UniID = vxdID;
	
	assert (LayerID > MaxLayer);
	assert (SubLayerID > MaxSubLayer);
	assert (UniID > MaxVxdID);
	assert (sectorNumber > MaxSector);

	LayerID <<= LayerBitShift;
	SubLayerID <<= SubLayerBitShift;
	UniID <<= VxdIDBitShift;
	m_fullSecID = LayerID | SubLayerID | UniID | sectorNumber; // should be the same as below
// 	m_fullSecID = LayerID + SubLayerID + UniID + sectorNumber; // should be the same as above
}

short int FullSecID::getLayerID() {
	return m_fullSecID >> LayerBitShift;
}

bool FullSecID::getSubLayerID() {
	return m_fullSecID bitand SubLayerMask;
}

VxdID FullSecID::getVxdID() {
	return m_fullSecID bitand VxdIDMask;
}

short int FullSecID::getUniID() {
	return m_fullSecID bitand SectorMask;
}

int FullSecID::getFullSecID() {
	return m_fullSecID;
}