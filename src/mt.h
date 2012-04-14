#ifndef MT_H
#define MT_H

class MT
{
public:
	enum actions
	{
		eNone,
		eUsage,
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

	bool updateManifest(const std::string &output, const std::string &manifest, int id = 1);
	bool hasManifest(const std::string &file, int id = 1);

	bool processAction();

private:
	bool getFileContent(const std::string &filename, std::string &content);
	bool fixManifest(std::string &manifest);

	bool m_nologo;
	std::string m_manifest;
	std::string m_output;
	int m_id;
	FILE *m_log;
	actions m_action;
	bool m_verbose;
};

#endif
