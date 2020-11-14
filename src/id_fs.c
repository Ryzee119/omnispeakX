/*
Omnispeak: A Commander Keen Reimplementation
Copyright (C) 2020 Omnispeak Authors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ck_cross.h"
#include "ck_ep.h"

#include "id_fs.h"
#include "id_us.h"

const char *fs_keenPath;
const char *fs_omniPath;
const char *fs_userPath;

size_t FS_Read(void *ptr, size_t size, size_t nmemb, FS_File file)
{
	return fread(ptr, size, nmemb, file);
}

size_t FS_Write(void *ptr, size_t size, size_t nmemb, FS_File file)
{
	return fwrite(ptr, size, nmemb, file);
}

size_t FS_SeekTo(FS_File file, size_t offset)
{
	size_t oldOff = ftell(file);
	fseek(file, offset, SEEK_SET);
	return oldOff;
}

void FS_CloseFile(FS_File file)
{
	fclose(file);
}

#ifdef __linux__
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

FS_File FSL_OpenFileInDirCaseInsensitive(const char *dirPath, const char *fileName, bool forWrite)
{
	int dirFd = open(dirPath, O_RDONLY | O_DIRECTORY);
	DIR *currentDirPtr = fdopendir(dirFd);

	// Search the current directory for matching names.
	for (struct dirent *dirEntry = readdir(currentDirPtr); dirEntry; dirEntry = readdir(currentDirPtr))
	{
		if (!CK_Cross_strcasecmp(dirEntry->d_name, fileName))
		{
			// We've found our file!
			if (forWrite)
			{
				int fd = openat(dirFd, dirEntry->d_name, O_WRONLY | O_TRUNC);
				close(dirFd);
				return fdopen(fd, "wb");
			}
			else
			{
				int fd = openat(dirFd, dirEntry->d_name, O_RDONLY);
				close(dirFd);
				return fdopen(fd, "rb");
			}
		}
	}
	return 0;
}

FS_File FSL_CreateFileInDir(const char *dirPath, const char *fileName)
{
	int dirFd = open(dirPath, O_PATH | O_DIRECTORY);

	int fd = openat(dirFd, fileName, O_CREAT | O_WRONLY, 0664);
	close(dirFd);
	return fdopen(fd, "wb");
}

size_t FS_GetFileSize(FS_File file)
{
	struct stat fileStat;
	if (fstat(fileno(file), &fileStat))
		return 0;

	printf("FS_GetFileSize(%p): fileno = %d, size = %d\n", file, fileno(file), fileStat.st_size);
	return fileStat.st_size;
}

#elif _WIN32
#define WIN32_MEAN_AND_LEAN
#undef UNICODE
#include <io.h>
#include <windows.h>
FS_File FSL_OpenFileInDirCaseInsensitive(const char *dirPath, const char *fileName, bool forWrite)
{
	// TODO: We really should scan through the path on windows anyway, as that'll
	// make sure there isn't any nasty path manipulation shenanigans going on.
	char fullFileName[MAX_PATH];
	sprintf(fullFileName, "%s\\%s", dirPath, fileName);

	return fopen(fullFileName, forWrite ? "wb" : "rb");
}

FS_File FSL_CreateFileInDir(const char *dirPath, const char *fileName)
{
	// TODO: We really should scan through the path on windows anyway, as that'll
	// make sure there isn't any nasty path manipulation shenanigans going on.
	char fullFileName[MAX_PATH];
	sprintf(fullFileName, "%s\\%s", dirPath, fileName);

	return fopen(fullFileName, "wbx");
}

size_t FS_GetFileSize(FS_File file)
{
	HANDLE fHandle = (HANDLE)_get_osfhandle(_fileno(file));

	// NOTE: size_t is 32-bit on win32 (and none of Keen's files should be big),
	// so we just use the low 32-bits of the filesize here. This stops the
	// annoying compiler warning we'd otherwise get.
	return GetFileSize(fHandle, NULL);
}

#else

#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>

FS_File FSL_OpenFileInDirCaseInsensitive(const char *dirPath, const char *fileName, bool forWrite)
{
	DIR *currentDirPtr = opendir(dirPath);

	// Search the current directory for matching names.
	for (struct dirent *dirEntry = readdir(currentDirPtr); dirEntry; dirEntry = readdir(currentDirPtr))
	{
		if (!CK_Cross_strcasecmp(dirEntry->d_name, fileName))
		{
			// We've found our file!
			char *fullFileName = (char *)malloc(strlen(dirPath) + 1 + strlen(dirEntry->d_name));
			sprintf(fullFileName, "%s/%s", dirPath, dirEntry->d_name);

			FS_File f;
			if (forWrite)
				f = fopen(fullFileName, "wb");
			else
				f = fopen(fullFileName, "rb");

			free(fullFileName);
			return f;
		}
	}
	return 0;
}

FS_File FSL_CreateFileInDir(const char *dirPath, const char *fileName)
{
	char *fullFileName = (char *)malloc(strlen(dirPath) + 1 + strlen(fileName));
	sprintf(fullFileName, "%s/%s", dirPath, fileName);

	FS_File f = fopen(fullFileName, "wb");
	free(fullFileName);
	return f;
}

size_t FS_GetFileSize(FS_File file)
{
	long int oldPos = ftell(file);
	fseek(file, 0, SEEK_END);
	long int fileSize = ftell(file);
	fseek(file, oldPos, SEEK_SET);
	return fileSize;
}

#endif

FS_File FS_OpenKeenFile(const char *fileName)
{
	return FSL_OpenFileInDirCaseInsensitive(fs_keenPath, fileName, false);
}

FS_File FS_OpenOmniFile(const char *fileName)
{
	// We should look for Omnispeak files (headers, actions, etc) in the
	// Keen drictory first, in case we're dealing with a game which has
	// them (e.g., a mod)
	FS_File file = FSL_OpenFileInDirCaseInsensitive(fs_keenPath, fileName, false);
	if (file)
		return file;
	return FSL_OpenFileInDirCaseInsensitive(fs_omniPath, fileName, false);
}

FS_File FS_OpenUserFile(const char *fileName)
{
	return FSL_OpenFileInDirCaseInsensitive(fs_userPath, fileName, false);
}

FS_File FS_CreateUserFile(const char *fileName)
{
	FS_File file = FSL_OpenFileInDirCaseInsensitive(fs_userPath, fileName, true);

	if (!file)
		file = FSL_CreateFileInDir(fs_userPath, fileName);

	return file;
}

// Does a file exist (and is it readable)
bool FS_IsKeenFilePresent(const char *filename)
{
	FS_File file = FS_OpenKeenFile(filename);
	if (!file)
		return false;
	FS_CloseFile(file);
	return true;
}

// Does a file exist (and is it readable)
bool FS_IsOmniFilePresent(const char *filename)
{
	FS_File file = FS_OpenOmniFile(filename);
	if (!file)
		return false;
	FS_CloseFile(file);
	return true;
}

// Adjusts the extension on a filename to match the current episode.
// This function is NOT thread safe, and the string returned is only
// valid until the NEXT invocation of this function.
char *FS_AdjustExtension(const char *filename)
{
	static char newname[16];
	strcpy(newname, filename);
	size_t fnamelen = strlen(filename);
	newname[fnamelen - 3] = ck_currentEpisode->ext[0];
	newname[fnamelen - 2] = ck_currentEpisode->ext[1];
	newname[fnamelen - 1] = ck_currentEpisode->ext[2];
	return newname;
}


// Does a file exist (and is it readable)
bool FS_IsUserFilePresent(const char *filename)
{
	FS_File file = FS_OpenUserFile(filename);
	if (!file)
		return false;
	FS_CloseFile(file);
	return true;
}

bool FSL_IsGoodOmniPath(const char *ext)
{
	if (!FS_IsOmniFilePresent("ACTION.CK4"))
		return false;
	return true;
}

static const char *fs_parmStrings[] = {"GAMEPATH", "USERPATH", NULL};

void FS_Startup()
{
	// For now, all of the paths will be the current directory.

	fs_keenPath = ".";
	fs_omniPath = ".";
	fs_userPath = ".";

#ifdef WITH_SDL
#if SDL_VERSION_ATLEAST(2, 0, 1)
	if (!FSL_IsGoodOmniPath(fs_omniPath))
	{
		fs_omniPath = SDL_GetBasePath();
	}
#endif
#endif

	// Check command line args.
	for (int i = 1; i < us_argc; ++i)
	{
		int parmIdx = US_CheckParm(us_argv[i], fs_parmStrings);
		switch (parmIdx)
		{
		case 0:
			fs_keenPath = us_argv[++i];
			break;
		case 1:
			fs_userPath = us_argv[++i];
			break;
		}
	}
}

size_t FS_ReadInt8LE(void *ptr, size_t count, FILE *stream)
{
	return fread(ptr, 1, count, stream);
}

size_t FS_ReadInt16LE(void *ptr, size_t count, FILE *stream)
{
	count = fread(ptr, 2, count, stream);
#ifdef CK_CROSS_IS_BIGENDIAN
	uint16_t *uptr = (uint16_t *)ptr;
	for (size_t loopVar = 0; loopVar < count; loopVar++, uptr++)
		*uptr = CK_Cross_Swap16(*uptr);
#endif
	return count;
}

size_t FS_ReadInt32LE(void *ptr, size_t count, FILE *stream)
{
	count = fread(ptr, 4, count, stream);
#ifdef CK_CROSS_IS_BIGENDIAN
	uint32_t *uptr = (uint32_t *)ptr;
	for (size_t loopVar = 0; loopVar < count; loopVar++, uptr++)
		*uptr = CK_Cross_Swap32(*uptr);
#endif
	return count;
}

size_t FS_WriteInt8LE(const void *ptr, size_t count, FILE *stream)
{
	return fwrite(ptr, 1, count, stream);
}

size_t FS_WriteInt16LE(const void *ptr, size_t count, FILE *stream)
{
#ifndef CK_CROSS_IS_BIGENDIAN
	return fwrite(ptr, 2, count, stream);
#else
	uint16_t val;
	size_t actualCount = 0;
	uint16_t *uptr = (uint16_t *)ptr;
	for (size_t loopVar = 0; loopVar < count; loopVar++, uptr++)
	{
		val = CK_Cross_Swap16(*uptr);
		actualCount += fwrite(&val, 2, 1, stream);
	}
	return actualCount;
#endif
}

size_t FS_WriteInt32LE(const void *ptr, size_t count, FILE *stream)
{
#ifndef CK_CROSS_IS_BIGENDIAN
	return fwrite(ptr, 4, count, stream);
#else
	uint32_t val;
	size_t actualCount = 0;
	uint32_t *uptr = (uint32_t *)ptr;
	for (size_t loopVar = 0; loopVar < count; loopVar++, uptr++)
	{
		val = CK_Cross_Swap32(*uptr);
		actualCount += fwrite(&val, 4, 1, stream);
	}
	return actualCount;
#endif
}

size_t FS_ReadBoolFrom16LE(void *ptr, size_t count, FILE *stream)
{
	uint16_t val;
	size_t actualCount = 0;
	bool *currBoolPtr = (bool *)ptr; // No lvalue compilation error
	for (size_t loopVar = 0; loopVar < count; loopVar++, currBoolPtr++)
	{
		if (fread(&val, 2, 1, stream)) // Should be either 0 or 1
		{
			*currBoolPtr = (val); // NOTE: No need to byte-swap
			actualCount++;
		}
	}
	return actualCount;
}

size_t FS_WriteBoolTo16LE(const void *ptr, size_t count, FILE *stream)
{
	uint16_t val;
	size_t actualCount = 0;
	bool *currBoolPtr = (bool *)ptr; // No lvalue compilation error
	for (size_t loopVar = 0; loopVar < count; loopVar++, currBoolPtr++)
	{
		val = CK_Cross_SwapLE16((*currBoolPtr) ? 1 : 0);
		actualCount += fwrite(&val, 2, 1, stream);
	}
	return actualCount;
}
