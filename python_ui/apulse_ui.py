# -*- coding: utf-8 -*-

from apulse_iface import APulseIface
from PySide import QtGui, QtCore

if __name__ == '__main__':
    iface = APulseIface()
    iface.connect()
    iface.reset()
