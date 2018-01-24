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
