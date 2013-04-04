/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "../include/ExporterEventInfo.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace Belle2;


void ExporterEventInfo::addHit(ExporterHitInfo aHit) { m_hits.push_back(aHit); }

void ExporterEventInfo::addTc(ExporterTcInfo aTc) { m_tcs.push_back(aTc); }

void ExporterEventInfo::getHits(std::vector<ExporterHitInfo>* hits) { hits = &m_hits; }

void ExporterEventInfo::getTcs(std::vector<ExporterTcInfo>* tcs) { tcs = &m_tcs; }
