#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This steering file will
import os
from basf2 import *
set_log_level(LogLevel.ERROR)
set_random_seed(1)


geometry = register_module('Geometry')
geometry.param('Components', [
    'HeavyMetalShield',
    'MagneticField',
    'PXD',
    'SVD',
    ])
eventCounter = register_module('EventCounter')
eventCounter.logging.log_level = LogLevel.INFO
eventCounter.param('stepSize', 25)


gearbox = register_module('Gearbox')

geometry = register_module('Geometry')
geometry.param('Components', ['MagneticField', 'PXD', 'SVD'])  # only the tracking detectors will be simulated. Makes this example much faster


# Show progress of processing
progress = register_module('Progress')
rootinput = register_module('RootInput')
#rootinput.param('inputFileName', 'pGun1GeV5000ev2tea19t148.root')
rootinput.param('inputFileName', 'Ev5kPdg-13Tracks1Teta90PhiAllPt1000B15.root')

mctrackfinder = register_module('MCTrackFinder')
param_mctrackfinder = {
    'UseCDCHits': 0,
    'UseSVDHits': 1,
    'UsePXDHits': 1,
    'Smearing': 0,
    }
mctrackfinder.param(param_mctrackfinder)

trackfitter = register_module('GenFitter2')
trackfitter.logging.log_level = LogLevel.WARNING
trackfitchecker = register_module('TrackFitChecker')
trackfitchecker.logging.log_level = LogLevel.INFO  # the reults of the statistical tests will only show up at info or debug level
trackfitchecker.param('testSi', True)
trackfitchecker.param('writeToTextFile', True)
trackfitchecker.param('truthAvailable', True)
trackfitchecker.param('robustTests', True)
trackfitchecker.param('writeToRootFile', True)
trackfitchecker.param('inspectTracks', False)
trackfitchecker.param('outputFileName', 'siTracking')

# Create paths
main = create_path()
# Add modules to paths
main.add_module(progress)
main.add_module(rootinput)
main.add_module(gearbox)
main.add_module(geometry)
main.add_module(eventCounter)
main.add_module(mctrackfinder)
main.add_module(trackfitter)
main.add_module(trackfitchecker)
# Process events
process(main)
