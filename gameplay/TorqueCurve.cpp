/*
This source file is part of Rigs of Rods
Copyright 2005-2012 Pierre-Michel Ricordel
Copyright 2007-2012 Thomas Fischer

For more information, see http://www.rigsofrods.com/

Rigs of Rods is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3, as 
published by the Free Software Foundation.

Rigs of Rods is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rigs of Rods.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "TorqueCurve.h"
#include "Ogre.h"

using namespace Ogre;
using namespace std;

const String TorqueCurve::customModel = "CustomModel";

TorqueCurve::TorqueCurve() : usedSpline(0), usedModel("")
{
	loadDefaultTorqueModels();
	setTorqueModel("default");
}

TorqueCurve::~TorqueCurve()
{
	splines.clear();
}

Real TorqueCurve::getEngineTorque(Real rpmRatio)
{
	if(!usedSpline) return 1.0f; //return a good value upon error?
	return usedSpline->interpolate(rpmRatio).y;
}

int TorqueCurve::loadDefaultTorqueModels()
{
	//LOG("loading default torque Curves");
	// check if we have a config file
	String group = "";
	try
	{
		group = ResourceGroupManager::getSingleton().findGroupContainingResource("torque_models.cfg");
	} catch(...)
	{
	}
	// emit a warning if we did not found the file
	if (group.empty())
	{
		LOG("torque_models.cfg not found");
		return 1;
	}

	// open the file for reading
	DataStreamPtr ds = ResourceGroupManager::getSingleton().openResource("torque_models.cfg", group);
	String line = "";
	String currentModel = "";

	while (!ds->eof())
	{
		line=ds->getLine();
		StringUtil::trim(line);

		if (line.empty() || line[0]==';')
			continue;

		Ogre::StringVector args = StringUtil::split(line, ",");

		if (args.size() == 1)
		{
			currentModel = line;
			continue;
		}

		// process the line if we got a model
		if(!currentModel.empty())
			processLine(args, currentModel);
	}
	return 0;
}

int TorqueCurve::processLine(String line, String model)
{
	return processLine(StringUtil::split(line, ","), model);
}

int TorqueCurve::processLine(Ogre::StringVector args, String model)
{
	// if its just one arguments, it must be a known model
	if (args.size() == 1)
		return setTorqueModel(args[0]);

	// we only accept 2 arguments
	if (args.size() != 2)
		return 1;
	// parse the data
	float pointx = StringConverter::parseReal(args[0]);
	float pointy = StringConverter::parseReal(args[1]);
	Vector3 point = Vector3(pointx,pointy,0);

	// find the spline to attach the points
	if(splines.find(model) == splines.end())
		splines[model] = SimpleSpline();

	// attach the points to the spline
	// LOG("curve "+model+" : " + TOSTRING(point));
	splines[model].addPoint(point);

	// special case for custom model:
	// we set it as active curve as well!
	if(model == TorqueCurve::customModel)
		setTorqueModel(TorqueCurve::customModel);

	return 0;
}

int TorqueCurve::setTorqueModel(String name)
{
	//LOG("using torque curve: " + name);
	// check if we have such a model loaded
	if (splines.find(name) == splines.end())
	{
		LOG("Torquemodel "+String(name)+" not found! ignoring that and using default model...");
		return 1;
	}
	// use the model
	usedSpline = &splines.find(name)->second;
	usedModel = name;
	return 0;
}

int TorqueCurve::spaceCurveEvenly(Ogre::SimpleSpline *spline)
{
	if(!spline) return 0;
	SimpleSpline tmpSpline=*spline;
	Real points=tmpSpline.getNumPoints();
	if (points>1)
	{
		//clear the original spline, so it's prepared for the new corrected numbers
		spline->clear();
		Real minDistance=10000000.0f;
		//looking for the mininimum distance (spacing) in the current spline
		for(int i=0;i<points-1;i++)
		{
			// the rpms must be in an ascending order, as the points should be added at the end of the spline
			if (tmpSpline.getPoint(i+1).x<tmpSpline.getPoint(i).x) return 1;
			Real distance=tmpSpline.getPoint(i+1).x-tmpSpline.getPoint(i).x;
			if (distance<minDistance) minDistance=distance;
		}
		//first(smallest), and last rpm
		Vector3 minPoint=tmpSpline.getPoint(0);
		Vector3 maxPoint=tmpSpline.getPoint(points-1);

		Real rpmPoint=minPoint.x;
		int PointNum=1; //this is the index used to interpolate between the rpm points
		while (rpmPoint<=maxPoint.x && PointNum<points)
		{
			//if actual rpm is higher than point of the spline, proceed to the next point
			if(rpmPoint>tmpSpline.getPoint(PointNum).x) PointNum++;
			//interpolate(linear)
			Real newPoint=tmpSpline.getPoint(PointNum-1).y+(tmpSpline.getPoint(PointNum).y-tmpSpline.getPoint(PointNum-1).y)/
				(tmpSpline.getPoint(PointNum).x-tmpSpline.getPoint(PointNum-1).x)*(rpmPoint-tmpSpline.getPoint(PointNum-1).x);
			spline->addPoint(Vector3(rpmPoint,newPoint,0));
			rpmPoint+=minDistance;
		}
		//if the last point is missing due the even spacing, we add the last point manually
		//criterium is that it must be smaller than 1% of the maximum rpm.
		if(spline->getPoint(spline->getNumPoints()-1).x<maxPoint.x && (rpmPoint-maxPoint.x)<0.01*maxPoint.x) spline->addPoint(Vector3(rpmPoint,maxPoint.y,0));
	}
	return 0;
}
