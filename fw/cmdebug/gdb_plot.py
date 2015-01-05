import gdb
import matplotlib
# For some reasons the other backends don't work...
matplotlib.use('TKAgg')

import numpy as np
import pylab
import re
import multiprocessing


class Plot(gdb.Command):

    def __init__(self):
        super(Plot, self).__init__("plot", gdb.COMMAND_DATA)

    def invoke(self, args, from_tty):
        s = str(args).split(" ")

        if not s[0]:
            gdb.write("What do you want to plot?\n")
        else:
            r = re.compile('(?P<base>[A-Za-z_\*]+)'
                           '(?P<targs><[0-9A-Za-z_ \*,]+>)?'
                           '(?: \[(?P<numel>[0-9]+)\])?')
            arrs = []
            for word in s:
                gdb.write("Word: '{}'\n".format(word))
                val = gdb.parse_and_eval(word)
                match = r.match(str(val.type)).groupdict()
                gdb.write("Lazy: '{}'\n".format(val.is_lazy))

                gdb.write("Type: '{}'\n".format(val.type))
                gdb.write("Type: '{}'\n".format(match['base']))
                gdb.write("Numel: '{}'\n".format(match['numel']))

                if not match['numel']:
                    gdb.write("This isn't an array\n")
                    continue

                numel = int(match['numel'])

                base = match['base']
                if base == 'float':
                    arr = np.empty(numel, dtype=np.float32)
                    for i in range(numel):
                        arr[i] = val[i]
                elif base in ['int32', 'int32_t', 'int']:
                    arr = np.empty(numel, dtype=np.int32)
                    for i in range(numel):
                        arr[i] = val[i]
                elif base in ['uint32_t', 'uint32']:
                    arr = np.empty(numel, dtype=np.uint32)
                    for i in range(numel):
                        arr[i] = val[i]
                elif base in ['sFractional', 'uFractional']:
                    parts = re.match('<(?P<i>[0-9\.]+)[A-Za-z]+, ?'
                                     '(?P<f>[0-9a-z\.]+)[A-Za-z]+>',
                                     match['targs']).groupdict()
                    fbits = int(parts['f'])
                    arr = np.empty(numel, dtype=np.float32)
                    gdb.write("VAL: {}".format(val[0]['i']))
                    for i in range(numel):
                        arr[i] = float(int(val[i]['i'])) / (2 ** fbits)
                else:
                    gdb.write("Yeah don't know what to do with "
                              "type '{}'\n".format(base))
                    continue

                arrs.append((word, arr,))

            def plotlauncher(data):
                title = ', '.join(map(lambda x, y: x, data))
                fig = pylab.gcf()
                fig.canvas.set_window_title(title)
                for a in data:
                    pylab.plot(a[1])
                pylab.title(title)
                pylab.show()

            if arrs:
                multiprocessing.Process(target=plotlauncher,
                                        args=(arrs,)).start()
                #pylab.plot(arr)
                #pylab.show(block=False)
Plot()