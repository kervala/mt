/*
 *  Microsoft Manifest Tool (mt.exe) clone for Wine
 *  Copyright (C) 2012-2018  Cedric OCHS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MT_H
#define MT_H

class MT
{
public:
	enum actions
	{
		eNone,
		eUsage,
		eFixManifest,
		eUpdateManifest
	};

	MT();
	virtual ~MT();

	bool parseCommandLine(std::vector<std::string> &params, int argc, char* argv[]);
	bool parseEnv(std::vector<std::string> &params, const char *name);
	bool parseParameters(std::vector<std::string> &params);

	void printHeader();
	void printUsage();

	void printInfo(const char *format, ...);
	void printError(const char *format, ...);
	void printDebug(const char *format, ...);

	bool fixManifest(const std::string &ouput, const std::string &input);
	bool updateManifest(const std::string &output, const std::string &manifest, int id = 1);
	bool hasManifest(const std::string &file, int id = 1);

	bool processAction();

private:
	bool getFileContent(const std::string &filename, std::string &content);
	bool setFileContent(const std::string &filename, const std::string &content);
	bool fixManifest(std::string &manifest, bool stripheader);
	bool fixPEHeader(const std::string &filename);

	bool m_nologo;
	std::string m_manifest;
	std::string m_output;
	int m_id;
	FILE *m_log;
	actions m_action;
	bool m_verbose;
};

#endif
