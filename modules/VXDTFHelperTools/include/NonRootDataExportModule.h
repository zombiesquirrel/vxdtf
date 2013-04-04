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

#ifndef NonRootDataExportModule_H_
#define NonRootDataExportModule_H_

#include <framework/core/Module.h>
#include "tracking/vxdCaTracking/HitExporter.h"
// #include <fstream>
#include <string>
#include <vector>


namespace Belle2 {

  /** The event counter module
   *
   * this module can be used for text-file-output of data (in the VXD) generated during events. It supports export of trueHits, clusterHits (converted to 2D hits in the SVD) and GFTrackCands
   *
   */
  class NonRootDataExportModule : public Module {

  public:

    /**
     * Constructor of the module.
     */
    NonRootDataExportModule();

    /** Destructor of the module. */
    virtual ~NonRootDataExportModule();

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

  protected:

// 		std::vector<std::string> m_PARAMGfTcArrays; /** carries names of all storearrays for gftrackcand which shall be exported */
		std::string m_PARAMExportTrueHits; /** allows you to export true hits. here, various values will be set at once, first part will determine whether you want to export PXD, SVD or VXD hits, second part says whether you want to export 'real' hits or 'background' hits, full example: 'SVDreal'. Wrong input values will cause fatal error. If you don't want to export trueHits, use 'none' */
// 		std::string m_PARAMExportClusters; /** TODO */
		std::string m_PARAMDetectorType; /** set detectorype. Please choose between 'PXD', 'SVD' (standard) or 'VXD'. Wrong input values will set to SVD with an error. */
		HitExporter m_exportContainer; /** container storing storing hit infos */
    int m_eventCounter; /**< knows current event number */
    int m_runCounter; /**< knows current run number */

  private:

  };
}

#endif /* NonRootDataExportModule_H_ */
