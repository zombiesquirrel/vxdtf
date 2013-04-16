#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This steering file will
import os
from sys import argv
from basf2 import *
from time import time

numTracks = 1 #pGun only
numEvents = 5000
initialValue = 1
pMin = 0.1 # 1.0# pGun only
pMax = 0.1 # 1.0 # pGun only
tMin = 90. # pGun only
tMax = 90. # pGun only
phiMin = 100.
phiMax = 100.

print 'running {events:} events, Seed {theSeed:} - evtGen No BG'.format(events=numEvents,
        theSeed=initialValue)

set_log_level(LogLevel.ERROR)
set_random_seed(initialValue)
# register the modules and set there options
evtmetagen = register_module('EvtMetaGen')
evtmetagen.param('ExpList', [0])
evtmetagen.param('RunList', [1])
evtmetagen.param('EvtNumList', [numEvents])

evtmetainfo = register_module('EvtMetaInfo')

gearbox = register_module('Gearbox')

evtgeninput = register_module('EvtGenInput')
evtgeninput.logging.log_level = LogLevel.WARNING

geometry = register_module('Geometry')
geometry.param('Components', [
    #'BeamPipe',
    #'Cryostat',
    'HeavyMetalShield',
    'MagneticField',
    'PXD',
    'SVD',
    #'SVD-Support',
    #'KLM'
    ])

g4sim = register_module('FullSim')
g4sim.param('StoreAllSecondaries', True)


pGun = register_module('ParticleGun')
param_pGun = {
    'pdgCodes': [-211], # , 211], # 13: muons, 211: charged pions
    'nTracks': numTracks,
    'momentumGeneration': 'uniformPt',
    'momentumParams': [pMin, pMax],
    'thetaGeneration': 'uniform',
    'thetaParams': [tMin, tMax],
    'phiGeneration': 'uniform',
    'phiParams': [phiMin, phiMax],
    'vertexGeneration': 'uniform',
    'xVertexParams': [-0.0, 0.0],
    'yVertexParams': [-0.0, 0.0],
    'zVertexParams': [-0.0, 0.0],
    }
pGun.param(param_pGun)

eventCounter = register_module('EventCounter')
eventCounter.logging.log_level = LogLevel.INFO
eventCounter.param('stepSize', 25)

simpleoutput = register_module('RootOutput')
#simpleoutput.param('outputFileName', 'evtGeneV5000.root')
simpleoutput.param('outputFileName', 'Ev5kPdg-211Tracks1Teta90Phi100Pt1000B15.root')


#mctrackfinder = register_module('MCTrackFinder')
#mctrackfinder.logging.log_level = LogLevel.WARNING
## select which detectors you would like to use
#param_mctrackfinder = {  # 'primary'
    #'UseCDCHits': 0,
    #'UseSVDHits': 1,
    #'UsePXDHits': 1,
    #'MinimalNDF': 6,
    #'UseClusters': 0,
    #'WhichParticles': ['PXD', 'SVD'],
    #'GFTrackCandidatesColName': '',
    #}
#mctrackfinder.param(param_mctrackfinder)

#mctrackfinder = register_module('MCTrackFinder')
#mctrackfinder.logging.log_level = LogLevel.WARNING
## select which detectors you would like to use
#param_mctrackfinder = {  # 'primary'
    #'UseCDCHits': 0,
    #'UseSVDHits': 1,
    #'UsePXDHits': 1,
    #'MinimalNDF': 6,
    #'UseClusters': 0,
    #'WhichParticles': ['PXD', 'SVD'],
    #'GFTrackCandidatesColName': '',
    #}
#mctrackfinder.param(param_mctrackfinder)

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



# Create paths
main = create_path()

# Add modules to paths

# main.add_module(inputM)
main.add_module(evtmetagen)
main.add_module(evtmetainfo)

main.add_module(gearbox)
main.add_module(geometry)
main.add_module(pGun)
#main.add_module(evtgeninput)  # instead of pGun
main.add_module(g4sim)
#main.add_module(mctrackfinder)
main.add_module(simpleoutput)
main.add_module(eventCounter)
#main.add_module(nonRootDataExporter)
main.add_module(display)
# Process events
process(main)

print statistics
