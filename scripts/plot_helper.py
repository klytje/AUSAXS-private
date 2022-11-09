import matplotlib.pyplot as plt
import numpy as np
from enum import Enum
import os

def isfloat(value: str):
    try:
        float(value)
        return True
    except ValueError:
        return False


class PlotType(Enum):
    Dataset = "PlotDataset"
    Histogram = "PlotHistogram"
    Image = "PlotImage"
    ImageAtoms = "PlotImageAtoms"
    Hline = "PlotHline"
    Vline = "PlotVline"
    Invalid = ""

# Options class. Defines how a dataset is plotted.
class Options:
    def __init__(self):
        # visuals
        self.color = "k"
        self.linestyle = "-"
        self.markerstyle = "."
        self.linewidth = 1
        self.markersize = 1
        self.drawline = True
        self.drawmarker = False
        self.drawerror = False

        # labels
        self.title = ""
        self.xlabel = "x"
        self.ylabel = "y"
        self.legend = ""

        # axes
        self.xrange = []
        self.yrange = []
        self.xlog = False
        self.ylog = False

    def parse_option(self, line):
        words = line.split()
        if len(words) < 2:
            return

        # visuals
        if (words[0] == "color"):
            self.color = words[1]
        elif (words[0] == "line_style"):
            self.linestyle = words[1]
        elif (words[0] == "marker_style"):
            self.markerstyle = words[1]
        elif (words[0] == "line_width"):
            self.linewidth = float(words[1])
        elif (words[0] == "marker_size"):
            self.markersize = float(words[1])
        elif (words[0] == "draw_line"):
            self.drawline = int(words[1])
        elif (words[0] == "draw_markers"):
            self.drawmarker = int(words[1])
        elif (words[0] == "draw_errors"):
            self.drawerror = int(words[1])

        # labels
        elif (words[0] == "title"):
            self.title = words[1]
        elif (words[0] == "xlabel"):
            self.xlabel = " ".join(words[1:])
        elif (words[0] == "ylabel"):
            self.ylabel = " ".join(words[1:])
        elif (words[0] == "legend"):
            self.legend = words[1]
        
        # axes
        elif (words[0] == "xlimits"):
            if (words[1] == words[2]):
                return
            self.xrange = [float(words[1]), float(words[2])]
        elif (words[0] == "ylimits"):
            if (words[1] == words[2]):
                return
            self.yrange = [float(words[1]), float(words[2])]
        elif (words[0] == "logx"):
            self.xlog = int(words[1])
        elif (words[0] == "logy"):
            self.ylog = int(words[1])
        elif (words[0] == "same"):
            self.same = int(words[1])
        else:
            print("Options.parse_option: Invalid option: " + words[0])
            exit(1)

class Dataset:
    def __init__(self, data, options):
        self.data = np.array(data)
        self.options = options

class Hline: 
    def __init__(self, y, options):
        self.y = y
        self.options = options

class Vline:
    def __init__(self, x, options):
        self.x = x
        self.options = options

def read_options(file):
    """Reads plot options from a .plot file.

    Args:
        file: The file iterator.

    Returns:
        options: The plot options.
    """

    options: Options = Options()
    while(line := file.readline()):
        line = line.rstrip()

        # empty lines separates sections
        if line == "":
            break

        options.parse_option(line)
    return options

def read_dataset(file):
    """Reads a dataset from a .plot file.

    Args:
        file: The file iterator.

    Returns:
        datasets: A list of datasets.
    """

    data: list[tuple[float]] = []
    while(line := file.readline()):
        line = line.rstrip()

        # empty lines separates sections
        if line == "":
            break

        words = line.split()

        # otherwise just parse the line
        l = [float(w) for w in words]
        data.append(l)
        continue
    return Dataset(data, read_options(file))

def read_hline(file):
    """Reads a horizontal line from a .plot file.

    Args:
        file: The file iterator.

    Returns:
        hline: The horizontal line.
    """

    val = float(file.readline().rstrip())
    file.readline() # empty space
    return Hline(val, read_options(file))

def read_vline(file):
    """Reads a vertical line from a .plot file.

    Args:
        file: The file iterator.

    Returns:
        vline: The vertical line.
    """

    val = float(file.readline().rstrip())
    file.readline() # empty space
    return Vline(val, read_options(file))

first_plot: bool = True
def plot_dataset(d: Dataset): 
    """Plots a dataset.

    Args:
        data: A list of datasets to plot.
        options: Plot options.
    """

    if d.options.drawerror:
        if (d.data.shape[1] < 3):
            print("plot_dataset: Not enough columns for error bars.")
            exit(1)
        plt.errorbar(d.data[:,0], d.data[:,1], yerr=d.data[:,2], 
            color=d.options.color, 
            linestyle="none",
            marker=d.options.markerstyle, 
            markersize=d.options.markersize, 
            label=d.options.legend,
            capsize=2,
            zorder=5
        )

    elif d.options.drawmarker: 
        plt.plot(d.data[:,0], d.data[:,1], 
            color=d.options.color, 
            linestyle="none",
            marker=d.options.markerstyle, 
            markersize=d.options.markersize, 
            label=d.options.legend
        )

    if d.options.drawline:
        plt.plot(d.data[:,0], d.data[:,1], 
            color=d.options.color, 
            linestyle=d.options.linestyle, 
            linewidth=d.options.linewidth, 
            label=d.options.legend
        )
    
    global first_plot
    if (first_plot):
        first_plot = False
        plt.title(d.options.title)
        plt.xlabel(r"{}".format(d.options.xlabel))
        plt.ylabel(r"{}".format(d.options.ylabel))
        plt.ticklabel_format(axis='both', style='sci', scilimits=(0, 4))
        if (d.options.xrange != []):
            plt.xlim(d.options.xrange)
        if (d.options.yrange != []):
            plt.ylim(d.options.yrange)
        if (d.options.xlog):
            plt.xscale("log")
        if (d.options.ylog):
            plt.yscale("log")
    if (d.options.legend != ""):
        plt.legend()        
    return

def plot_hline(h: Hline):
    """Plots a horizontal line.

    Args:
        h: The horizontal line to plot.
    """

    plt.axhline(h.y, 
        color=h.options.color, 
        linestyle=h.options.linestyle, 
        linewidth=h.options.linewidth, 
    )
    return

def plot_vline(v: Vline):
    """Plots a vertical line.

    Args:
        v: The vertical line to plot.
    """

    plt.axvline(v.x, 
        color=v.options.color, 
        linestyle=v.options.linestyle, 
        linewidth=v.options.linewidth, 
    )
    return

def plot_file(file: str):
    """Plots a .plot file.

    Args:
        file: The input file.
    """

    def determine_type(line: str) -> PlotType:
        for t in PlotType:
            if line == t.value:
                return t
        return PlotType.Invalid

    global first_plot
    first_plot = True
    plt.figure(figsize=(10, 8))
    with open(file) as f:
        # keep reading until eof
        while line := f.readline():
            # determine the type of plot to make
            match determine_type(line.rstrip()):
                case PlotType.Dataset:
                    dataset: Dataset = read_dataset(f)
                    plot_dataset(dataset)

                case PlotType.Hline:
                    hline: Hline = read_hline(f)
                    plot_hline(hline)

                case PlotType.Vline:
                    vline: Vline = read_vline(f)
                    plot_vline(vline)

                case PlotType.Histogram:
                    print("Histogram not implemented yet.")
                    exit(1)

                case PlotType.Image:
                    print("Image not implemented yet.")
                    exit(1)

                case PlotType.ImageAtoms:
                    print("ImageAtoms not implemented yet.")
                    exit(1)

                case _: 
                    print("plot_file: Invalid plot type: " + line)
                    exit(1)

    path = file.rsplit('.', 1)[0]
    plt.savefig(path)

    # delete the .plot file
    # os.remove(file)

    return