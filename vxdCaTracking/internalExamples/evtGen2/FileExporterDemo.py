#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This steering file will
import os
from sys import argv
from basf2 import *
from time import time

gearbox = register_module('Gearbox')
geometry = register_module('Geometry')
geometry.param('Components', [
    'BeamPipe',
    'Cryostat',
    'HeavyMetalShield',
    'MagneticField',
    'PXD',
    'SVD',
    'SVD-Support',
    ])

eventCounter = register_module('EventCounter')
eventCounter.logging.log_level = LogLevel.INFO
eventCounter.param('stepSize', 25)

nonRootDataExporter = register_module('NonRootDataExport')
nonRootDataExporter.logging.log_level = LogLevel.INFO
nonRootDataExporter.logging.debug_level = 1
nonRootDataExporter.param('exportTrueHits', 'all')
nonRootDataExporter.param('detectorType', 'VXD')
nonRootDataExporter.param('exportGFTCs', True)


#mctrackfinder = register_module('MCTrackFinder')
#mctrackfinder.logging.log_level = LogLevel.WARNING
#param_mctrackfinder = {  # 'primary'
    #'UseCDCHits': 0,
    #'UseSVDHits': 1,
    #'UsePXDHits': 0,
    #'MinimalNDF': 6,
    #'UseClusters': 0,
    #'WhichParticles': ['PXD', 'SVD'],
    #'GFTrackCandidatesColName': '', #mcTracks
    #}
    ## 'Force2DSVDClusters': 1,
    ## 'forceExisting2DClusters4SVD': 0
#mctrackfinder.param(param_mctrackfinder)

rootinput = register_module('RootInput')
rootinput.param('inputFileName', 'RootOutputEvtGen10k.root')


# Show progress of processing
progress = register_module('Progress')

# Create paths
main = create_path()
# Add modules to paths
main.add_module(progress)
main.add_module(rootinput)
main.add_module(gearbox)
main.add_module(geometry)
main.add_module(eventCounter)
main.add_module(nonRootDataExporter)
# main.add_module(display)
# Process events
process(main)

print statistics
