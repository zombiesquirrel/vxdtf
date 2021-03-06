#include <framework/gearbox/Const.h>
#include <framework/logging/Logger.h>
#include <tracking/vxdCaTracking/TrackletFilters.h>
#include <tracking/vxdCaTracking/LittleHelper.h>
#include <vector>
#include <TVector3.h>
#include <math.h>
#include <gtest/gtest.h>

using namespace std;

namespace Belle2 {
#define EXPECT_FATAL(x) EXPECT_EXIT(x,::testing::KilledBySignal(SIGABRT),"");

  /** Producing suitable input data for for the TrackletFilters-class */
  class TrackletFiltersTest : public ::testing::Test {
  protected:
  };

  /** TrackletFilters expects a row of hits stored as a vector of TVector3. These should be on a realistic track through the detector, meaning in the x-y-plane, they form a circle passing the origin (or a point near to it). There have to be at least 4 hits for the ZigZag-Test, for the circlefitter it can be 3 hits, but it should be 4 at least. we will test it first with an aequivalent of a 500 MeV (pT) track and another one having only 50 MeV. The reason for having more than one cirle is the fact that the circlefitter seems to be optimized somehow and therefore could have problems with circles formed by a small number of hits or circles with small radii*/
  TEST_F(TrackletFiltersTest, simpleTest)
  {
		TVector3 a, b, c, d, e, f, g, h, i, j, k, l, m; // there are up to 12+1 Hits (one for the primary vertex) possible within the VXD before the particle curls back the first time
		
		//The hits will be produced using the same radius (which will be smeared) and different phi-values to describe a cirle-segment covering only a part of the outline of the circle. To get a full helix, the z-value of the hits are changed accordingly to their original phi-value
		
		LittleHelper hBox = LittleHelper(); // smears hits using smearValueGauss(double low, double high, double mean, double sigma)
		double sigma = 0.1, scalePhi = 11.9/*11.9*/, scaleZ = 88., t90 = M_PI*0.5, radius = 11.;
		double aVal=0.0001, bVal=0.006, cVal=0.007, dVal=0.011, eVal=0.0124, fVal=0.018, gVal=0.0202, hVal=0.039, iVal=0.042, jVal=0.052, kVal=0.055, lVal=0.068, mVal=0.0715;
		//phi does not have to be smeared to simulate the realistic situation, some hits will be positioned pretty near next to each other
		a.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, aVal*scalePhi),
		b.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, bVal*scalePhi), //L1
		c.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, cVal*scalePhi),
		d.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, dVal*scalePhi), //L2
		e.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, eVal*scalePhi),
		f.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, fVal*scalePhi), //L3
		g.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, gVal*scalePhi),
		h.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, hVal*scalePhi), //L4
		i.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, iVal*scalePhi),
		j.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, jVal*scalePhi), //L5
		k.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, kVal*scalePhi),
		l.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, lVal*scalePhi), //L6
		m.SetMagThetaPhi(hBox.smearValueGauss(-1111., 1111., radius, sigma), t90, mVal*scalePhi);

		B2WARNING("before moving TVector3: a mag is " << a.Mag() << " with x,y,z: " << a.X() <<","<< a.Y() <<","<< a.Z())
		B2WARNING("before moving TVector3: b mag is " << b.Mag() << " with x,y,z: " << b.X() <<","<< b.Y() <<","<< b.Z())
		B2WARNING("before moving TVector3: c mag is " << c.Mag() << " with x,y,z: " << c.X() <<","<< c.Y() <<","<< c.Z())
		B2WARNING("before moving TVector3: d mag is " << d.Mag() << " with x,y,z: " << d.X() <<","<< d.Y() <<","<< d.Z())
		B2WARNING("before moving TVector3: e mag is " << e.Mag() << " with x,y,z: " << e.X() <<","<< e.Y() <<","<< e.Z())
		B2WARNING("before moving TVector3: f mag is " << f.Mag() << " with x,y,z: " << f.X() <<","<< f.Y() <<","<< f.Z())
		B2WARNING("before moving TVector3: g mag is " << g.Mag() << " with x,y,z: " << g.X() <<","<< g.Y() <<","<< g.Z())
		B2WARNING("before moving TVector3: h mag is " << h.Mag() << " with x,y,z: " << h.X() <<","<< h.Y() <<","<< h.Z())
		B2WARNING("before moving TVector3: i mag is " << i.Mag() << " with x,y,z: " << i.X() <<","<< i.Y() <<","<< i.Z())
		B2WARNING("before moving TVector3: j mag is " << j.Mag() << " with x,y,z: " << j.X() <<","<< j.Y() <<","<< j.Z())
		B2WARNING("before moving TVector3: k mag is " << k.Mag() << " with x,y,z: " << k.X() <<","<< k.Y() <<","<< k.Z())
		B2WARNING("before moving TVector3: l mag is " << l.Mag() << " with x,y,z: " << l.X() <<","<< l.Y() <<","<< l.Z())
		B2WARNING("before moving TVector3: m mag is " << m.Mag() << " with x,y,z: " << m.X() <<","<< m.Y() <<","<< m.Z())
		
		a.SetZ(aVal*scaleZ), b.SetZ(bVal*scaleZ), c.SetZ(cVal*scaleZ), d.SetZ(dVal*scaleZ), e.SetZ(eVal*scaleZ), f.SetZ(fVal*scaleZ), g.SetZ(gVal*scaleZ), h.SetZ(hVal*scaleZ), i.SetZ(iVal*scaleZ), j.SetZ(jVal*scaleZ), k.SetZ(kVal*scaleZ), l.SetZ(lVal*scaleZ), m.SetZ(mVal*scaleZ);
		
		//now we have got hits lying on a small part of a circle having its center at the origin. Therefore I have to move the circle so its arch passes the origin
		TVector3 move500 = TVector3(radius, 0.5, 0.);
		
		a -= move500, b -= move500, c -= move500, d -= move500, e -= move500, f -= move500, g -= move500, h -= move500, i -= move500, j -= move500, k -= move500, l -= move500, m -= move500;
		vector<TVector3> highPtMini, highPtMaxi;
		
		B2WARNING("after moving TVector3: a mag is " << a.Mag() << " with x,y,z: " << a.X() <<","<< a.Y() <<","<< a.Z())
		B2WARNING("after moving TVector3: b mag is " << b.Mag() << " with x,y,z: " << b.X() <<","<< b.Y() <<","<< b.Z())
		B2WARNING("after moving TVector3: c mag is " << c.Mag() << " with x,y,z: " << c.X() <<","<< c.Y() <<","<< c.Z())
		B2WARNING("after moving TVector3: d mag is " << d.Mag() << " with x,y,z: " << d.X() <<","<< d.Y() <<","<< d.Z())
		B2WARNING("after moving TVector3: e mag is " << e.Mag() << " with x,y,z: " << e.X() <<","<< e.Y() <<","<< e.Z())
		B2WARNING("after moving TVector3: f mag is " << f.Mag() << " with x,y,z: " << f.X() <<","<< f.Y() <<","<< f.Z())
		B2WARNING("after moving TVector3: g mag is " << g.Mag() << " with x,y,z: " << g.X() <<","<< g.Y() <<","<< g.Z())
		B2WARNING("after moving TVector3: h mag is " << h.Mag() << " with x,y,z: " << h.X() <<","<< h.Y() <<","<< h.Z())
		B2WARNING("after moving TVector3: i mag is " << i.Mag() << " with x,y,z: " << i.X() <<","<< i.Y() <<","<< i.Z())
		B2WARNING("after moving TVector3: j mag is " << j.Mag() << " with x,y,z: " << j.X() <<","<< j.Y() <<","<< j.Z())
		B2WARNING("after moving TVector3: k mag is " << k.Mag() << " with x,y,z: " << k.X() <<","<< k.Y() <<","<< k.Z())
		B2WARNING("after moving TVector3: l mag is " << l.Mag() << " with x,y,z: " << l.X() <<","<< l.Y() <<","<< l.Z())
		B2WARNING("after moving TVector3: m mag is " << m.Mag() << " with x,y,z: " << m.X() <<","<< m.Y() <<","<< m.Z())
		
		highPtMini.push_back(l), highPtMini.push_back(g), highPtMini.push_back(f), highPtMini.push_back(a);
		highPtMaxi.push_back(a), highPtMaxi.push_back(b), highPtMaxi.push_back(c), highPtMaxi.push_back(d), highPtMaxi.push_back(e), highPtMaxi.push_back(f), highPtMaxi.push_back(g), highPtMaxi.push_back(h), highPtMaxi.push_back(i), highPtMaxi.push_back(j), highPtMaxi.push_back(k), highPtMaxi.push_back(l), highPtMaxi.push_back(m);
		
// 		TVector3 testa = TVector3(2.,2.,0);
// 		TVector3 testb = TVector3(3.,1.,0);
// 		TVector3 testc = TVector3(2.,0.,0);
// 		TVector3 testd = TVector3(1.,1.,0);
		TVector3 testa = TVector3(1.,1.,0);
		TVector3 testb = TVector3(2.,0.,0);
		TVector3 testc = TVector3(1.,-1.,0);
		TVector3 testd = TVector3(1./sqrt(2.),1./sqrt(2.),0);
		TVector3 moveSmall = TVector3(0.,0.5, 0.);
		vector<TVector3> testVector;
		testVector.push_back(testa += moveSmall); testVector.push_back(testb+= moveSmall);testVector.push_back(testc+= moveSmall); testVector.push_back(testd += moveSmall);
		ThreeHitFilters f3h = ThreeHitFilters();
		B2WARNING(" calcsign mlk, lkj, kji, jih, ihg, hgf, gfe, fed, edc, dcb, cba:" <<f3h.calcSign(m,l,k) <<"," <<f3h.calcSign(l,k,j) <<","<<f3h.calcSign(k,j,i) <<","<< f3h.calcSign(j,i,h) <<","<< f3h.calcSign(i,h,g) <<","<< f3h.calcSign(h,g,f) <<","<< f3h.calcSign(g,f,e) <<","<< f3h.calcSign(f,e,d) <<","<< f3h.calcSign(e,d,c) <<","<< f3h.calcSign(d,c,b) <<","<< f3h.calcSign(c,b,a));
		EXPECT_EQ(f3h.calcSign(l, g, f), f3h.calcSign(g, f, a));
		
    TrackletFilters aFilter = TrackletFilters();
		double clapPhi = 0, clapR = 0, chi2, estimatedRadius = 0;
		
		aFilter.resetValues(highPtMaxi);
		EXPECT_EQ(false, aFilter.ziggZaggXY());
		EXPECT_EQ(false, aFilter.ziggZaggRZ());
		chi2 = aFilter.circleFit(clapPhi, clapR, estimatedRadius);
		B2WARNING("after maxi-test, chi2 is " << chi2 << ", clapPhi,clapR,estimatedRadius is: " << clapPhi<<","<<clapR<<","<<estimatedRadius)
		
		aFilter.resetValues(highPtMini);
		EXPECT_EQ(false, aFilter.ziggZaggXY());
		EXPECT_EQ(false, aFilter.ziggZaggRZ());
		chi2 = aFilter.circleFit(clapPhi, clapR, estimatedRadius);
		B2WARNING("after mini-test, chi2 is " << chi2 << " clapPhi,clapR,estimatedRadius is: " << clapPhi<<","<<clapR<<","<<estimatedRadius)
		
		aFilter.resetValues(testVector);
		EXPECT_EQ(false, aFilter.ziggZaggXY());
		EXPECT_EQ(false, aFilter.ziggZaggRZ());
		chi2 = aFilter.circleFit(clapPhi, clapR, estimatedRadius);
		B2WARNING("after testVector-test, chi2 is " << chi2 << " clapPhi,clapR,estimatedRadius is: " << clapPhi<<","<<clapR<<","<<estimatedRadius)
		
//     EXPECT_DOUBLE_EQ(0., aFilter.calcNormedDist3D());

  }
}  // namespace
