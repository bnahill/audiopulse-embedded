# -*- coding: utf-8 -*-

from apulse_iface import APulseIface, FixedTone
import numpy as np

import matplotlib
matplotlib.use('Qt4Agg')
import pylab

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from PySide import QtGui, QtCore

if __name__ == '__main__':
    iface = APulseIface()
    iface.connect()
    iface.reset()
    status = iface.get_status()
    print status

    tone = FixedTone(1000, 1000, 10000, 65, 0)
    iface.config_tones([tone])

    iface.config_capture(1500, 100, 256)
    iface.start()

    status = iface.get_status()
    print status