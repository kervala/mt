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

#include <stdio.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		printf("Not enough parameters\n");
		return 1;
	}

	if (strcmp(argv[1], "os") == 0)
	{
		if (strcmp(argv[2], "get") == 0)
		{
			if (strcmp(argv[3], "localdatetime") == 0)
			{
				time_t rawtime;
				struct tm* timeinfo;
				char buffer[40];

				time(&rawtime);
				timeinfo = localtime(&rawtime);

				strftime(buffer, 40, "%Y%m%d%H%M%S.000000+000", timeinfo);

				printf("LocalDateTime\n%s\n", buffer);
			}
			else
			{
				printf("Command %s not implemented\n", argv[3]);
				return 1;
			}
		}
		else
		{
			printf("Command %s not implemented\n", argv[2]);
			return 1;
		}
	}
	else
	{
		printf("Command %s not implemented\n", argv[1]);
		return 1;
	}

	return 0;
}
