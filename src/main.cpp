#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

void ErrorHandler(const char* error)
{
	std::cerr << error << std::endl;
}

int main(int argc, char* argv[])
{
	std::ofstream ofs("/tmp/mt.log", std::ios_base::app);

	if (argc < 2)
	{
		std::cerr << "Syntax: mt.exe -manifest <manifest> -outputresource:\"<executable>;1\"" << std::endl;
		return 1;
	}

	bool verbose = true;
	std::string manifest;
	std::string output;

	for(int i = 1; i < argc; ++i)
	{
		std::string param = argv[i];

		ofs << "Param " << i << ": " << param << std::endl;

		if (param == "-manifest" || param == "/manifest")
		{
			manifest = "1";
		}
		else if (param == "/nologo")
		{
			verbose = false;
		}
		else if (param.substr(0, 15) == "-outputresource" || param.substr(0, 15) == "/outputresource")
		{
			size_t j = 16;

			while(j < param.size() && (param[j] == ':' || param[j] == '"')) ++j;

			size_t k = j;

			while(k < param.size() && param[k] != ';' && param[k] != '"') ++k;

			output = param.substr(j, k-j);
		}
		else if (manifest == "1")
		{
			manifest = param;
		}
		else
		{
			std::cerr << "Parameter " << param << " not processed" << std::endl;
			return 1;
		}
	}

	if (verbose)
	{
		std::cout << "Opening manifest " << manifest << std::endl;
	}

	std::ifstream ifs(manifest.c_str(), std::ifstream::binary);

	if (!ifs.is_open())
	{
		std::cerr << "Can't open " << manifest << std::endl;
		return 1;
	}

	ifs.seekg(0, std::ios_base::end);
	size_t size = ifs.tellg();

	ifs.seekg(0, std::ios_base::beg);

	uint8_t *buffer = new uint8_t[size];

	ifs.read((char*)buffer, size);

	ifs.close();

	if (verbose)
	{
		std::cout << "Updating executable " << output << std::endl;
	}

	// Open the file to which you want to add the dialog box resource.
	HANDLE hUpdateRes = BeginUpdateResource(output.c_str(), FALSE);
	if (hUpdateRes == NULL)
	{
		ErrorHandler(TEXT("Could not open file for writing."));
		return 1;
	}

	// Add the dialog box resource to the update list.
	BOOL result = UpdateResource(hUpdateRes, RT_MANIFEST, CREATEPROCESS_MANIFEST_RESOURCE_ID, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), buffer, (DWORD)size);

	if (result == FALSE)
	{
		ErrorHandler(TEXT("Could not add resource."));
		return 1;
	}

	// Write changes to FOOT.EXE and then close it.
	if (!EndUpdateResource(hUpdateRes, FALSE))
	{
		ErrorHandler(TEXT("Could not write changes to file."));
		return 1;
	}

	return 0;
}
