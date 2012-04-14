#include "common.h"
#include "mt.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#ifdef HAVE_REVISION_H
#include "revision.h"
#else
#define VERSION "1.0"
#define REVISION "0"
#define AUTHOR "Kervala"
#define YEAR "2012"
#endif
#endif

#ifdef DEBUG_NEW
#define new DEBUG_NEW
#endif

void split(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters){
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos){
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void join(const std::vector<std::string> &tokens, std::string &str, const std::string &delimiter){
	for(size_t i = 0; i < tokens.size(); ++i){
		if (!str.empty())
			str += delimiter;
		str += tokens[i];
	}
}

MT::MT():m_nologo(false), m_id(0), m_log(NULL), m_action(eNone), m_verbose(false)
{
}

MT::~MT()
{
	if (m_log)
	{
		fclose(m_log);
	}
}

bool MT::parseCommandLine(std::vector<std::string> &params, int argc, char* argv[])
{
	printHeader();

	if (argc < 2)
	{
		m_action = eUsage;
		return false;
	}

	for(int i = 1; i < argc; ++i)
	{
		params.push_back(argv[i]);
	}

	return true;
}

bool MT::parseEnv(std::vector<std::string> &params, const char *name)
{
	if (!getenv(name)) return false;

	std::string env = getenv(name);

	split(env, params, " ");

	return true;
}

bool MT::parseParameters(std::vector<std::string> &params)
{
	bool res = true;

	for(size_t i = 0; i < params.size(); ++i)
	{
		std::string param = params[i];
		std::string value;

		// switch
		if (param[0] == '-' || param[0] == '/')
		{
			param = param.substr(1);

			// in case where parameter is "--param"
			if (param[0] == '-') param = param.substr(1);

			std::string::size_type pos = param.find(':');

			if (pos != std::string::npos && pos+1 < param.size())
			{
				value = param.substr(pos+1);
				param = param.substr(0, pos);
			}

			if (param == "help" || param == "h" || param == "?")
			{
				m_action = eUsage;
			}
			else if (param == "manifest")
			{
				m_manifest = "1";
			}
			else if (param == "nologo")
			{
				m_nologo = true;
			}
			else if (param == "outputresource")
			{
				m_output = "1";
			}
			else if (param == "log")
			{
				m_log  = (FILE*)1;
			}
			else if (param == "verbose")
			{
				m_verbose  = true;
			}
			else
			{
				printError("Unexpected/Unknown option \"-%s\".  Use the /? option for help on usage and samples.", param.c_str());
				res = false;
			}
		}
		else if (param[0] == '@')
		{
			param = param.substr(1);

			std::string redirect;

			getFileContent(param, redirect);

			if (redirect[0] == (char)0xff && redirect[1] == (char)0xfe)
			{
				std::string fixed;

				for(size_t j = 2; j < redirect.size(); ++j)
				{
					if (redirect[j] == 0x0d || redirect[j] == 0x0a)
					{
						fixed += ' ';
					}
					else if (redirect[j] == 0x00)
					{
					}
					else
					{
						fixed += redirect[j];
					}
				}

				redirect = fixed;
			}

			split(redirect, params, " ");
		}
		else
		{
			value = param;
		}

		if (!value.empty())
		{
			if (m_manifest == "1")
			{
				m_manifest = value;

				m_action = eUpdateManifest;
			}
			else if (m_output == "1")
			{
				if (value[0] == '"' || value[0] == '\'') value = value.substr(1);

				std::string::size_type pos = value.find(';');

				if (pos != std::string::npos)
				{
					std::string id = value.substr(pos+1);
					value = value.substr(0, pos);

					if (id[0] == '#') id = id.substr(1);

					pos = id.find_first_not_of("0123456789");

					if (pos != std::string::npos) id = id.substr(0, pos);

					std::istringstream ss(id);

					ss >> m_id;
				}

				pos = value.length()-1;

				if (value[pos] == '"' || value[pos] == '\'') value = value.substr(0, pos);

				m_output = value;
			}
			else if (m_log == (FILE*)1)
			{
				m_log = fopen(value.c_str(), "a");

				if (!m_log)
				{
					printError("Unable to access to %s", value.c_str());
					res = false;
				}
			}
			else
			{
				printError("Unexpected/Unknown option \"%s\".  Use the /? option for help on usage and samples.", value.c_str());
				res = false;
			}
		}
	}

	if (m_verbose)
	{
		std::string res;
		join(params, res, " ");

		printDebug("mt.exe %s", res.c_str());
	}

	return res;
}

void MT::printHeader()
{
	printInfo("Kervala (R) Manifest Tool version %s (rev %s)", VERSION, REVISION);
	printInfo("Copyright (c) %s %s", AUTHOR, YEAR);
	printInfo("All rights reserved.");
	printInfo("");
}

void MT::printUsage()
{
	printInfo("Usage:");
	printInfo("-----");
	printInfo("mt.exe");
	printInfo("    [ -manifest <manifest1 name> <manifest2 name> ... ]");
	printInfo("    [ -identity:<identity string> ]");
	printInfo("    [ < <[-rgs:<.rgs filename>] [-tlb:<.tlb filename>]> -dll:<filename> > [ -replacements:<XML filename> ] ]");
	printInfo("    [ -managedassemblyname:<managed assembly> [ -nodependency ] [ -category ] ]");
	printInfo("    [ -out:<output manifest name> ]");
	printInfo("    [ -inputresource:<file>[;[#]<resource_id>] ]");
	printInfo("    [ -outputresource:<file>[;[#]<resource_id>] ]");
	printInfo("    [ -updateresource:<file>[;[#]<resource_id>] ]");
	printInfo("    [ -hashupdate[:<path to the files>] ]");
	printInfo("    [ -makecdfs ]");
	printInfo("    [ -validate_manifest ]");
	printInfo("    [ -validate_file_hashes:<path to the files> ]");
	printInfo("    [ -canonicalize ]");
	printInfo("    [ -check_for_duplicates ]");
	printInfo("    [ -nologo ]");
	printInfo("    [ -log:<file> ]");
	printInfo("    [ -debug ]");
	printInfo("");
	printInfo("Options:");
	printInfo("-------");
	printInfo("-manifest                 Used to specify manifests that need to be processed.");
	printInfo("                          At least one manifest name should follow this option.");
	printInfo("                          NOTE: There is no colon(:) after -manifest.");
	printInfo("");
	printInfo("<manifest1 name> <manifest2 name> ...");
	printInfo("                          Names of manifests to be processed and/or merged.");
	printInfo("                          Required if the -manifest option is used.");
	printInfo("                          NOTE: More than one manifest automatically indicates");
	printInfo("                          a manifest \"merge\" operation.  In that case, an");
	printInfo("                          output specified by one of -out / -outputresource /");
	printInfo("                          -updateresource is mandatory.");
	printInfo("");
	printInfo("-identity:<identity string>");
	printInfo("                           The identity string contains the attributes of the");
	printInfo("                           assemblyIdentity element.  The identity string is a");
	printInfo("                           set of comma separated name=value pairs starting");
	printInfo("                           with the \"name\" attribute's value.  e.g.:");
	printInfo("                           \"Microsoft.Windows.Common-Controls,");
	printInfo("                           processorArchitecture=x86, version=6.0.0.0,");
	printInfo("                           type=win32, publicKeyToken=6595b64144ccf1df\".");
	printInfo("                           NOTE: Only the \"name\" attribute is not of the form");
	printInfo("                           \"name=value\" and it should be the first attribute in");
	printInfo("                           the identity string.");
	printInfo("");
	printInfo("-rgs:                      Takes the name of the .RGS (Registrar script).");
	printInfo("");
	printInfo("-tlb:                      Takes the name of the .TLB (Typelib file).");
	printInfo("");
	printInfo("-dll:                      Takes the name of the DLL: this represents the DLL");
	printInfo("                           that is eventually built from the .RGS and .TLB");
	printInfo("                           files. Required if either -rgs or -tlb is specified.");
	printInfo("");
	printInfo("-replacements:<.XML filename>");
	printInfo("                           Specifies the file that contains values for");
	printInfo("                           replaceable strings in the RGS file.");
	printInfo("");
	printInfo("-managedassemblyname:<managed assembly> [ -nodependency ] [ -category ]");
	printInfo("                           Generates a manifest from a managed assembly.");
	printInfo("                           -nodependency suppresses the generation");
	printInfo("                           of dependency elements in the final manifest.");
	printInfo("                           -category causes the category tags to be");
	printInfo("                           generated.");
	printInfo("");
	printInfo("-out:<Output manifest name>");
	printInfo("                           Name of the output manifest.  If this is skipped");
	printInfo("                           and only one manifest is being operated upon by the");
	printInfo("                           tool, that manifest is modified in place.");
	printInfo("");
	printInfo("-inputresource:<file>[;[#]<resource_id>]");
	printInfo("                           Input the manifest from a resource of type");
	printInfo("                           RT_MANIFEST with the specified id.");
	printInfo("                           resource_id is restricted to be a non-negative,");
	printInfo("                           16 bit number.");
	printInfo("                           resource_id is optional and defaults to");
	printInfo("                           CREATEPROCESS_MANIFEST_RESOURCE_ID (winuser.h).");
	printInfo("");
	printInfo("-outputresource:<file>[;[#]<resource_id>]");
	printInfo("                           Output the manifest to a resource of type");
	printInfo("                           RT_MANIFEST with the specified id.");
	printInfo("                           resource_id is restricted to be a non-negative,");
	printInfo("                           16 bit number.");
	printInfo("                           resource_id is optional and defaults to");
	printInfo("                           CREATEPROCESS_MANIFEST_RESOURCE_ID (winuser.h).");
	printInfo("");
	printInfo("-updateresource:<file>[;[#]<resource_id>]");
	printInfo("                           Equivalent to specifying both -inputresource and");
	printInfo("                           -ouputresource with identical parameters.");
	printInfo("                           resource_id is restricted to be a non-negative,");
	printInfo("                           16 bit number.");
	printInfo("");
	printInfo("-hashupdate:<path to the files>");
	printInfo("                           Computes the hash of files specified in the file");
	printInfo("                           elements and updates the hash attribute with this");
	printInfo("                           value.  The searchpath for the actual files");
	printInfo("                           specified in the file elements is specified");
	printInfo("                           explicitly.  If <path to the files> is not");
	printInfo("                           specified, the searchpath defaults to the location");
	printInfo("                           of the output manifest.");
	printInfo("");
	printInfo("-makecdfs                  Generates Catalog Definition Files (.cdf) - used to");
	printInfo("                           make catalogs.");
	printInfo("");
	printInfo("-validate_manifest         Validates to check syntactic correctness of a");
	printInfo("                           manifest and its conformance to the manifest schema.");
	printInfo("");
	printInfo("-validate_file_hashes:<path to the files>");
	printInfo("                           Validates the hash values of all the file elements.");
	printInfo("");
	printInfo("-canonicalize              Does a C14N canonicalization of the output manifest");
	printInfo("                           contents.");
	printInfo("");
	printInfo("-check_for_duplicates      Performs a check to see if the final manifest");
	printInfo("                           contains duplicate elements.");
	printInfo("");
	printInfo("-nologo                    Runs without displaying standard Microsoft copyright");
	printInfo("                           data. This may be used to suppress unwanted output");
	printInfo("                           in log files when running mt.exe as part of a build");
	printInfo("                           process or from a build environment.");
	printInfo("");
	printInfo("-log:<file>                Logs all messages in file instead of displaying them.");
	printInfo("");
	printInfo("-debug                     Displays debugging messages.");
	printInfo("");
	printInfo("Samples:");
	printInfo("-------");
	printInfo("");
	printInfo("> To update the hash of an XML manifest:");
	printInfo("mt.exe -manifest 1.manifest -hashupdate -out:updated.manifest");
	printInfo("");
	printInfo("> To update the hash of an XML manifest while simultaneously producing the .cdf file:");
	printInfo("mt.exe -manifest 1.manifest -hashupdate -makecdfs -out:updated.manifest");
	printInfo("");
	printInfo("> To merge two manifests:");
	printInfo("mt.exe -manifest 1.manifest 2.manifest -out:merged.manifest");
	printInfo("");
	printInfo("> To merge two manifests and finally update the hash to produce the final merged manifest.");
	printInfo("> Note: The searchpath for the actual files specified in the file elements is specified explicitly.");
	printInfo("mt.exe -manifest 1.manifest 2.manifest -hashupdate:d:\filerepository -out:merged.manifest");
	printInfo("");
	printInfo("> To generate a manifest from an RGS and/or TLB file:");
	printInfo("mt.exe -rgs:MSClus.rgs -tlb:MSClus.tlb -dll:foo.dll -replacements:replacements.manifest -identity:\"type=win32, name=Microsoft.Tools.SampleAssembly, version=6.0.0.0, processorArchitecture=x86, publicKeyToken=6595b64144ccf1df\" -out:rgstlb.manifest");
	printInfo("");
	printInfo("> To generate an XML manifest from a managed assembly:");
	printInfo("mt.exe -managedassemblyname:managed.dll -out:out.manifest");
	printInfo("> To suppress dependencies:");
	printInfo("mt.exe -managedassemblyname:managed.dll -nodependency -out:out.manifest");
	printInfo("> To generate <category> elements:");
	printInfo("mt.exe -managedassemblyname:managed.dll -category -out:out.manifest");
	printInfo("");
	printInfo("> To extract manifest out of a dll:");
	printInfo("mt.exe -inputresource:dll_with_manifest.dll;#1 -out:extracted.manifest");
	printInfo("");
	printInfo("> To merge two manifests, one of them embedded in a dll, and embedding final merged manifest into another dll's resource:");
	printInfo("mt.exe -inputresource:dll_with_manifest.dll;#1 -manifest 2.manifest -outputresource:dll_with_merged_manifest.dll;#3");
	printInfo("");
	printInfo("> To update the manifest in a PE's resource (by updating the hashes of the file elements):");
	printInfo("mt.exe -updateresource:dll_with_manifest.dll;#1 -hashupdate:f:\files");
	printInfo("");
	printInfo("> To validate the hash values of all the file elements:");
	printInfo("mt.exe -manifest 1.manifest -validate_file_hashes:\"c:\files\"");
	printInfo("");
	printInfo("> To validate a manifest (i.e., to see if it conforms to the manifest schema):");
	printInfo("mt.exe -manifest 1.manifest -validate_manifest");
	printInfo("");
	printInfo("> To do a C14N canonicalization of a manifest (in order to get rid of spurious namespace prefixes (like \"dsig\")):");
	printInfo("mt.exe -manifest 1.manifest -canonicalize");
	printInfo("");
	printInfo("> To check for duplicate elements in a manifest:");
	printInfo("mt.exe -manifest 1.manifest -check_for_duplicates");
}

void MT::printInfo(const char *format, ...)
{
	if (m_nologo) return;

	static const char *footer = "\n";

	va_list vl;
	va_start(vl, format);

	size_t size = strlen(format) + strlen(footer) + 1;

	std::auto_ptr<char> buffer(new char[size]);
	*buffer = '\0';
	strcat(buffer.get(), format);
	strcat(buffer.get(), footer);

	vprintf(buffer.get(), vl);
	va_end(vl);
}

void MT::printError(const char *format, ...)
{
	if (m_nologo && m_log == NULL) return;

	static const char *header = "Error: ";
	static const char *footer = "\n";

	va_list vl;
	va_start(vl, format);

	size_t size = strlen(format) + strlen(header) + strlen(footer) + 1;

	std::auto_ptr<char> buffer(new char[size]);
	*buffer = '\0';
	strcat(buffer.get(), header);
	strcat(buffer.get(), format);
	strcat(buffer.get(), footer);

	if (m_log)
	{
		vfprintf(m_log, buffer.get(), vl);
	}
	else
	{
		vprintf(buffer.get(), vl);
	}

	va_end(vl);
}

void MT::printDebug(const char *format, ...)
{
	if (!m_verbose) return;

	static const char *header = "Debug: ";
	static const char *footer = "\n";

	va_list vl;
	va_start(vl, format);

	size_t size = strlen(format) + strlen(header) + strlen(footer) + 1;

	std::auto_ptr<char> buffer(new char[size]);
	*buffer = '\0';
	strcat(buffer.get(), header);
	strcat(buffer.get(), format);
	strcat(buffer.get(), footer);

	if (m_log)
	{
		vfprintf(m_log, buffer.get(), vl);
	}
	else
	{
		vprintf(buffer.get(), vl);
	}

	va_end(vl);
}

bool MT::updateManifest(const std::string &output, const std::string &manifest, int id)
{
	printDebug("Opening manifest %s", manifest.c_str());

	std::string buffer;
	
	if (!getFileContent(manifest, buffer)) return false;

	fixManifest(buffer);

	printDebug("Updating executable %s", output.c_str());

	// Open the file to which you want to add the dialog box resource.
	HANDLE hUpdateRes = BeginUpdateResource(output.c_str(), FALSE);

	if (hUpdateRes == NULL)
	{
		printError("Could not open file %s for writing.", output.c_str());
		return false;
	}

	// Add the manifest to the update list.
	BOOL result = UpdateResource(hUpdateRes, RT_MANIFEST, MAKEINTRESOURCE(id), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPVOID)buffer.c_str(), (DWORD)buffer.length());

	if (result == FALSE)
	{
		printError("Could not add resource %d to %s.", id, output.c_str());
		return false;
	}

	// Write changes to file and then close it.
	if (!EndUpdateResource(hUpdateRes, FALSE))
	{
		printError("Could not write changes to file.");
		return false;
	}

	return true;
}

bool MT::hasManifest(const std::string &file, int id)
{
	bool res = false;

	// Load the .EXE file
	HMODULE hExe = LoadLibrary(file.c_str());

	if (hExe == NULL)
	{
		printError("Could not load %s", file.c_str());
		return res;
	}

	// Locate the manifest resource in the .EXE file
	HRSRC hRes = FindResourceExA(hExe, RT_MANIFEST, MAKEINTRESOURCE(id), MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

	if (hRes == NULL)
	{
		printDebug("Could not locate manifest %d with error %u.", id, GetLastError());
	}
	else
	{
		printDebug("Found manifest %d in %s", id, file.c_str());
		res = true;
	}

	// Clean up
	if (!FreeLibrary(hExe))
	{
		printError("Could not free executable.");
	}

	return res;
}

bool MT::processAction()
{
	switch(m_action)
	{
		case eNone:
		return true;

		case eUsage:
		printUsage();
		return true;

		case eUpdateManifest:
		{
			bool before = hasManifest(m_output, m_id);
			bool res = updateManifest(m_output, m_manifest, m_id ? m_id:1);
			bool after = hasManifest(m_output, m_id);
			return res;
		}

		default:
		break;
	}

	return false;
}

bool MT::getFileContent(const std::string &filename, std::string &content)
{
	std::ifstream ifs(filename.c_str(), std::ifstream::binary);

	if (!ifs.is_open())
	{
		printError("Can't open %s", filename.c_str());
		return false;
	}

	ifs.seekg(0, std::ios_base::end);
	size_t size = (size_t)ifs.tellg();

	ifs.seekg(0, std::ios_base::beg);

	std::auto_ptr<char> buffer(new char[size]);

	ifs.read(buffer.get(), size);

	ifs.close();

	content.assign(buffer.get(), size);

	return true;
}

bool MT::fixManifest(std::string &manifest)
{
	std::string::size_type pos;

	// Remove XML header
	if (manifest.substr(0, 5) == "<?xml")
	{
		pos = manifest.find_first_of("\n\r");
		pos = manifest.find_first_not_of("\n\r", pos);

		manifest = manifest.substr(pos);
	}

	// Remove spaces at the end
	pos = manifest.find_last_not_of("\n\r \t");

	if (pos != std::string::npos)
	{
		manifest = manifest.substr(0, pos+1);
	}

	// Replace single by double quotes
	for(size_t i = 0; i < manifest.size(); ++i)
	{
		if (manifest[i] == '\'') manifest[i] = '"';
	}

	// TODO: replace <tag /> by <tag></tag>

	return true;
}
