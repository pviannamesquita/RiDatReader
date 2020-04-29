# -*- coding: utf-8 -*-

"""
This is a small module for conveniently reading .RiDat binary files in Python. It only requires numpy to work.
The main function is "read_ridat_file", which receives the file's path as input, and returns 4 things:
....the time data of the acquisition
....the real component of the signal
....the imaginary component of the signal
....the acquisition parameters (a class with "System", "Application" and "Processing" parameters, and a title string)

Another function is "export_ridat_data_to_text_file", which opens a .RiDat file and export the time, real and imaginary
arrays to a text file, with values separated by '\t' (default) or another delimiter defined by the yser.

The remaining functions are for reading the file's bytes into data types, while the classes only serve as a way to wrap
all of the 70343196 zillion parameters inside these .RiDat files. These are divided in "System", "Application" and 
"Processing" parameters. There are also "RfChannel" parameters.

I didn't kept track of what all these parameters means, but it appears that most of them have the same name inside RINMR.

This is a free software with a MIT License.

Created by Pedro Vianna Mesquita -> pedroviannamesquita@gmail.com
"""

__author__ = 'Pedro Vianna Mesquita'
__copyright__ = '2019'
__credits__ = ["UFFLAR", 'Schlumberger']
__license__ = 'MIT License'
__version__ = '1.0'
__maintainer__ = 'Pedro Vianna Mesquita'
__email__ = 'pedroviannamesquita@gmail.com'
__status__ = 'active'

import struct
import numpy as np


# Class for storing Rf Channel parameters
class RfChannelsParameters(object):
    def __init__(self):
        self.SF = 0.0
        self.Offset = 0.0
        self.MultReg = 0
        self.PhaseTwiddle = 0
        self.ChanAOffset = 0
        self.ChanBOffset = 0
        self.ExtAPhaseTrim = 0
        self.ExtAAmpTrim = 0
        self.ExtBPhaseTrim = 0
        self.ExtBAmpTrim = 0
        self.IntAAmpTrim = 0
        self.IntBAmpTrim = 0
        self.PhaseTrim0 = 0
        self.AmpTrim0 = 0
        self.PhaseTrim90 = 0
        self.AmpTrim90 = 0
        self.PhaseTrim180 = 0
        self.AmpTrim180 = 0
        self.PhaseTrim270 = 0
        self.AmpTrim270 = 0
        self.quadtrim = 0


# Class for storing System parameters
class SysParameters(object):
    def __init__(self):
        self.Dead1 = 0.0
        self.Dead2 = 0.0
        self.P90 = 0.0
        self.P180 = 0.0
        self.rf_channels = [RfChannelsParameters(), RfChannelsParameters(), RfChannelsParameters()]
        self.GSH1, self.GSH2, self.GSH3, self.GSH4, self.GSH5 = "", "", "", "", ""
        self.EndTime = 0.0
        self.PreXK, self.PreXA = np.zeros(4, dtype=np.float64), np.zeros(4, dtype=np.float64)
        self.PreYK, self.PreYA = np.zeros(4, dtype=np.float64), np.zeros(4, dtype=np.float64)
        self.PreZK, self.PreZA = np.zeros(4, dtype=np.float64), np.zeros(4, dtype=np.float64)
        self.XB0K, self.XB0A = 0.0, 0.0
        self.YB0K, self.YB0A = 0.0, 0.0
        self.ZB0K, self.ZB0A = 0.0, 0.0
        self.DummyParam1, self.DummyParam2 = 0.0, 0.0
        self.Dec90 = 0.0
        self.CPD = ""
        self.Trigger = 0
        self.XB0, self.YB0, self.ZB0 = 0.0, 0.0, 0.0
        self.XOffset, self.YOffset, self.ZOffset = 0.0, 0.0, 0.0
        self.Acquisition = 0


# Class for storing Application parameters
class AppParameters(object):
    def __init__(self):
        self.SI = 0
        self.DW = 0.0
        self.Pulses = np.zeros(5, dtype=np.float64)
        self.RD = 0.0    # Recycle Delay
        self.tau = 0.0   # tau
        self.Delays = np.zeros(32, dtype=np.float64)
        self.NS = 0      # Number of Scans
        self.FW = 0.0
        self.PH1, self.PH2, self.PH3, self.PH4, self.PH5 = "", "", "", "", ""
        self.RG = 0.0    # Receiver Gain
        self.NECH = 0    # Number of Echos
        self.SW = 0.0
        self.DB = 0.0    # Attenuation ?
        self.Bessel = 0.0
        self.Butterworth = 0.0
        self.SequenceName = ""
        self.RfAmps_ch0 = np.zeros(6, dtype=np.float64)    # 6 rf amplitudes channel 0
        self.RfAmps_ch1 = np.zeros(6, dtype=np.float64)    # 6 rf amplitudes channel 1
        self.WW = 0.0     # WobbleWidth ?
        self.Counters = np.zeros(32, dtype=np.int32)
        self.GRead, self.GPhase, self.GSlice = 0, 0, 0   # GX, GY, GZ previously?
        self.Gradients = np.zeros(32, dtype=np.int32)
        self.MAC1, self.MAC2 = 0.0, 0.0
        self.SH1, self.SH2, self.SH3, self.SH4, self.SH5 = "", "", "", "", ""
        self.DS = 0
        self.NA = 0
        self.GradientsIncrements = np.zeros(9, dtype=np.int32)
        self.DimX, self.DimY, self.DimZ, self.DimC = 0, 0, 0, 0
        self.ImageEchos, self.ImageSlices = 0, 0
        self.GradPhase, self.GradSlice, self. GradRead = "", "", ""
        self.TimePoints = 0  # Points in Read direction for which time points are saved in .RiImage
        self.SNR = 0  # signal to noise ratio?
        self.FPs = np.zeros(5, dtype=np.float64)  # ??
        self.GREADX, self.GREADY, self.GREADZ = 0.0, 0.0, 0.0
        self.GPHASEX, self.GPHASEY, self.GPHASEZ = 0.0, 0.0, 0.0
        self.GSLICEX, self.GSLICEY, self.GSLICEZ = 0.0, 0.0, 0.0
        self.MoreGains = np.zeros(9, dtype=np.float64)


# Class for storing Processing parameters
class ProcessingParameters(object):
    def __init__(self):
        self.ProcFlags = 0
        self.ProcDummies = None
        self.LB = 0.0
        self.PA = 0.0
        self.PB = 0.0
        self.DP = 0.0
        self.SMP = 0
        self.PivotPoint = 0
        self.NOBC = 0
        self.PPRF = 0
        self.PPTH = 0.0
        self.PPBL = 0.0
        self.PPAF = 0
        self.INC2D = 0.0
        self.SD2D = 0.0


class AcquisitionParameters(object):
    def __init__(self, sys, app, proc, comment):
        self.sys = sys
        self.app = app
        self.proc = proc
        self.title = comment


# reads 4 bytes from a file, and turn them into a Python integer.
def read_int_from_file(read_file):
    return int(struct.unpack('i', read_file.read(4))[0])


# reads 8 bytes from a file, and turn them into a np.float64.
def read_double_from_file(read_file):
    return np.float64(struct.unpack('d', read_file.read(8))[0])


# reads 4 bytes from a file, and turn them into a np.float32.
def read_float_from_file(read_file):
    return np.float32(struct.unpack('f', read_file.read(4))[0])


# reads a number of bytes from a file, and turn them into a Python String.
def read_string_from_file(read_file, size):
    return str(read_file.read(size), "utf-8").rstrip('\x00')


# reads arrays of data from file, depending on which type and size of array.
def read_data_sequence_from_file(str_d_type, num_elements, read_file):
    if str_d_type == 'f':
        data = np.zeros(num_elements, dtype=np.float32)
        for i in range(num_elements):
            data[i] = read_float_from_file(read_file)
    elif str_d_type == 'd':
        data = np.zeros(num_elements, dtype=np.float64)
        for i in range(num_elements):
            data[i] = read_double_from_file(read_file)
    elif str_d_type == 'i':
        data = np.zeros(num_elements, dtype=np.int32)
        for i in range(num_elements):
            data[i] = read_int_from_file(read_file)
    else:
        raise ValueError("fuck me")
    return data


# This is the main thing. Receives the path of the .RiDat file, and returns
# 3 numpy arrays (for the time values, and the amplitudes of the real and
# imaginary channels), and a Class with all parameters, divided
# as "System", "Application" and "Processing" parameters (sys, app, proc variables).
def read_ridat_file(f_path):
    with open(f_path, 'rb') as ridat_file:

        # sanitity check for the file. If magic number is not 190955, this isn't a .RiImage or .RiDat file
        magic_number = read_int_from_file(ridat_file)
        if magic_number != 190955:
            raise IOError("The Magic number of the file is wrong. File is not a valid RiDat or is corrupted.")

        # file version.
        file_version = read_int_from_file(ridat_file)
        if file_version == 1:
            raise IOError("File is RiImage. Can not read it")
        elif file_version != 0:
            raise IOError("Could not understand file version")

        # Sizes (number of bytes) for each data section of the file.
        Sect1Size = read_int_from_file(ridat_file)
        Sect2Size = read_int_from_file(ridat_file)
        Sect3Size = read_int_from_file(ridat_file)
        Sect4Size = read_int_from_file(ridat_file)
        Comment = read_string_from_file(ridat_file, 128)

        IdEndMark = read_int_from_file(ridat_file)

        # ######################################################################################################
        # #################################################################################### SYSTEM PARAMETERS
        ridat_file.seek(Sect1Size)
        sys_params = SysParameters()

        sys_params.Dead1 = read_float_from_file(ridat_file)
        sys_params.Dead2 = read_float_from_file(ridat_file)
        sys_params.P90 = read_float_from_file(ridat_file)
        sys_params.P180 = read_float_from_file(ridat_file)

        sys_params.rf_channels[0].SF = read_double_from_file(ridat_file)
        sys_params.rf_channels[0].Offset = read_double_from_file(ridat_file)
        Dummy1 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].MultReg = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].PhaseTwiddle = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].ChanAOffset = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].ChanBOffset = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].ExtAPhaseTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].ExtAAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].ExtBPhaseTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].ExtBAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].IntAAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].IntBAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].PhaseTrim0 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].AmpTrim0 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].PhaseTrim90 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].AmpTrim90 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].PhaseTrim180 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].AmpTrim180 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].PhaseTrim270 = read_int_from_file(ridat_file)
        sys_params.rf_channels[0].AmpTrim270 = read_int_from_file(ridat_file)

        sys_params.rf_channels[1].SF = read_double_from_file(ridat_file)
        sys_params.rf_channels[1].Offset = read_double_from_file(ridat_file)
        sys_params.rf_channels[1].MultReg = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].PhaseTwiddle = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].ChanAOffset = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].ChanBOffset = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].ExtAPhaseTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].ExtAAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].ExtBPhaseTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].ExtBAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].IntAAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].IntBAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].PhaseTrim0 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].AmpTrim0 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].PhaseTrim90 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].AmpTrim90 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].PhaseTrim180 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].AmpTrim180 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].PhaseTrim270 = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].AmpTrim270 = read_int_from_file(ridat_file)

        sys_params.rf_channels[2].SF = read_double_from_file(ridat_file)
        sys_params.rf_channels[2].Offset = read_double_from_file(ridat_file)
        sys_params.rf_channels[2].MultReg = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].PhaseTwiddle = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].ChanAOffset = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].ChanBOffset = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].ExtAPhaseTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].ExtAAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].ExtBPhaseTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].ExtBAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].IntAAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].IntBAmpTrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].PhaseTrim0 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].AmpTrim0 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].PhaseTrim90 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].AmpTrim90 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].PhaseTrim180 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].AmpTrim180 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].PhaseTrim270 = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].AmpTrim270 = read_int_from_file(ridat_file)

        sys_params.rf_channels[0].quadtrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[1].quadtrim = read_int_from_file(ridat_file)
        sys_params.rf_channels[2].quadtrim = read_int_from_file(ridat_file)

        sys_params.GSH1 = read_string_from_file(ridat_file, 20)
        sys_params.GSH2 = read_string_from_file(ridat_file, 20)
        sys_params.GSH3 = read_string_from_file(ridat_file, 20)
        sys_params.GSH4 = read_string_from_file(ridat_file, 20)
        sys_params.GSH5 = read_string_from_file(ridat_file, 20)

        sys_params.EndTime = read_double_from_file(ridat_file)

        sys_params.PreXK[0] = read_float_from_file(ridat_file)
        sys_params.PreXA[0] = read_float_from_file(ridat_file)
        sys_params.PreXK[1] = read_float_from_file(ridat_file)
        sys_params.PreXA[1] = read_float_from_file(ridat_file)
        sys_params.PreXK[2] = read_float_from_file(ridat_file)
        sys_params.PreXA[2] = read_float_from_file(ridat_file)
        sys_params.PreXK[3] = read_float_from_file(ridat_file)
        sys_params.PreXA[3] = read_float_from_file(ridat_file)

        sys_params.PreYK[0] = read_float_from_file(ridat_file)
        sys_params.PreYA[0] = read_float_from_file(ridat_file)
        sys_params.PreYK[1] = read_float_from_file(ridat_file)
        sys_params.PreYA[1] = read_float_from_file(ridat_file)
        sys_params.PreYK[2] = read_float_from_file(ridat_file)
        sys_params.PreYA[2] = read_float_from_file(ridat_file)
        sys_params.PreYK[3] = read_float_from_file(ridat_file)
        sys_params.PreYA[3] = read_float_from_file(ridat_file)

        sys_params.PreZK[0] = read_float_from_file(ridat_file)
        sys_params.PreZA[0] = read_float_from_file(ridat_file)
        sys_params.PreZK[1] = read_float_from_file(ridat_file)
        sys_params.PreZA[1] = read_float_from_file(ridat_file)
        sys_params.PreZK[2] = read_float_from_file(ridat_file)
        sys_params.PreZA[2] = read_float_from_file(ridat_file)
        sys_params.PreZK[3] = read_float_from_file(ridat_file)
        sys_params.PreZA[3] = read_float_from_file(ridat_file)

        sys_params.XB0K = read_float_from_file(ridat_file)
        sys_params.XB0A = read_float_from_file(ridat_file)
        sys_params.YB0K = read_float_from_file(ridat_file)
        sys_params.YB0A = read_float_from_file(ridat_file)
        sys_params.ZB0K = read_float_from_file(ridat_file)
        sys_params.ZB0A = read_float_from_file(ridat_file)

        sys_params.DummyPar1 = read_float_from_file(ridat_file)
        sys_params.DummyPar2 = read_float_from_file(ridat_file)

        sys_params.Dec90 = read_float_from_file(ridat_file)
        sys_params.CPD = read_string_from_file(ridat_file, 20)
        sys_params.Trigger = read_int_from_file(ridat_file)
        sys_params.XB0 = read_float_from_file(ridat_file)
        sys_params.YB0 = read_float_from_file(ridat_file)
        sys_params.ZB0 = read_float_from_file(ridat_file)
        sys_params.XOffset = read_float_from_file(ridat_file)
        sys_params.YOffset = read_float_from_file(ridat_file)
        sys_params.ZOffset = read_float_from_file(ridat_file)
        sys_params.Acquisition = read_int_from_file(ridat_file)

        SysEndMark = read_int_from_file(ridat_file)

        # ###################################################################################################
        # #################################################################################### APP PARAMETERS
        ridat_file.seek(Sect1Size + Sect2Size)
        app_params = AppParameters()

        app_params.SI = read_int_from_file(ridat_file)
        app_params.DW = read_float_from_file(ridat_file)
        app_params.Pulses = read_data_sequence_from_file('f', 5, ridat_file)
        app_params.RD = read_float_from_file(ridat_file)
        app_params.tau = read_float_from_file(ridat_file)
        app_params.Delays[:5] = read_data_sequence_from_file('f', 5, ridat_file)
        app_params.NS = read_int_from_file(ridat_file)
        app_params.FW = read_float_from_file(ridat_file)
        app_params.PH1 = read_string_from_file(ridat_file, 132)
        app_params.PH2 = read_string_from_file(ridat_file, 132)
        app_params.PH3 = read_string_from_file(ridat_file, 132)
        app_params.PH4 = read_string_from_file(ridat_file, 132)
        app_params.PH5 = read_string_from_file(ridat_file, 132)
        app_params.RG = read_float_from_file(ridat_file)
        app_params.NECH = read_int_from_file(ridat_file)
        app_params.SW = read_double_from_file(ridat_file)
        app_params.DB = read_int_from_file(ridat_file)
        app_params.Bessel = read_double_from_file(ridat_file)
        app_params.Butterworth = read_double_from_file(ridat_file)
        app_params.SequenceName = read_string_from_file(ridat_file, 32)
        app_params.RfAmps_ch0 = read_data_sequence_from_file('f', 6, ridat_file)
        app_params.RfAmps_ch1 = read_data_sequence_from_file('f', 6, ridat_file)
        app_params.WW = read_float_from_file(ridat_file)
        app_params.Counters[:5] = read_data_sequence_from_file('i', 5, ridat_file)
        app_params.GRead = read_int_from_file(ridat_file)
        app_params.GPhase = read_int_from_file(ridat_file)
        app_params.GSlice = read_int_from_file(ridat_file)
        app_params.Gradients[:9] = read_data_sequence_from_file('i', 9, ridat_file)
        app_params.MAC1 = read_float_from_file(ridat_file)
        app_params.MAC2 = read_float_from_file(ridat_file)
        app_params.SH1 = read_string_from_file(ridat_file, 20)
        app_params.SH2 = read_string_from_file(ridat_file, 20)
        app_params.SH3 = read_string_from_file(ridat_file, 20)
        app_params.SH4 = read_string_from_file(ridat_file, 20)
        app_params.SH5 = read_string_from_file(ridat_file, 20)
        app_params.DS = read_int_from_file(ridat_file)
        app_params.NA = read_int_from_file(ridat_file)
        app_params.GradientsIncrements = read_data_sequence_from_file('i', 9, ridat_file)
        app_params.DimX = read_int_from_file(ridat_file)
        app_params.DimY = read_int_from_file(ridat_file)
        app_params.DimZ = read_int_from_file(ridat_file)
        app_params.DimC = read_int_from_file(ridat_file)
        app_params.ImageEchos = read_int_from_file(ridat_file)
        app_params.ImageSlices = read_int_from_file(ridat_file)
        app_params.Delays[5:12] = read_data_sequence_from_file('f', 7, ridat_file)
        app_params.GradPhase = read_string_from_file(ridat_file, 4)
        app_params.GradSlice = read_string_from_file(ridat_file, 4)
        app_params.GradRead = read_string_from_file(ridat_file, 4)
        app_params.TimePoints = read_int_from_file(ridat_file)
        app_params.SNR = read_int_from_file(ridat_file)
        app_params.Counters[5:12] = read_data_sequence_from_file('i', 7, ridat_file)
        app_params.FPs = read_data_sequence_from_file('f', 5, ridat_file)
        app_params.GREADX = read_float_from_file(ridat_file)
        app_params.GREADY = read_float_from_file(ridat_file)
        app_params.GREADZ = read_float_from_file(ridat_file)
        app_params.GPHASEX = read_float_from_file(ridat_file)
        app_params.GPHASEY = read_float_from_file(ridat_file)
        app_params.GPHASEZ = read_float_from_file(ridat_file)
        app_params.GSLICEX = read_float_from_file(ridat_file)
        app_params.GSLICEY = read_float_from_file(ridat_file)
        app_params.GSLICEZ = read_float_from_file(ridat_file)
        app_params.Delays[12:32] = read_data_sequence_from_file('f', 20, ridat_file)
        app_params.Counters[12:32] = read_data_sequence_from_file('i', 20, ridat_file)
        app_params.Gradients[9:32] = read_data_sequence_from_file('i', 23, ridat_file)
        app_params.MoreGains = read_data_sequence_from_file('f', 9, ridat_file)

        AppEndMark = read_int_from_file(ridat_file)

        # ###################################################################################################
        # ############################################################################# PROCESSING PARAMETERS
        ridat_file.seek(Sect1Size + Sect2Size + Sect3Size)
        proc_params = ProcessingParameters()

        proc_params.ProcFlags = read_int_from_file(ridat_file)
        proc_params.ProcDummies = read_data_sequence_from_file('i', 9, ridat_file)
        proc_params.LB = read_float_from_file(ridat_file)
        proc_params.PA = read_float_from_file(ridat_file)
        proc_params.PB = read_float_from_file(ridat_file)
        proc_params.DP = read_float_from_file(ridat_file)
        proc_params.SMP = read_int_from_file(ridat_file)
        proc_params.PivotPoint = read_int_from_file(ridat_file)
        proc_params.NOBC = read_int_from_file(ridat_file)
        proc_params.PPRF = read_int_from_file(ridat_file)
        proc_params.PPTH = read_double_from_file(ridat_file)
        proc_params.PPBL = read_double_from_file(ridat_file)
        proc_params.PPAF = read_int_from_file(ridat_file)
        proc_params.INC2D = read_float_from_file(ridat_file)
        proc_params.SD2D = read_double_from_file(ridat_file)

        ProcEndMark = read_int_from_file(ridat_file)

        # ###################################################################################################
        # ############################################################################################## DATA
        real_values, imag_values, time_values = [], [], []
        ridat_file.seek(Sect1Size + Sect2Size + Sect3Size + Sect4Size)

        while True:
            try:
                real_values.append(read_float_from_file(ridat_file))
                imag_values.append(read_float_from_file(ridat_file))
                time_values.append(read_double_from_file(ridat_file))
            except:
                break

        real_values = np.array(real_values, dtype=np.float64)
        imag_values = np.array(imag_values, dtype=np.float64)
        time_values = np.array(time_values, dtype=np.float64)

        # ###################################################################################################
        # ###################################################################################################

    acquisition_parameters = AcquisitionParameters(sys_params, app_params, proc_params, Comment)
    return time_values, real_values, imag_values, acquisition_parameters


# this function opens a .RiDat and saves the time, real and imag data into a text file.
# inputs are the path of the .Ridat, and the path of the file to save.
def export_ridat_data_to_text_file(ridat_path, save_path, delimiter='\t'):
    if not delimiter:
        raise ValueError("The delimiter can not be empty.")
    time, real, imag, acq_params = read_ridat_file(ridat_path)
    data = np.zeros((time.size, 3), dtype=np.float64)
    data[:, 0], data[:, 1], data[:, 2] = time, real, imag
    header = 'Time (us)' + delimiter + "Real (Machine Units)" + delimiter + "Imag (Machine Units)"
    np.savetxt(save_path, data, delimiter=delimiter, header=header)

