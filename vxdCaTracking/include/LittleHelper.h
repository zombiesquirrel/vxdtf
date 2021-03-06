/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/


#ifndef LITTLEHELPER_H
#define LITTLEHELPER_H

#include <TVector3.h>



namespace Belle2 {
  namespace Tracking {

    /** bundles small but often used functions for smearing and others.  */
    class LittleHelper {
    public:

      /** Empty constructor. For initialisation only, an object generated this way can be misleading (since mean and sigma are set automatically), therefore reset() should be called at least once in this case */
      LittleHelper():
        m_meanValue(0.),
        m_sigmaValue(1.) {}

      /** Constructor. first parameter is the mean, second is the sigma of the distribution */
      LittleHelper(double mean, double sigma):
        m_meanValue(mean),
        m_sigmaValue(sigma) {}

      /** Destructor. */
      ~LittleHelper() {}


      /** Overrides Constructor-Setup. Needed if you want to reuse the instance instead of recreating one */
      void resetValues(double mean, double sigma) {
        m_meanValue = mean;
        m_sigmaValue = sigma;
      }

      /** use this member if you want to smear your value using gauss within a certain area (generalized). */
      double smearValueGauss(double low, double high, double mean, double sigma);

      /** use this member if you want to smear your value using gauss within a certain area using preset for mean and sigma. */
      double smearValueGauss(double low, double high) { return smearValueGauss(low, high, m_meanValue, m_sigmaValue); }

      /** use this member if you want to smear your value using gauss resulting in a positive value (generalized). */
      double smearPositiveGauss(double high, double mean, double sigma) { return smearValueGauss(0, high, mean, sigma); }

      /** use this member if you want to smear your value using gauss resulting in a positive value using preset for mean and sigma. */
      double smearPositiveGauss(double high) {  return smearValueGauss(0, high, m_meanValue, m_sigmaValue); }

      /** use this member if you want to smear your value using gauss resulting in a value between 0-1 (generalized). */
      double smearNormalizedGauss(double mean, double sigma) { return smearValueGauss(0, 1, mean, sigma); }

      /** use this member if you want to smear your value using gauss resulting in a value between 0-1 using preset for sigma. */
      double smearNormalizedGauss(double mean) { return smearValueGauss(0, 1, mean, m_sigmaValue); }

      /** use this member if you want to smear your value using gauss resulting in a value between 0-1 using preset for mean and sigma. */
      double smearNormalizedGauss() { return smearValueGauss(0, 1, m_meanValue, m_sigmaValue); }

      /** use this member if you want to smear your value using uniform within a certain area (generalized). */
      double smearValueUniform(double low, double high, double mean, double sigma);

      /** use this member if you want to smear your value using uniform within a certain area using preset for mean and sigma. */
      double smearValueUniform(double low, double high) { return smearValueUniform(low, high, m_meanValue, m_sigmaValue); }

      /** use this member if you want to smear your value using uniform resulting in a positive value (generalized). */
      double smearPositiveUniform(double high, double mean, double sigma) { return smearValueUniform(0, high, mean, sigma); }

      /** use this member if you want to smear your value using uniform resulting in a positive value using preset for mean and sigma. */
      double smearPositiveUniform(double high) { return smearValueUniform(0, high, m_meanValue, m_sigmaValue); }

      /** use this member if you want to smear your value using uniform resulting in a value between 0 and 1 (generalized). */
      double smearNormalizedUniform(double mean, double sigma) { return smearValueUniform(0, 1, mean, sigma); }

      /** use this member if you want to smear your value using uniform resulting in a value between 0-1 using preset for mean and sigma. */
      double smearNormalizedUniform() { return smearValueUniform(0, 1, m_meanValue, m_sigmaValue); }

      /** use this member if you want a uniformly distributed integer value between first and second variable */
      int getRandomIntegerUniform(int low, int high);

      /** use this member if you want a gaussian distributed integer value between first and second variable */
      int getRandomIntegerGauss(int low, int high);
    protected:
      /** safety check whether incoming values make sense for smearing */
      bool checkSanity(double low, double high, double mean, double sigma);

      double m_meanValue; /**< delivers preset for a mean value to be smeared */
      double m_sigmaValue; /**< delivers preset for a sigma value to determine the degree of smearing */
    }; //end class LittleHelper
  } // end namespace Tracking
} //end namespace Belle2

#endif //LITTLEHELPER


