#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>

#include "TRandom2.h"

using namespace std;

// z positions of sensors wrt. RP centre, in mm
double det_rel_z[10] = { -20.3, -15.7, -11.3, -6.7, -2.3, 2.3, 6.7, 11.3, 15.7, 20.3 };

//----------------------------------------------------------------------------------------------------

void GenerateOneRPTrueAlignment(unsigned int rp_id, FILE *f)
{
	double si_rp_rot_xy = 6E-3;		// rad
	double si_rp_rot_z = 5E-3;		// rad

	double si_det_sh_xy = 20E-3;	// mm
	double si_det_rot_z = 1E-3;		// rad
	
	//--------------------
	
	double rp_rot_x = si_rp_rot_xy * gRandom->Gaus();
	double rp_rot_y = si_rp_rot_xy * gRandom->Gaus();
	double rp_rot_z = si_rp_rot_z * gRandom->Gaus();

	fprintf(f, "\n");
	fprintf(f, "\t<rp id=\"%3u\" rot_z=\"%+6.1f\" />\n", rp_id, rp_rot_z*1E3);

	for (unsigned int i = 0; i < 10; i++)
	{
		unsigned det_id = 10*rp_id + i;

		double sh_x = rp_rot_x * det_rel_z[i]  +  si_det_sh_xy * gRandom->Gaus();
		double sh_y = rp_rot_y * det_rel_z[i]  +  si_det_sh_xy * gRandom->Gaus();
		double rot_z = si_det_rot_z * gRandom->Gaus();

		fprintf(f, "\t\t<det id=\"%3u\" sh_x=\"%+6.1f\" sh_y=\"%+6.1f\" rot_z=\"%+6.1f\" />\n", det_id, sh_x*1E3, sh_y*1E3, rot_z*1E3);
	}
}

//----------------------------------------------------------------------------------------------------

void GenerateOneRPAlignmentDeterminationError(unsigned int rp_id, FILE *f)
{
	double si_rp_sh_x = 20E-3;		// mm
	double si_rp_sh_y = 20E-3;		// mm
	double si_rp_rot_z = 0.2E-3;	// rad

	double si_det_sh_xy = 2E-3;		// mm
	double si_det_rot_z = 0.2E-3;	// rad
	
	//--------------------
	
	double rp_sh_x = si_rp_sh_x * gRandom->Gaus();
	double rp_sh_y = si_rp_sh_y * gRandom->Gaus();
	double rp_rot_z = si_rp_rot_z * gRandom->Gaus();

	fprintf(f, "\n");
	fprintf(f, "\t<rp id=\"%3u\" sh_x=\"%+6.1f\" sh_y=\"%+6.1f\" rot_z=\"%+6.1f\" />\n", rp_id, rp_sh_x*1E3, rp_sh_y*1E3, rp_rot_z*1E3);

	for (unsigned int i = 0; i < 10; i++)
	{
		unsigned det_id = 10*rp_id + i;

		double sh_x = si_det_sh_xy * gRandom->Gaus();
		double sh_y = si_det_sh_xy * gRandom->Gaus();
		double rot_z = si_det_rot_z * gRandom->Gaus();

		fprintf(f, "\t\t<det id=\"%3u\" sh_x=\"%+6.1f\" sh_y=\"%+6.1f\" rot_z=\"%+6.1f\" />\n", det_id, sh_x*1E3, sh_y*1E3, rot_z*1E3);
	}
}

//----------------------------------------------------------------------------------------------------

void PrintUsage()
{
	printf("USAGE: generate_misalignments <option> <option> ...\n");
	printf("OPTIONS:\n");
	printf("\t-seed <number>\trandom seed to use\n");
	printf("\t-rps <string>\tcomma-separated list of RPs to use\n");
	printf("\t-output <label>\tlabel for output files\n");
}

//----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	// parse command line
	string seed_str="1";
	string outputLabel = "output";
	string rps_str="";

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-seed") == 0)
		{
			if (argc-1 > i)
				seed_str = argv[++i];
			continue;
		}
		
		if (strcmp(argv[i], "-output") == 0)
		{
			if (argc-1 > i)
				outputLabel = argv[++i];
			continue;
		}
		
		if (strcmp(argv[i], "-rps") == 0)
		{
			if (argc-1 > i)
				rps_str = argv[++i];
			continue;
		}

		printf("ERROR: unknown parameter `%s'.\n", argv[i]);
		PrintUsage();
		return 1;
	}

	// set parameters
	unsigned int seed = atoi(seed_str.c_str());
	gRandom->SetSeed(seed);

	vector<unsigned int> RPs;
	char buf[100];
	strcpy(buf, rps_str.c_str());

	char *pch = strtok(buf, ",");
	while (pch != NULL)
	{
		RPs.push_back(atof(pch));
		pch = strtok(NULL, ",");
	}

	// real/true/actual alignment
	FILE *f_meas = fopen((outputLabel+"_true.xml").c_str(), "w");
	fprintf(f_meas, "<xml DocumentType=\"AlignmentDescription\">\n");

	for (unsigned int i = 0; i < RPs.size(); i++)
		GenerateOneRPTrueAlignment(RPs[i], f_meas);

	fprintf(f_meas, "</xml>\n");
	fclose(f_meas);

	// the error made when determining the true alignment
	FILE *f_real = fopen((outputLabel+"_determination_error.xml").c_str(), "w");
	fprintf(f_real, "<xml DocumentType=\"AlignmentDescription\">\n");

	for (unsigned int i = 0; i < RPs.size(); i++)
		GenerateOneRPAlignmentDeterminationError(RPs[i], f_real);

	fprintf(f_real, "</xml>\n");
	fclose(f_real);

	return 0;
}
