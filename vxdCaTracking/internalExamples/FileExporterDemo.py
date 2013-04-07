#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This steering file will
import os
from sys import argv
from basf2 import *
from time import time

numEvents = 10
initialValue = 1

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

eventCounter = register_module('EventCounter')
eventCounter.logging.log_level = LogLevel.INFO
eventCounter.param('stepSize', 1)

nonRootDataExporter = register_module('NonRootDataExport')
nonRootDataExporter.logging.log_level = LogLevel.DEBUG
nonRootDataExporter.logging.debug_level = 111
nonRootDataExporter.param('exportTrueHits', 'all')
nonRootDataExporter.param('detectorType', 'VXD')



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
main.add_module(nonRootDataExporter)
# main.add_module(display)
# Process events
process(main)

print statistics
