# -*- coding: utf-8 -*-

from apulse_iface import APulseIface, FixedTone
import numpy as np
import sys
import time

import matplotlib
matplotlib.use('Qt4Agg')
matplotlib.rcParams['backend.qt4']='PySide'
import pylab

from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from PySide import QtGui, QtCore


class DataSeries(object):
    def __init__(self, label, data, xunits, yunits, xvalues):
        self.data = data
        self.label = label
        self.xunits = xunits
        self.yunits = yunits
        self.xvalues = xvalues


class PSDSeries(DataSeries):
    def __init__(self, label, data):
        xvals = np.linspace(0, 8000, 257)
        super(PSDSeries, self).__init__(label, data, "Frequency (Hz)",
                                        "Power (SPL)", xvals)
        assert len(data) == 257, "Wrong PSD length"


class LogPSDSeries(DataSeries):
    def __init__(self, label, data):
        xvals = np.linspace(0, 8000, 257)
        data = 10.0 * np.log10(0.7071 * data / np.float128(0x7FFFFFFF))
        super(LogPSDSeries, self).__init__(label, data, "Frequency (Hz)",
                                           "Power (dB SPL)", xvals)
        assert len(data) == 257, "Wrong PSD length"


class TimeSeries(DataSeries):
    def __init__(self, label, data):
        xvals = np.linspace(0, 512.0 / 16000.0, 512)
        super(TimeSeries, self).__init__(label, data, "t (ms)",
                                         "Amplitude", xvals)
        assert len(data) == 512, "Wrong Average length"


class PlotFigure(object):
    signals = []
    canvas = None

    def __init__(self, parent=None):
        self.fig = Figure(figsize=(600, 600),
                          dpi=72,
                          facecolor=(1, 1, 1),
                          edgecolor=(0, 0, 0))
        self.canvas = FigureCanvas(self.fig)

    def addSeries(self, series):
        self.rmSeries(series)
        # Add a new series
        if self.signals:
            if((self.signals[0].xunits == series.xunits) and
               (self.signals[0].yunits == series.yunits)):
                self.signals.append(series)
            else:
                self.signals = [series]
        else:
            self.signals = [series]

    def rmSeries(self, series):
        if self.signals:
            try:
                self.signals.remove(series)
            except:
                pass

    def clearSeries(self):
        self.signals = []

    def draw(self):
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.ax.cla()
        for s in self.signals:
            self.ax.plot(s.xvalues, s.data, label=s.label)
        if len(self.signals):
            self.ax.set_xlabel(self.signals[0].xunits)
            self.ax.set_ylabel(self.signals[0].yunits)

        handles, labels = self.ax.get_legend_handles_labels()
        self.ax.legend(handles[::-1], labels[::-1])
        self.ax.legend(handles, labels)

        self.canvas.draw()

    def getWidget(self):
        return self.canvas


class SignalListItem(QtGui.QListWidgetItem):
    def __init__(self, signal):
        super(SignalListItem, self).__init__(str(signal.label))
        self.signal = signal


class UIWindow(QtGui.QMainWindow):
    iface = None

    def __init__(self):
        super(UIWindow, self).__init__()
        self.iface = APulseIface()
        self.setupUI()

    def setupUI(self):
        self.statusBar().showMessage("Started!")
        self.setGeometry(0, 0, 800, 600)
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

        f_1 = QtGui.QLineEdit("2000", buttonpanel)
        t1_1 = QtGui.QLineEdit("200", buttonpanel)
        t2_1 = QtGui.QLineEdit("8000", buttonpanel)
        db_1 = QtGui.QLineEdit("65.0", buttonpanel)

        f_2 = QtGui.QLineEdit("2400", buttonpanel)
        t1_2 = QtGui.QLineEdit("200", buttonpanel)
        t2_2 = QtGui.QLineEdit("8000", buttonpanel)
        db_2 = QtGui.QLineEdit("65.0", buttonpanel)

        t1_capture_label = QtGui.QLabel("t1 (ms)")
        t2_capture_label = QtGui.QLabel("t2 (ms")
        overlap_label = QtGui.QLabel("Overlap")
        capture_label = QtGui.QLabel("Capture")
        t1_capture = QtGui.QLineEdit("300", buttonpanel)
        t2_capture = QtGui.QLineEdit("7500", buttonpanel)
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
        buttongrid.addWidget(t2_capture_label, 9, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(overlap_label, 9, 3, QtCore.Qt.AlignHCenter)

        buttongrid.addWidget(capture_label, 10, 0, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(t1_capture, 10, 1, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(t2_capture, 10, 2, QtCore.Qt.AlignHCenter)
        buttongrid.addWidget(overlap, 10, 3, QtCore.Qt.AlignHCenter)

        for i in [f_1, t1_1, t2_1, db_1, f_2, t1_2, t2_2, db_2,
                  t1_capture, t2_capture, db_2]:
            i.setInputMethodHints(QtCore.Qt.ImhFormattedNumbersOnly)

        # Frame that will contain figures
        plotpanel = QtGui.QFrame(rframe)
        plotpanel.setSizePolicy(QtGui.QSizePolicy.Expanding,
                                QtGui.QSizePolicy.Expanding)
        messagebox = QtGui.QTextEdit(rframe)
        messagebox.setFixedHeight(200)
        rightlayout.addWidget(plotpanel)
        rightlayout.addWidget(messagebox)

        itemframe = QtGui.QFrame(lframe)
        itemlayout = QtGui.QGridLayout(itemframe)
        itemframe.setLayout(itemlayout)
        clearbutton = QtGui.QPushButton("&Clear", itemframe)
        clearallbutton = QtGui.QPushButton("&Clear All", itemframe)
        deletebutton = QtGui.QPushButton("&Delete", itemframe)
        deleteallbutton = QtGui.QPushButton("&Delete All", itemframe)
        itemlayout.addWidget(clearbutton, 0, 0)
        itemlayout.addWidget(clearallbutton, 0, 1)
        itemlayout.addWidget(deletebutton, 1, 0)
        itemlayout.addWidget(deleteallbutton, 1, 1)
        leftlayout.addWidget(itemframe)

        clearbutton.clicked.connect(self.clearhandler)
        clearallbutton.clicked.connect(self.clearallhandler)
        deletebutton.clicked.connect(self.deletehandler)
        deleteallbutton.clicked.connect(self.deleteallhandler)
        listlist.itemDoubleClicked.connect(self.addsignalhandler)
        calbutton.clicked.connect(self.calibrate)
        decalbutton.clicked.connect(self.decalibrate)

        self.connbutton = connbutton
        self.statbutton = statbutton
        self.rstbutton = rstbutton
        self.startbutton = startbutton
        self.calbutton = calbutton
        self.decalbutton = decalbutton
        self.databutton = databutton

        self.t1_capture = t1_capture
        self.t2_capture = t2_capture
        self.overlap = overlap

        self.plotpanel = plotpanel
        self.messagebox = messagebox

        self.fig = PlotFigure()
        figlayout = QtGui.QHBoxLayout(self.plotpanel)
        self.plotpanel.setLayout(figlayout)
        figlayout.addWidget(self.fig.getWidget())

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
            for i in range(2):
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
                                      int(self.t2_capture.displayText()),
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

        t = time.localtime()
        d = "{}.{}.{}.{}.{}".format(t.tm_mon, t.tm_mday, t.tm_hour,
                                    t.tm_min, t.tm_sec)
        self.datalist.addItem(SignalListItem(LogPSDSeries("LogPSD " + d, psd)))
        self.datalist.addItem(SignalListItem(PSDSeries("PSD " + d, psd)))
        self.datalist.addItem(SignalListItem(TimeSeries("Avg " + d, avg)))

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

    def calibrate(self):
        self.iface.calibrate()
        self.statusBar().showMessage("Device calibrating...")

    def decalibrate(self):
        self.iface.decalibrate()
        self.statusBar().showMessage("Device calibration cleared")

    def clearhandler(self):
        for i in self.datalist.selectedItems():
            self.fig.rmSeries(i.signal)
        self.fig.draw()

    def clearallhandler(self):
        for i in range(self.datalist.count()):
            self.fig.rmSeries(self.datalist.item(i).signal)
        self.fig.draw()

    def deletehandler(self):
        self.clearhandler()
        for i in self.datalist.selectedItems():
            row = self.datalist.row(i)
            self.datalist.takeItem(row)

    def deleteallhandler(self):
        self.clearallhandler()
        for i in range(self.datalist.count()):
            self.datalist.takeItem(0)

    def addsignalhandler(self, item):
        self.fig.addSeries(item.signal)
        self.fig.draw()


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    a = UIWindow()
    sys.exit(app.exec_())
