#include "MexCommand.h"
#include "MexFunctions.h"
#include "..\Global\Globals.h"

void MexSetBackgroundColor::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	double* bc = (double*)mxGetData(prhs[0]);
	Vec<float>* background = new Vec<float>;
	background->x = (float)(bc[0]);
	background->y = (float)(bc[1]);
	background->z = (float)(bc[2]);

	gDataQueue->writeMessage("setBackgroundColor", (void*)background);
}

std::string MexSetBackgroundColor::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if (nrhs != 1)
		return "There must be one input consisting of three doubles between [0,1]!";

	return "";
}

void MexSetBackgroundColor::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	//inArgs.push_back("arrowFaces");
}

void MexSetBackgroundColor::help(std::vector<std::string>& helpLines) const
{
	//helpLines.push_back("This initializes the D3d viewing window.  It takes a widget to show orientation. The ARROW will be the polygons that point down each axis.  The SPHERE is the center polygon that is centered at the (widget's) axis origin.");

	//helpLines.push_back("\tArrowFaces -- This is an ordered list of vertices that make up the facets (triangles) for each axis.");
}