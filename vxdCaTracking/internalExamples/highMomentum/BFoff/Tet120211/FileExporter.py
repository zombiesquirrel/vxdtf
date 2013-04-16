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
nonRootDataExporter.param('smearTrueHits', True)


mctrackfinder = register_module('MCTrackFinder')
mctrackfinder.logging.log_level = LogLevel.WARNING
param_mctrackfinder = {  # 'primary'
    'UseCDCHits': 0,
    'UseSVDHits': 1,
    'UsePXDHits': 1,
    'MinimalNDF': 2,
    'UseClusters': 0,
    'WhichParticles': ['PXD', 'SVD'],
    'GFTrackCandidatesColName': '', #mcTracks
    }
    # 'Force2DSVDClusters': 1,
    # 'forceExisting2DClusters4SVD': 0
mctrackfinder.param(param_mctrackfinder)

rootinput = register_module('RootInput')
#rootinput.param('inputFileName', 'pGun1GeV5000ev2tea19t148.root')
rootinput.param('inputFileName', 'Ev5kPdg211Tracks1Teta120Phi100Pt1000B0.root')


display = register_module('Display')

# The Options parameter is a combination of:
# A autoscale PXD/SVD errors - use when hits are too small to be seen
# D draw detectors - draw simple detector representation (with different size)
#   for each hit
# H draw track hits
# M draw track markers - intersections of track with detector planes
#   (use with T)
# P draw detector planes
# S scale manually - spacepoint hits are drawn as spheres and scaled with
#   errors
# T draw track (straight line between detector planes)
#
# Note that you can always turn off an individual detector component or track
# interactively by removing its checkmark in the 'Eve' tab.
#
# This option only makes sense when ShowGFTracks/ShowGFTrackCands is used
display.param('Options', 'AHTM')  # default

# should hits always be assigned to a particle with c_PrimaryParticle flag?
# with this option off, many tracking hits will be assigned to secondary e-
display.param('AssignHitsToPrimaries', 1)

# show all primary MCParticles?
display.param('ShowAllPrimaries', True)

# show all charged MCParticles? (SLOW)
# display.param('ShowCharged', True)

# show all neutral MCParticles? (SLOW)
# display.param('ShowNeutrals', True)

# show tracks?
display.param('ShowGFTracks', True)

# show track candidates?
# You most likely don't want this unless you are a tracking developer
display.param('ShowGFTrackCands', False)
# If ShowGFTrackCands is true, you can set this option to switch between
# PXD/SVDClusters and PXD/SVDTrueHits
#display.param('UseClusters', True)

# save events non-interactively (without showing window)?
display.param('Automatic', False)



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
main.add_module(mctrackfinder)
main.add_module(nonRootDataExporter)
#main.add_module(display)
# Process events
process(main)

print statistics
