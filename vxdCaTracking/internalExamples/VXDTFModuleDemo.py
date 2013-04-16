#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This steering file will
import os
from sys import argv
from basf2 import *
from time import time

numEvents = 10000
initialValue = 1
secSetup = ['evtgenHIGH', 'evtgenSTD', 'evtgenLOW']  # ,'evtgenHIGH' 'evtgenSTD', 'evtgenLOW'] # when using the same secSetup several times, use this for setting Value

rSeed = initialValue

print 'running {events:} events, Seed {theSeed:} - evtGen No BG'.format(events=numEvents,
        theSeed=initialValue)

set_log_level(LogLevel.ERROR)
set_random_seed(rSeed)
# register the modules and set there options
evtmetagen = register_module('EvtMetaGen')
evtmetagen.param('ExpList', [0])
evtmetagen.param('RunList', [1])
evtmetagen.param('EvtNumList', [numEvents])

evtmetainfo = register_module('EvtMetaInfo')

gearbox = register_module('Gearbox')

pxdDigitizer = register_module('PXDDigitizer')
svdDigitizer = register_module('SVDDigitizer')
pxdClusterizer = register_module('PXDClusterizer')
svdClusterizer = register_module('SVDClusterizer')
evtgeninput = register_module('EvtGenInput')
evtgeninput.logging.log_level = LogLevel.WARNING

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

g4sim = register_module('FullSim')
g4sim.param('StoreAllSecondaries', True)

vxdtf = register_module('VXDTF')
vxdtf.logging.log_level = LogLevel.INFO
vxdtf.logging.debug_level = 1
param_vxdtf = {
    'tccMinLayer': [4],
    'detectorType': [1],
    'sectorSetup': secSetup,
    'calcQIType': 'kalman',
    }
    # 'storeBrokenQI': True,
    # 'smearMean': 0.0,
    # 'smearSigma': 0.0001,
    # 'KFBackwardFilter': 0,
vxdtf.param(param_vxdtf)

mctrackfinder = register_module('MCTrackFinder')
mctrackfinder.logging.log_level = LogLevel.WARNING

# select which detectors you would like to use
param_mctrackfinder = {  # 'primary'
    'UseCDCHits': 0,
    'UseSVDHits': 1,
    'UsePXDHits': 0,
    'MinimalNDF': 6,
    'UseClusters': 1,
    'WhichParticles': ['PXD', 'SVD'],
    'GFTrackCandidatesColName': 'mcTracks',
    }
    # 'Force2DSVDClusters': 1,
    # 'forceExisting2DClusters4SVD': 0
mctrackfinder.param(param_mctrackfinder)

mctf2 = register_module('MCTrackFinder')
mctf2.logging.log_level = LogLevel.WARNING

# select which detectors you would like to use
param_mctf2 = {  # 'primary'
    'UseCDCHits': 0,
    'UseSVDHits': 1,
    'UsePXDHits': 0,
    'MinimalNDF': 6,
    'UseClusters': 1,
    'WhichParticles': ['PXD', 'SVD'],
    'GFTrackCandidatesColName': 'mcTracks2',
    }
    # 'Force2DSVDClusters': 1,
    # 'forceExisting2DClusters4SVD': 0
mctf2.param(param_mctf2)

eventCounter = register_module('EventCounter')
eventCounter.logging.log_level = LogLevel.INFO
eventCounter.param('stepSize', 25)

analyzer = register_module('TFAnalizer')
analyzer.logging.log_level = LogLevel.INFO
analyzer.logging.debug_level = 11
param_analyzer = {'printExtentialAnalysisData': False,  # set true if PRINTINFO is wanted
		  'mcTCname' : 'mcTracks',
		  'caTCname' : 'mcTracks2'}
    # 'minTMomentumFilter': 0.05,
    # 'maxTMomentumFilter': 0.200,
analyzer.param(param_analyzer)

display = register_module('Display')
param_Display = {'ShowGFTrackCands': 1, 'UseClusters': 1}
display.param(param_Display)

# Create paths
main = create_path()

# Add modules to paths

# main.add_module(inputM)
main.add_module(evtmetagen)
main.add_module(evtmetainfo)

main.add_module(gearbox)
main.add_module(geometry)
# main.add_module(pGun)
main.add_module(evtgeninput)  # instead of pGun
main.add_module(g4sim)
main.add_module(pxdDigitizer)
main.add_module(pxdClusterizer)
main.add_module(svdDigitizer)
main.add_module(svdClusterizer)
main.add_module(eventCounter)
main.add_module(mctf2)
main.add_module(mctrackfinder)
main.add_module(analyzer)
# main.add_module(display)
# Process events
process(main)

print statistics
