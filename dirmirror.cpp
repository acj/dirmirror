/*
 * dirmirror: A program to mirror one directory structure to another location.
 *
 * Copyright (c) 2006-2007 Adam Jensen <ajensen@linuxguy.org>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <string>
#include <iostream>
#include <queue>

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cout << "Usage: " << argv[0] << " <source> <destination>" << endl;
		exit(1);
	}

	cout << "Mirroring " << argv[1] << " to " << argv[2] << endl; 

	string source_start  = argv[1];
	string target_start = argv[2];

	queue<string> dirs;
	DIR *current_dir;
	struct dirent64 *current_dirp;

	/* 
	 * Push the initial directory onto the stack.  This path is relative to
	 * source_start.
	 */ 
	dirs.push("");

	string next_dir;
	string full_path;

	// Verify that the target directory exists; if it doesn't, create it
	DIR *target_check = opendir(target_start.c_str());

	if (target_check == 0)
	{
		int target_make = mkdir(target_start.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);

		if (target_make == -1)
		{
			cerr << "Could not create target directory: ";
			perror("mkdir");
		}
	}

	while (!dirs.empty()) 
	{
		next_dir = dirs.front();

		dirs.pop();

		full_path = source_start + next_dir;

		// Don't recurse - this will lead to an infinite loop.
		if (full_path == target_start)
		{
			continue;
		}

		current_dir = opendir(full_path.c_str());

		if (current_dir == NULL)
		{
			cout << "Couldn't open directory " << source_start+next_dir << endl;
			continue;
		}
		else
		{
			while ((current_dirp = readdir64(current_dir)) != NULL)
			{
				if (current_dirp->d_type == DT_DIR
					&& strcmp(current_dirp->d_name, ".") != 0
					&& strcmp(current_dirp->d_name, "..") != 0)
				{
					string push_dir = "";
					
					push_dir  = next_dir + "/" + current_dirp->d_name;

					string make_dir = target_start + push_dir; 
					dirs.push(push_dir);
					
					if (mkdir(make_dir.c_str(), 
						(S_IRUSR | S_IWUSR | S_IXUSR)) == -1)
					{
						cout << "E " 
							 << make_dir
							 << "(" << dirs.size() << ")" 
							 << endl;
					}
					else
					{
						cout << "+ " 
							 << push_dir 
							 << "(" << dirs.size() << ")" 
							 << endl;
					}
				}
			}

			closedir(current_dir);
		}
	}

	return 0;
}
