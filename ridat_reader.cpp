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

#include "ridat_reader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////// FUNCTIONS FOR READING DATA FROM BINARY FILES


static int read_int_from_binary_file(ifstream& file_stream)
{
	int value;
	file_stream.read(reinterpret_cast<char*>(&value), 4);
	return value;
}

static float read_float_from_binary_file(ifstream& file_stream)
{
	float value;
	file_stream.read(reinterpret_cast<char*>(&value), 4);
	return value;
}

static double read_double_from_binary_file(ifstream& file_stream)
{
	double value;
	file_stream.read(reinterpret_cast<char*>(&value), 8);
	return value;
}

static string read_string_from_binary_file(ifstream& file_stream, int lenght)
{
	char* temp = new char[lenght + 1];
	file_stream.read(temp, lenght);
	temp[lenght] = '\0';
	string return_string(temp);
	delete[] temp;
	return return_string;
}


static void read_data_from_binary_file_to_array(ifstream& file_stream, int* array_ptr, int size)
{
	for (int i = 0; i < size; i++) { array_ptr[i] = read_int_from_binary_file(file_stream); }
}

static void read_data_from_binary_file_to_array(ifstream& file_stream, float* array_ptr, int size)
{
	for (int i = 0; i < size; i++) { array_ptr[i] = read_float_from_binary_file(file_stream); }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int readRiDatFile(string file_path, RiDatAcqData& acq_data)
{
	ifstream ridat_file(file_path, std::ios::binary | std::ios::in);

	if (!ridat_file)
	{
		cout << "Could not open file at path " << file_path << endl;
		return 0;
	}

	// sanitity check for the file. If magic number is not 190955, this isn't a .RiImage or .RiDat file
	const int magic_number = read_int_from_binary_file(ridat_file);
	if (magic_number != 190955)
	{
		cout << "The Magic number of the file is wrong. File is not a valid RiDat or is corrupted." << endl;
		return -1;
	}

	// checking file version
	const int file_version = read_int_from_binary_file(ridat_file);
	if (file_version == 1)
	{
		cout << "File is RiImage. Can not read it" << endl;
		return -2;
	}
	else if (file_version != 0)
	{
		cout << "Could not understand file version" << endl;
		return -3;
	}

	const int Sect1Size = read_int_from_binary_file(ridat_file);
	const int Sect2Size = read_int_from_binary_file(ridat_file);
	const int Sect3Size = read_int_from_binary_file(ridat_file);
	const int Sect4Size = read_int_from_binary_file(ridat_file);

	acq_data.title = read_string_from_binary_file(ridat_file, 128);

	const int IdEndMark = read_int_from_binary_file(ridat_file);


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////// System Parameters
	ridat_file.seekg(Sect1Size);

	acq_data.sys_params.Dead1 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.Dead2 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.P90 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.P180 = read_float_from_binary_file(ridat_file);

	acq_data.sys_params.rf_channel_0.SF = read_double_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.Offset = read_double_from_binary_file(ridat_file);
	const int Dummy1 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.MultReg = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.PhaseTwiddle = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.ChanAOffset = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.ChanBOffset = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.ExtAPhaseTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.ExtAAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.ExtBPhaseTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.ExtBAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.IntAAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.IntBAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.PhaseTrim0 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.AmpTrim0 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.PhaseTrim90 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.AmpTrim90 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.PhaseTrim180 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.AmpTrim180 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.PhaseTrim270 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_0.AmpTrim270 = read_int_from_binary_file(ridat_file);

	acq_data.sys_params.rf_channel_1.SF = read_double_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.Offset = read_double_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.MultReg = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.PhaseTwiddle = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.ChanAOffset = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.ChanBOffset = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.ExtAPhaseTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.ExtAAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.ExtBPhaseTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.ExtBAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.IntAAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.IntBAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.PhaseTrim0 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.AmpTrim0 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.PhaseTrim90 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.AmpTrim90 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.PhaseTrim180 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.AmpTrim180 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.PhaseTrim270 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.AmpTrim270 = read_int_from_binary_file(ridat_file);

	acq_data.sys_params.rf_channel_2.SF = read_double_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.Offset = read_double_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.MultReg = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.PhaseTwiddle = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.ChanAOffset = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.ChanBOffset = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.ExtAPhaseTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.ExtAAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.ExtBPhaseTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.ExtBAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.IntAAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.IntBAmpTrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.PhaseTrim0 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.AmpTrim0 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.PhaseTrim90 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.AmpTrim90 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.PhaseTrim180 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.AmpTrim180 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.PhaseTrim270 = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.AmpTrim270 = read_int_from_binary_file(ridat_file);

	acq_data.sys_params.rf_channel_0.quadtrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_1.quadtrim = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.rf_channel_2.quadtrim = read_int_from_binary_file(ridat_file);

	acq_data.sys_params.GSH1 = read_string_from_binary_file(ridat_file, 20);
	acq_data.sys_params.GSH2 = read_string_from_binary_file(ridat_file, 20);
	acq_data.sys_params.GSH3 = read_string_from_binary_file(ridat_file, 20);
	acq_data.sys_params.GSH4 = read_string_from_binary_file(ridat_file, 20);
	acq_data.sys_params.GSH5 = read_string_from_binary_file(ridat_file, 20);

	acq_data.sys_params.EndTime = read_double_from_binary_file(ridat_file);

	acq_data.sys_params.PreXK[0] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXA[0] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXK[1] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXA[1] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXK[2] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXA[2] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXK[3] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreXA[3] = read_float_from_binary_file(ridat_file);

	acq_data.sys_params.PreYK[0] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYA[0] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYK[1] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYA[1] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYK[2] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYA[2] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYK[3] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreYA[3] = read_float_from_binary_file(ridat_file);

	acq_data.sys_params.PreZK[0] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZA[0] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZK[1] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZA[1] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZK[2] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZA[2] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZK[3] = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.PreZA[3] = read_float_from_binary_file(ridat_file);

	acq_data.sys_params.XB0K = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.XB0A = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.YB0K = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.YB0A = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.ZB0K = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.ZB0A = read_float_from_binary_file(ridat_file);

	acq_data.sys_params.DummyPar1 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.DummyPar1 = read_float_from_binary_file(ridat_file);

	acq_data.sys_params.Dec90 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.CPD = read_string_from_binary_file(ridat_file, 20);
	acq_data.sys_params.Trigger = read_int_from_binary_file(ridat_file);
	acq_data.sys_params.XB0 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.YB0 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.ZB0 = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.XOffset = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.YOffset = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.ZOffset = read_float_from_binary_file(ridat_file);
	acq_data.sys_params.Acquisition = read_int_from_binary_file(ridat_file);

	const int SysEndMark = read_int_from_binary_file(ridat_file);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////// Application Parameters
	ridat_file.seekg(Sect1Size + Sect2Size);

	acq_data.app_params.SI = read_int_from_binary_file(ridat_file);
	acq_data.app_params.DW = read_float_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Pulses.data(), 5);
	acq_data.app_params.RD = read_float_from_binary_file(ridat_file);
	acq_data.app_params.tau = read_float_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Delays.data(), 5);
	acq_data.app_params.NS = read_int_from_binary_file(ridat_file);
	acq_data.app_params.FW = read_float_from_binary_file(ridat_file);
	acq_data.app_params.PH1 = read_string_from_binary_file(ridat_file, 132);
	acq_data.app_params.PH2 = read_string_from_binary_file(ridat_file, 132);
	acq_data.app_params.PH3 = read_string_from_binary_file(ridat_file, 132);
	acq_data.app_params.PH4 = read_string_from_binary_file(ridat_file, 132);
	acq_data.app_params.PH5 = read_string_from_binary_file(ridat_file, 132);
	acq_data.app_params.RG = read_float_from_binary_file(ridat_file);
	acq_data.app_params.NECH = read_int_from_binary_file(ridat_file);
	acq_data.app_params.SW = read_double_from_binary_file(ridat_file);
	acq_data.app_params.DB = read_int_from_binary_file(ridat_file);
	acq_data.app_params.Bessel = read_double_from_binary_file(ridat_file);
	acq_data.app_params.Butterworth = read_double_from_binary_file(ridat_file);
	acq_data.app_params.SequenceName = read_string_from_binary_file(ridat_file, 32);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.RfAmps_ch0.data(), 6);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.RfAmps_ch1.data(), 6);
	acq_data.app_params.WW = read_float_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Counters.data(), 5);
	acq_data.app_params.GRead = read_int_from_binary_file(ridat_file);
	acq_data.app_params.GPhase = read_int_from_binary_file(ridat_file);
	acq_data.app_params.GSlice = read_int_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Gradients.data(), 9);
	acq_data.app_params.MAC1 = read_float_from_binary_file(ridat_file);
	acq_data.app_params.MAC2 = read_float_from_binary_file(ridat_file);
	acq_data.app_params.SH1 = read_string_from_binary_file(ridat_file, 20);
	acq_data.app_params.SH2 = read_string_from_binary_file(ridat_file, 20);
	acq_data.app_params.SH3 = read_string_from_binary_file(ridat_file, 20);
	acq_data.app_params.SH4 = read_string_from_binary_file(ridat_file, 20);
	acq_data.app_params.SH5 = read_string_from_binary_file(ridat_file, 20);
	acq_data.app_params.DS = read_int_from_binary_file(ridat_file);
	acq_data.app_params.NA = read_int_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.GradientIncrements.data(), 9);
	acq_data.app_params.DimX = read_int_from_binary_file(ridat_file);
	acq_data.app_params.DimY = read_int_from_binary_file(ridat_file);
	acq_data.app_params.DimZ = read_int_from_binary_file(ridat_file);
	acq_data.app_params.DimC = read_int_from_binary_file(ridat_file);
	acq_data.app_params.ImageEchos = read_int_from_binary_file(ridat_file);
	acq_data.app_params.ImageSlices = read_int_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Delays.data() + 5, 7);
	acq_data.app_params.GradPhase = read_string_from_binary_file(ridat_file, 4);
	acq_data.app_params.GradSlice = read_string_from_binary_file(ridat_file, 4);
	acq_data.app_params.GradRead = read_string_from_binary_file(ridat_file, 4);
	acq_data.app_params.TimePoints = read_int_from_binary_file(ridat_file);
	acq_data.app_params.SNR = read_int_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Counters.data() + 5, 7);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.FPs.data(), 5);
	acq_data.app_params.GREADX = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GREADY = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GREADZ = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GPHASEX = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GPHASEY = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GPHASEZ = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GSLICEX = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GSLICEY = read_float_from_binary_file(ridat_file);
	acq_data.app_params.GSLICEZ = read_float_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Delays.data() + 12, 20);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Counters.data() + 12, 20);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.Gradients.data() + 9, 23);
	read_data_from_binary_file_to_array(ridat_file, acq_data.app_params.MoreGains->data(), 9);
	const int AppEndMark = read_int_from_binary_file(ridat_file);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////// Processing Parameters
	ridat_file.seekg(Sect1Size + Sect2Size + Sect3Size);

	acq_data.proc_params.ProcFlags = read_int_from_binary_file(ridat_file);
	read_data_from_binary_file_to_array(ridat_file, acq_data.proc_params.ProcDummies.data(), 9);
	acq_data.proc_params.LB = read_float_from_binary_file(ridat_file);
	acq_data.proc_params.PA = read_float_from_binary_file(ridat_file);
	acq_data.proc_params.PB = read_float_from_binary_file(ridat_file);
	acq_data.proc_params.DP = read_float_from_binary_file(ridat_file);
	acq_data.proc_params.SMP = read_int_from_binary_file(ridat_file);
	acq_data.proc_params.PivotPoint = read_int_from_binary_file(ridat_file);
	acq_data.proc_params.NOBC = read_int_from_binary_file(ridat_file);
	acq_data.proc_params.PPRF = read_int_from_binary_file(ridat_file);
	acq_data.proc_params.PPTH = read_double_from_binary_file(ridat_file);
	acq_data.proc_params.PPBL = read_double_from_binary_file(ridat_file);
	acq_data.proc_params.PPAF = read_int_from_binary_file(ridat_file);
	acq_data.proc_params.INC2D = read_float_from_binary_file(ridat_file);
	acq_data.proc_params.SD2D = read_double_from_binary_file(ridat_file);

	const int ProcEndMark = read_int_from_binary_file(ridat_file);

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////// Reading Decay Data

	acq_data.time.clear();
	acq_data.real_amplitude.clear();
	acq_data.imag_amplitude.clear();

	ridat_file.seekg(Sect1Size + Sect2Size + Sect3Size + Sect4Size);

	while (ridat_file.good())
	{
		acq_data.real_amplitude.push_back((double)read_float_from_binary_file(ridat_file));
		acq_data.imag_amplitude.push_back((double)read_float_from_binary_file(ridat_file));
		acq_data.time.push_back(read_double_from_binary_file(ridat_file));
	}
	return 1;
}

