/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/


#ifndef THREEHITFILTERS_H
#define THREEHITFILTERS_H

#include <TVector3.h>
#include "TwoHitFilters.h"



namespace Belle2 {

  /** The class 'ThreeHitFilters' bundles filter methods using 3 hits which are stored in TVector3s. */
  class ThreeHitFilters {
  public:

    /** Empty constructor. For initialisation only, an object generated this way is useless unless resetValues(a, b) is called at least once */
    ThreeHitFilters():
      m_circleCenterCalculated(false),
      m_radiusCalculated(false),
      m_radius(0.),
      m_x2(0.),
      m_y2(0.),
      m_z2(0.) {
      m_hitA.SetXYZ(0., 0., 0.);
      m_hitB.SetXYZ(0., 0., 0.);
      m_hitC.SetXYZ(0., 0., 0.);
      m_vecAB.SetXYZ(0., 0., 0.);
      m_vecBC.SetXYZ(0., 0., 0.);
    }

    /** Constructor. needs the first parameter is outer hit, second is center hit, third is inner hit. Parameters in TVector3-format*/
    ThreeHitFilters(TVector3& outerHit, TVector3& centerHit, TVector3& innerHit):
      m_circleCenterCalculated(false),
      m_radiusCalculated(false),
      m_radius(0.),
      m_hitA(outerHit),
      m_hitB(centerHit),
      m_hitC(innerHit),
      m_vecAB(outerHit - centerHit),
      m_vecBC(centerHit - innerHit) {
      m_x2 = m_vecAB[0] * m_vecBC[0]; /// x-part of m_vecAB.Dot(m_vecBC)
      m_y2 = m_vecAB[1] * m_vecBC[1]; /// y-part of m_vecAB.Dot(m_vecBC)
      m_z2 = m_vecAB[2] * m_vecBC[2]; /// z-part of m_vecAB.Dot(m_vecBC)
    }


    /** Destructor. */
    ~ThreeHitFilters() {}

    /** Overrides Constructor-Setup. Needed if you want to reuse the instance instead of recreating one */
    void resetValues(TVector3& outerHit, TVector3& centerHit, TVector3& innerHit) {
      m_radiusCalculated = false;
      m_circleCenterCalculated = false;
      m_radius = 0.;
      m_hitA = outerHit;
      m_hitB = centerHit;
      m_hitC = innerHit;
      m_vecAB = outerHit - centerHit;
      m_vecBC = centerHit - innerHit;

      m_x2 = m_vecAB[0] * m_vecBC[0]; /// x-part of m_vecAB.Dot(m_vecBC)
      m_y2 = m_vecAB[1] * m_vecBC[1]; /// y-part of m_vecAB.Dot(m_vecBC)
      m_z2 = m_vecAB[2] * m_vecBC[2]; /// z-part of m_vecAB.Dot(m_vecBC)
    }

    /** calculates the angle between the hits/vectors (3D), returning unit: none (calculation for degrees is incomplete, if you want readable numbers, use fullAngle3D instead) */
    double calcAngle3D() {
			double angle = ((m_x2 + m_y2 + m_z2) / (m_vecAB.Mag2() * m_vecBC.Mag2())); // fullCalc would be acos(m_vecAB.Dot(m_vecBC) / m_vecAB.Mag()*m_vecBC.Mag())
			return m_twoHitFilter.filterNan(angle);
		} // return unit: none (calculation for degrees is incomplete, if you want readable numbers, use fullAngle3D instead)


    /** calculates the angle between the hits/vectors (3D), returning unit: angle in radians */
    double fullAngle3D();

    /** calculates the angle between the hits/vectors (XY), returning unit: none (calculation for degrees is incomplete, if you want readable numbers, use fullAngleXY instead) */
    double calcAngleXY() {
			double angle = ((m_x2 + m_y2) / (m_vecAB.Perp2() * m_vecBC.Perp2())); // fullAngle:
			return m_twoHitFilter.filterNan(angle);
		} // return unit: none (calculation for degrees is incomplete, if you want readable numbers, use fullAngleXY instead)


    /** calculates the angle between the hits/vectors (XY), returning unit: angle in radians */
    double fullAngleXY();

    /** calculates the angle between the hits/vectors (RZ), returning unit: none (calculation for degrees is incomplete, if you want readable numbers, use fullAngleRZ instead) */
    double calcAngleRZ() {
			TVector3 rzVecAB(m_vecAB.Perp(), m_vecAB[2], 0.);
			TVector3 rzVecBC(m_vecBC.Perp(), m_vecBC[2], 0.);
			return calcAngle2D(rzVecAB, rzVecBC);
		} // return unit: none (calculation for degrees is incomplete, if you want readable numbers, use fullAngleRZ instead)


    /** calculates the angle between the hits/vectors (RZ), returning unit: angle in radians */
    double fullAngleRZ();

    /** calculates the distance of the point of closest approach of circle to the IP, returning unit: cm */
    double calcCircleDist2IP();

    /** calculates the estimation of the transverse momentum of the 3-hit-tracklet, returning unit: GeV/c */
    double calcPt() {
			if (m_circleCenterCalculated == false) {
				calcCircleCenter(m_hitA, m_hitB, m_hitC, m_centerABC);
				m_circleCenterCalculated = true;
			}
			if (m_radiusCalculated == false) {
				m_radius = calcRadius(m_hitA, m_hitB, m_hitC, m_centerABC);
				m_radiusCalculated = true;
			}
			return (0.00449565 * m_radius); // pT[GeV/c] = 0.299710*B[T]*r[m] = 0.449565*r[cm]/100 = 0.00449565*r[cm]
		} // return unit: GeV/c

    /** calculates deviations in the slope of the inner segment and the outer segment, returning unit: none */
    double calcDeltaSlopeRZ() {
			m_twoHitFilter.resetValues(m_hitA, m_hitB);
			double slopeAB = m_twoHitFilter.calcSlopeRZ();
			m_twoHitFilter.resetValues(m_hitB, m_hitC);
			double slopeBC = m_twoHitFilter.calcSlopeRZ();

			return m_twoHitFilter.filterNan(slopeBC / slopeAB); // value should be near 1
		} // return unit: none

    /** calculates the helixparameter describing the deviation in z per unit angle, returning unit: none */
    double calcHelixFit() {
			if (m_circleCenterCalculated == false) { calcCircleCenter(m_hitA, m_hitB, m_hitC, m_centerABC); }
			TVector3 points2hitA = m_hitA - m_centerABC;
			TVector3 points2hitB = m_hitB - m_centerABC;
			TVector3 points2hitC = m_hitC - m_centerABC;
			double alfaAB = calcAngle2D(points2hitA, points2hitB);
			double alfaBC = calcAngle2D(points2hitB, points2hitC);
			// real calculation: ratio is (m_vecij[2] = deltaZ): alfaAB/deltaZab : alfaBC/deltaZbc, the following equation saves two times '/'
			return m_twoHitFilter.filterNan((alfaAB * m_vecBC[2]) / (alfaBC * m_vecAB[2]));
		} // return unit: none

    /** calculates the angle between the hits/vectors (2D), generalized, returning unit: none. used by calcAngleRZ and calcHelixFit (angleXY could use it too, but this one profits from other optimizations instead) */
    double calcAngle2D(TVector3& vecA, TVector3& vecB) {
			double angle = ((vecA[0] * vecB[0] + vecA[1] * vecB[1]) / (vecA.Perp2() * vecB.Perp2()));
			return m_twoHitFilter.filterNan(angle);
		}

    /** calculates the angle between the hits/vectors (2D), generalized, returning unit: angle in radians */
    double fullAngle2D(TVector3& vecA, TVector3& vecB);

    /** calculates an estimation of the radius of given hits and existing estimation of circleCenter, returning unit: radius in [cm] (positive value)*/
    double calcRadius(TVector3& a, TVector3& b, TVector3& c, TVector3& circleCenter); // used by calcPt() and calcCircleDist2IP()

    /** calculates an estimation of circleCenter position, result is written into the 4th input-parameter */
    void calcCircleCenter(TVector3& a, TVector3& b, TVector3& c, TVector3& circleCenter) {
			TVector3 b2c = b - c;
			TVector3 a2b = a - b;
			TVector3 cAB = 0.5 * a2b + b; //([kx ky]  -[jx jy])/2 + [jx jy] = central point of outer segment (k-j)/2+j
			TVector3 cBC = 0.5 * b2c + c; // = central point of inner segment (l-k)/2+k
			TVector3 nAB(-a2b(1), a2b(0), 0.); //normal vector of m_vecAB
			TVector3 nBC(-b2c(1), b2c(0), 0.); //normal vector of m_vecBC
			double muVal = (((cAB(1) - cBC(1)) / nBC(1)) + (((cBC(0) - cAB(0)) / nAB(0)) * nAB(1) / nBC(1))) / (1. - ((nBC(0) / nAB(0)) * (nAB(1) / nBC(1))));
			circleCenter.SetX(cBC(0) + muVal * nBC(0)); // x-coord of intersection point
			circleCenter.SetY(cBC(1) + muVal * nBC(1)); // y-coord of intersection point
			circleCenter.SetZ(0.);
		}

    /** calculates calculates the sign of the curvature of given 3-hit-tracklet. a positive value represents a left-oriented curvature, a negative value means having a right-oriented curvature. first vector should be outer hit, second = center hit, third is inner hit*/
    int calcSign(TVector3& a, TVector3& b, TVector3& c);

  protected:

    TwoHitFilters m_twoHitFilter; /**< instance of TwoHitFilters-class used for some internal calculations */

    bool m_circleCenterCalculated; /**< initially set to false, will be set true if calcCircleCenter() is used at least once */
    bool m_radiusCalculated; /**< initially set to false, will be set true if calcInvCurvature() is used at least once */
    double m_radius; /**< radius[cm] of a circle in r-phi-plane formed by the 3 hits */
    double m_x2; /**< internal intermediate value storing x^2, no enduser-relevance */
    double m_y2; /**< internal intermediate value storing y^2, no enduser-relevance */
    double m_z2; /**< internal intermediate value storing z^2, no enduser-relevance */
    TVector3 m_centerABC;  /**< center position of a circle in r-phi-plane formed by the 3 hits */
    TVector3 m_hitA; /**< outer hit (position relevant for useful filter calculation) used for the filter calculation */
    TVector3 m_hitB; /**< center hit (position relevant for useful filter calculation) used for the filter calculation */
    TVector3 m_hitC; /**< inner hit (position relevant for useful filter calculation) used for the filter calculation */
    TVector3 m_vecAB; /**< vector pointing from center hit to outer hit (outer segment) */
    TVector3 m_vecBC; /**< vector pointing from inner hit to center hit (inner segment) */

  }; //end class ThreeHitFilters
} //end namespace Belle2

#endif //THREEHITFILTERS


