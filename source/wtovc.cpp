// wtovc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <vector>


struct WorldEntry {
	std::string name;
	float x, y, z;
	float scalex, scaley, scalez;
	float rotx, roty, rotz, rotw;
	int bIsInvisible;
};

// type 1
struct IDEEntry {
	int ID;
	std::string ModelName;
	std::string TxdName;
	int MeshCount;
	int DrawDistance;
	int Flags;

};

struct IPLEntry {
	int ID;
	std::string ModelName;
	int Interior;
	float x, y, z;
	float scalex, scaley, scalez;
	float rotx, roty, rotz, rotw;
};

std::string splitString(std::string& str, bool file) {

	std::string str_ret;
	int fnd = str.find_last_of("/\\");

	if (file == true) str_ret = str.substr(fnd + 1);
	if (file == false) str_ret = str.substr(0, fnd);
	return str_ret;

}



std::vector<IPLEntry> vIPLData;
std::vector<IDEEntry> vIDEData;

int GetIDFromModelName(std::string input)
{
	int iFind = 0;
	for (int i = 0; i < vIDEData.size(); i++)
	{
		if (vIDEData[i].ModelName == input)
		{
			iFind = i;
			break;
		}
	}
	return iFind;
}



int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		std::cout << "wtovc - Convert Trucker world files to GTA Vice City ipl/ide\n"
			"Usage: wtovc <input> <ide> <ipl>\n";
		return 1;
	}

	FILE* pFile = fopen(argv[1], "rb");

	if (!pFile)
	{
		std::cout << "ERROR: Failed to open " << argv[1] << "!" << std::endl;
		return 1;
	}

	if (pFile)
	{
		std::vector<WorldEntry> WorldData;
		char szLine[1024];
		char elementName[256];
		WorldEntry newWRLDItem;
		while (fgets(szLine, sizeof(szLine), pFile))
		{
			if (szLine[0] == '\n') continue;
			
			if (sscanf(szLine, "%s", &elementName) == 1)
			{
				if (strcmp(elementName, "mesh") == 0)
				{
					char name[256];
					sscanf(szLine + strlen("mesh"), "%s", &name);
					std::string strName(name, strlen(name));
					newWRLDItem.name = name;
				}
				if (strcmp(elementName, "xpos") == 0)
				{
					float value;
					sscanf(szLine + strlen("xpos"), "%f", &value);
					newWRLDItem.x = value;
				}
				if (strcmp(elementName, "ypos") == 0)
				{
					float value;
					sscanf(szLine + strlen("ypos"), "%f", &value);
					newWRLDItem.y = value;
				}
				if (strcmp(elementName, "zpos") == 0)
				{
					float value;
					sscanf(szLine + strlen("zpos"), "%f", &value);
					newWRLDItem.z = value;

				}
				if (strcmp(elementName, "rotation_angles") == 0)
				{
					float x,y,z;
					sscanf(szLine + strlen("rotation_angles"), "%f %f %f", &x, &y, &z);
					newWRLDItem.rotx = x;
					newWRLDItem.roty = y;
					newWRLDItem.rotz = z;
					newWRLDItem.rotw = 0.0;

				}
				if (strcmp(elementName, "scaling_axis") == 0)
				{
					float x, y, z;
					sscanf(szLine + strlen("scaling_axis"), "%f %f %f", &x, &y, &z);
					newWRLDItem.scalex = x;
					newWRLDItem.scaley = y;
					newWRLDItem.scalez = z;
				}
				if (strcmp(elementName, "invisible_in_game") == 0)
				{
					int result;
					sscanf(szLine + strlen("invisible_in_game"), "%d", &result);
					newWRLDItem.bIsInvisible = result;
					// push at last element
					WorldData.push_back(newWRLDItem);
				}
			}	
		}
		std::cout << "INFO: Loaded " << WorldData.size() << " objects!" << std::endl;
		if (!argv[2])
		{
			std::cout << "ERROR: IDE file was not specified!" << std::endl;
			return 1;
		}

		int id = 0;

		for (int i = 0; i < WorldData.size(); i++)
		{
			if (!WorldData[i].bIsInvisible)
			{
				IDEEntry ide;
				ide.ID = id;
				ide.ModelName = WorldData[i].name;
				ide.TxdName = "truckertex";
				ide.MeshCount = 1;
				ide.DrawDistance = 127;
				ide.Flags = 0;
				vIDEData.push_back(ide);
				id++;
			}
		}

		// write ide file

		FILE* ideFile = fopen(argv[2], "w");

		fprintf(ideFile, "objs\n");

		for (int i = 0; i < vIDEData.size(); i++)
		{
			std::string name = splitString(vIDEData[i].ModelName, true);
			fprintf(ideFile, "%d,%s,%s,%d,%d,%d\n", vIDEData[i].ID, name.substr(0,name.length() - 2).c_str(), vIDEData[i].TxdName.c_str(), vIDEData[i].MeshCount, vIDEData[i].DrawDistance, vIDEData[i].Flags);
		}
		fprintf(ideFile, "end\n");
		fclose(ideFile);

		std::cout << "INFO: IDE file created: " << argv[2] << std::endl;

		if (!argv[3])
		{
			std::cout << "ERROR: IPL file was not specified!" << std::endl;
			return 1;
		}


		for (int i = 0; i < WorldData.size(); i++)
		{
			if (!WorldData[i].bIsInvisible)
			{
				IPLEntry ipl;
				int iFind = GetIDFromModelName(WorldData[i].name);
				std::string name = splitString(vIDEData[iFind].ModelName, true);
				ipl.ID = vIDEData[iFind].ID;
				ipl.ModelName = name.substr(0, name.length() - 2).c_str();
				ipl.Interior = 0;
				ipl.x = WorldData[i].x;
				ipl.y = WorldData[i].y;
				ipl.z = WorldData[i].z;
				ipl.scalex = WorldData[i].scalex;
				ipl.scaley = WorldData[i].scaley;
				ipl.scalez = WorldData[i].scalez;
				ipl.rotx = WorldData[i].rotx;
				ipl.roty = WorldData[i].roty;
				ipl.rotz = WorldData[i].rotz;
				ipl.rotw = WorldData[i].rotw;
				vIPLData.push_back(ipl);
			}

		}

		// write ipl file
		FILE* iplFile = fopen(argv[3], "w");

		fprintf(iplFile, "inst\n");
		for (int i = 0; i < vIPLData.size(); i++)
		{
			fprintf(iplFile, "%d,%s,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", vIPLData[i].ID,
				vIPLData[i].ModelName.c_str(),
				vIPLData[i].Interior,
				vIPLData[i].x,
				vIPLData[i].y,
				vIPLData[i].z,
				vIPLData[i].scalex,
				vIPLData[i].scaley,
				vIPLData[i].scalez,
				vIPLData[i].rotx,
				vIPLData[i].roty,
				vIPLData[i].rotz,
				vIPLData[i].rotx);
		}
		fprintf(iplFile, "end\n");
		fclose(iplFile);

		std::cout << "INFO: IPL file created: " << argv[3] << std::endl;
	}
    return 0;
}

