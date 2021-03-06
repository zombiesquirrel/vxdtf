/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/

#include "../include/TwoHitFilters.h"
#include <boost/math/special_functions/fpclassify.hpp>
#include <cmath>


using namespace std;
using namespace Belle2;
using namespace Belle2::Tracking;


double TwoHitFilters::filterNan(double value)
{
  if (std::isnan(value) == true) { return 0; }
  return value;
}


double TwoHitFilters::fullSlopeRZ()
{
  double slope = sqrt(m_x2 + m_y2) / m_dz;
  return filterNan(slope);
}