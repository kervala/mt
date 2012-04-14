#include "common.h"
#include "mt.h"

int main(int argc, char* argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	MT mt;

	std::vector<std::string> params;

	mt.parseEnv(params, "MT_OPTIONS");
	mt.parseCommandLine(params, argc, argv);

	if (!mt.parseParameters(params))
	{
		return 1;
	}

	return mt.processAction() ? 0:1;
}
