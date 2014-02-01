# -*- coding: utf-8 -*-

from apulse_iface import APulseIface, FixedTone, APulseStatus
import numpy as np
import sys

import matplotlib
matplotlib.use('Qt4Agg')
matplotlib.rcParams['backend.qt4']='PySide'
import pylab

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from PySide import QtGui, QtCore


class DataSeries(object):
    def __init__(self, label, data, xunits, yunits):
        self.data = data
        self.label = label
        self.xunits = xunits
        self.yunits = yunits


class PSDSeries(DataSeries):
    def __init__(self, label, data):
        super(PSDSeries, self).__init__(label, data, "Hz", "dB SPL")
        assert len(data) == 257, "Wrong PSD length"

class TimeSeries(DataSeries):
    def __init__(self, label, data):
        super(TimeSeries, self).__init__(label, data, "Hz", "dB SPL")
        assert len(data) == 257, "Wrong PSD length"

class PlotFigure(object):
    signals = None

    def __init__(self):
        pass

    def addSeries(self, series):
        # Add a new series
        if self.signals:
            if((self.signals[0].xunits == series.xunits) and
               (self.signals[0].yunits == series.yunits)):
                self.signals.append(series)
        else:
            self.signals = [series]

    def mkWidget(self, parent=None):
        fig = Figure(figsize=(600, 600),
                     dpi=72,
                     facecolor=(1, 1, 1),
                     edgecolor=(0, 0, 0))
        ax = fig.add_subplot(1, 1, 1)
        for s in self.signals:
            ax.plot(s)
        canvas = FigureCanvas(fig)
        return canvas



class UIWindow(QtGui.QMainWindow):
    iface = None

    def __init__(self):
        super(UIWindow, self).__init__()
        self.iface = APulseIface()
        self.setupUI()

    def setupUI(self):
        self.statusBar().showMessage("Started!")
        self.setGeometry(0,0,800, 600)
        self.setWindowTitle("APulse UI")

        mainwidget = QtGui.QWidget(self)
        self.setCentralWidget(mainwidget)
        hlayout = QtGui.QHBoxLayout(mainwidget)
        mainwidget.setLayout(hlayout)
        lframe = QtGui.QFrame(self)
        rframe = QtGui.QFrame(self)
        hlayout.addWidget(lframe)
        hlayout.addWidget(rframe)
        lframe.setFixedWidth(300)
        rframe.setSizePolicy(QtGui.QSizePolicy.Expanding,
                             QtGui.QSizePolicy.Expanding)

        leftlayout = QtGui.QVBoxLayout(lframe)
        rightlayout = QtGui.QVBoxLayout(rframe)
        lframe.setLayout(leftlayout)
        rframe.setLayout(rightlayout)

        buttonpanel = QtGui.QFrame(lframe)
        buttongrid = QtGui.QGridLayout(buttonpanel)

        listpanel = QtGui.QFrame(lframe)
        listlist = QtGui.QListWidget(listpanel)
        listlayout = QtGui.QVBoxLayout(listpanel)
        listlayout.addWidget(listlist)

        leftlayout.addWidget(buttonpanel)
        leftlayout.addWidget(listpanel)

        connbutton = QtGui.QPushButton("&Connect", buttonpanel)
        rstbutton = QtGui.QPushButton("&Reset", buttonpanel)
        statbutton = QtGui.QPushButton("&Get Status", buttonpanel)
        startbutton = QtGui.QPushButton("&Start", buttonpanel)
        calbutton = QtGui.QPushButton("&Calibrate", buttonpanel)
        decalbutton = QtGui.QPushButton("&Decalibrate", buttonpanel)
        databutton = QtGui.QPushButton("&Get Data", buttonpanel)

        buttongrid.addWidget(connbutton, 0, 0, 1, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(rstbutton, 0, 2, 1, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(statbutton, 1, 0, 1, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(startbutton, 1, 2, 1, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(calbutton, 2, 0, 1, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(decalbutton, 2, 2, 1, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(databutton, 3, 0, 1, 4, QtCore.Qt.AlignHCenter)

        f_label = QtGui.QLabel("f (Hz)")
        t1_label = QtGui.QLabel("t1 (ms)")
        t2_label = QtGui.QLabel("t2 (ms)")
        db_label = QtGui.QLabel("A (dB SPL)")

        f_1 = QtGui.QLineEdit("1000", buttonpanel)
        t1_1 = QtGui.QLineEdit("200", buttonpanel)
        t2_1 = QtGui.QLineEdit("4000", buttonpanel)
        db_1 = QtGui.QLineEdit("65.0", buttonpanel)

        f_2 = QtGui.QLineEdit("1500", buttonpanel)
        t1_2 = QtGui.QLineEdit("200", buttonpanel)
        t2_2 = QtGui.QLineEdit("4000", buttonpanel)
        db_2 = QtGui.QLineEdit("65.0", buttonpanel)

        t1_capture_label = QtGui.QLabel("t1 (ms)")
        n_epochs_label = QtGui.QLabel("Epochs")
        overlap_label = QtGui.QLabel("Overlap")
        capture_label = QtGui.QLabel("Capture")
        t1_capture = QtGui.QLineEdit("300", buttonpanel)
        n_epochs = QtGui.QLineEdit("100", buttonpanel)
        overlap = QtGui.QLineEdit("256", buttonpanel)

        buttongrid.addWidget(f_label, 6, 0, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(t1_label, 6, 1, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(t2_label, 6, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(db_label, 6, 3, QtCore.Qt.AlignHCenter)

        buttongrid.addWidget(f_1, 7, 0)
        buttongrid.addWidget(t1_1, 7, 1)
        buttongrid.addWidget(t2_1, 7, 2)
        buttongrid.addWidget(db_1, 7, 3)

        buttongrid.addWidget(f_2, 8, 0)
        buttongrid.addWidget(t1_2, 8, 1)
        buttongrid.addWidget(t2_2, 8, 2)
        buttongrid.addWidget(db_2, 8, 3)

        buttongrid.addWidget(t1_capture_label, 9, 1, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(n_epochs_label, 9, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(overlap_label, 9, 3, QtCore.Qt.AlignHCenter)

        buttongrid.addWidget(capture_label, 10, 0, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(t1_capture, 10, 1, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(n_epochs, 10, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(overlap, 10, 3, QtCore.Qt.AlignHCenter)

        for i in [f_1, t1_1, t2_1, db_1, f_2, t1_2, t2_2, db_2,
                  t1_capture, n_epochs, db_2]:
            i.setInputMethodHints(QtCore.Qt.ImhFormattedNumbersOnly)

        # Frame that will contain figures
        plotpanel = QtGui.QFrame(rframe)
        plotpanel.setSizePolicy(QtGui.QSizePolicy.Expanding,
                                QtGui.QSizePolicy.Expanding)
        messagebox = QtGui.QTextEdit(rframe)
        messagebox.setFixedHeight(200)
        rightlayout.addWidget(plotpanel)
        rightlayout.addWidget(messagebox)

        self.connbutton = connbutton
        self.statbutton = statbutton
        self.rstbutton = rstbutton
        self.startbutton = startbutton
        self.calbutton = calbutton
        self.decalbutton = decalbutton
        self.databutton = databutton

        self.t1_capture = t1_capture
        self.n_epochs = n_epochs
        self.overlap = overlap

        self.plotpanel = plotpanel
        self.messagebox = messagebox

        self.tonetext = [
            [f_1, t1_1, t2_1, db_1],
            [f_2, t1_2, t2_2, db_2],
        ]

        self.datalist = listlist

        self.rstbutton.clicked.connect(self.reset)
        self.statbutton.clicked.connect(self.getstat)
        self.startbutton.clicked.connect(self.start)
        self.databutton.clicked.connect(self.get_data)
        self.disconnect()

        self.show()

    def reset(self):
        self.iface.reset()
        self.messagebox.setText(str(self.iface.get_status()))

    def getstat(self):
        self.messagebox.setText(str(self.iface.get_status()))

    def start(self):
        tones = list()
        try:
            for i in range(1):
                tones.append(
                    FixedTone(int(self.tonetext[i][0].displayText()),
                              int(self.tonetext[i][1].displayText()),
                              int(self.tonetext[i][2].displayText()),
                              float(self.tonetext[i][3].displayText()), i))
            self.iface.config_tones(tones)
        except Exception:
            self.statusBar().showMessage("Please provide numeric tone values")
            return
        try:
            self.iface.config_capture(int(self.t1_capture.displayText()),
                                      int(self.n_epochs.displayText()),
                                      int(self.overlap.displayText()))
        except:
            self.statusBar().showMessage(
                "Please provide numeric capture values")
            return
        self.iface.start()
        self.getstat()

    def get_data(self):
        if not self.iface.get_status().is_done():
            self.statusBar().showMessage("No completed test available")
            return

        (psd, avg) = self.iface.get_data()
        print(psd)
        print(avg)

        fig = PlotFigure()
        fig.addSeries(avg)
        layout = QtGui.QHBoxLayout(self.plotpanel)
        self.plotpanel.setLayout(layout)
        layout.addWidget(fig.mkWidget(self.plotpanel))

    def disconnect(self):
        self.iface.disconnect()
        self.connbutton.setText("Connect")
        self.connbutton.clicked.connect(self.connect)
        for b in [self.statbutton, self.rstbutton, self.startbutton,
                  self.calbutton, self.decalbutton, self.databutton]:
            b.setEnabled(False)

    def connect(self):
        try:
            self.iface.connect()
            self.iface.reset()
            self.connbutton.setText("Disconnect")
            self.connbutton.clicked.connect(self.disconnect)
            for b in [self.statbutton, self.rstbutton, self.startbutton,
                      self.calbutton, self.decalbutton, self.databutton]:
                b.setEnabled(True)
        except:
            self.statusBar().showMessage("Couldn't connect...")

    def dumb_test(self):
        tone = FixedTone(1000, 1000, 10000, 65, 0)
        self.iface.config_tones([tone])

        self.iface.config_capture(1500, 100, 256)
        self.iface.start()

        status = self.iface.get_status()
        print(status)


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    a = UIWindow()
    sys.exit(app.exec_())
