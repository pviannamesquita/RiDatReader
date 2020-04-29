/*
#RIDAT_READER
This is a small program for conveniently reading .RiDat binary files in C++.
The main function is "readRiDatFile", which receives the file's path (std::string) 
as input, and stores all data in a "RiDatAcqData" struct, which has:
....the time data of the acquisition       (std::vector<double>)
....the real component of the signal       (std::vector<double>)
....the imaginary component of the signal  (std::vector<double>)
....the acquisition parameters (structs with "System", "Application" and "Processing" parameters, and a title std::string)

I didn't kept track of what all these parameters means, but it appears that most of them have the same name inside RINMR.

This is a free software with a MIT License.

Created by Pedro Vianna Mesquita -> pedroviannamesquita@gmail.com
*/

#ifndef RIDAT_READER_H
#define RIDAT_READER_H

#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::array;
using std::ifstream;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////// STRUCTS FOR STORING PARAMETERS

struct RiDatRfChannelsParameters
{
	double SF, Offset;
	int MultReg, PhaseTwiddle, ChanAOffset, ChanBOffset, ExtAPhaseTrim, ExtAAmpTrim, ExtBPhaseTrim, ExtBAmpTrim, IntAAmpTrim;
	int IntBAmpTrim, PhaseTrim0, AmpTrim0, PhaseTrim90, AmpTrim90, PhaseTrim180, AmpTrim180, PhaseTrim270, AmpTrim270, quadtrim;
};

struct RiDatSysParameters
{
	float Dead1, Dead2, P90, P180;
	RiDatRfChannelsParameters rf_channel_0, rf_channel_1, rf_channel_2;
	string GSH1, GSH2, GSH3, GSH4, GSH5;
	double EndTime;
	array<float, 4> PreXK, PreXA, PreYK, PreYA, PreZK, PreZA;
	float XB0K, XB0A, YB0K, YB0A, ZB0K, ZB0A, DummyPar1, DummyPar2, Dec90;
	string CPD;
	int Trigger;
	float XB0, YB0, ZB0, XOffset,YOffset, ZOffset;
	int Acquisition;
};

struct RiDatAppParameters
{
	int SI;
	float DW;
	array<float, 5> Pulses;
	float RD, tau;
	array<float, 32> Delays;
	int NS;
	float FW;
	string PH1, PH2, PH3, PH4, PH5;
	float RG;
	int NECH;
	double SW;
	int DB;
	double Bessel, Butterworth;
	string SequenceName;
	array<float, 6> RfAmps_ch0, RfAmps_ch1;
	float WW;
	array<int, 32> Counters;
	int GRead, GPhase, GSlice;
	array<int, 32> Gradients;
	float MAC1, MAC2;
	string SH1, SH2, SH3, SH4, SH5;
	int DS, NA;
	array<int, 9> GradientIncrements;
	int DimX, DimY, DimZ, DimC, ImageEchos, ImageSlices;
	string GradPhase, GradSlice, GradRead;
	int TimePoints, SNR;
	array<float, 5> FPs;
	float GREADX, GREADY, GREADZ, GPHASEX, GPHASEY, GPHASEZ, GSLICEX, GSLICEY, GSLICEZ;
	array<float, 9> MoreGains[9];
};

struct RiDatProcParameters
{
	int ProcFlags;
	array<int, 9> ProcDummies;
	float LB, PA, PB, DP;
	int SMP, PivotPoint, NOBC, PPRF; 
	double PPTH, PPBL;
	int PPAF;
	float INC2D;
	double SD2D;
};

struct RiDatAcqData
{
	string title;
	RiDatSysParameters sys_params;
	RiDatAppParameters app_params;
	RiDatProcParameters proc_params;
	vector<double> time;
	vector<double> real_amplitude, imag_amplitude;
};

int readRiDatFile(string file_path, RiDatAcqData& acq_data);

#endif // !RIDAT_READER_H
