/**************************************************************************
 * BASF2 (Belle Analysis Framework 2)                                     *
 * Copyright(C) 2010 - Belle II Collaboration                             *
 *                                                                        *
 * Author: The Belle II Collaboration                                     *
 * Contributors: Jakob Lettenbichler                                      *
 *                                                                        *
 * This software is provided "as is" without any warranty.                *
 **************************************************************************/


#ifndef ExporterTcInfo_H
#define ExporterTcInfo_H

#include <vector>
#include <string>
#include <TVector3.h>

 /// WARNING dummy so far!
namespace Belle2 {

  /** Bundles information for a single track candidate to be stored by EventInfo (needed for HitExporter, which is needed by NonRootDataExportModule) */
	class ExporterTcInfo {
	public:
		/** Standard constructor. */
		ExporterTcInfo() {}
		
		/** Destructor. */
    ~ExporterTcInfo() {}
		

	protected:
	};
} //end namespace Belle2

#endif //ExporterTcInfo


