#include "pch.h"
#include "Path.h"
#include "Utility.h"

void Test_Folder(
	std::string path_string,
	std::string absolute_folder_path,
	std::string folder_name,
	std::string relative_folder_path)
{
	replace_all(path_string         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(absolute_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(folder_name         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(relative_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	CPath path(path_string);

	ASSERT_EQ(path.IsFile(), false);
	ASSERT_EQ(path.IsFolder(), true);

	ASSERT_EQ(path.GetAbsoluteFolderPath(), absolute_folder_path);
	ASSERT_EQ(path.GetFolderName(), folder_name);
	ASSERT_EQ(path.GetRelativeFolderPath(), relative_folder_path);

	ASSERT_ANY_THROW(path.GetRawFileName());
	ASSERT_ANY_THROW(path.GetFullFileName());
	ASSERT_ANY_THROW(path.GetExtension());
	ASSERT_ANY_THROW(path.GetAbsoluteFilePath());
	ASSERT_ANY_THROW(path.GetRelativeFilePath());
}

TEST(PathTest, RelativeFolder1)
{
	std::string path_string          = "home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, RelativeFolder2)
{
	std::string path_string          = "etc/../home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, AbsoluteFolder1)
{
	std::string path_string          = GetCurrentWorkingDirectory() + "/home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, AbsoluteFolder2)
{
	std::string path_string          = "~\\home\\";
	std::string absolute_folder_path = "~\\home\\";
	std::string folder_name          = "home";
	std::string relative_folder_path = "~\\home\\";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(PathTest, AbsoluteFolder3)
{
	std::string path_string          = "\\\\computer\\home\\";
	std::string absolute_folder_path = "\\\\computer\\home\\";
	std::string folder_name          = "home";
	std::string relative_folder_path = "\\\\computer\\home\\";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}


void Test_File(
	std::string path_string,
	std::string raw_file_name,
	std::string full_file_name,
	std::string extension,
	std::string absolute_file_path,
	std::string relative_file_path)
{
	replace_all(path_string       , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(raw_file_name     , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(full_file_name    , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(extension         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(absolute_file_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(relative_file_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	CPath path(path_string);

	ASSERT_EQ(path.IsFile(), true);
	ASSERT_EQ(path.IsFolder(), false);

	ASSERT_ANY_THROW(path.GetAbsoluteFolderPath());
	ASSERT_ANY_THROW(path.GetFolderName());
	ASSERT_ANY_THROW(path.GetRelativeFolderPath());

	ASSERT_EQ(path.GetRawFileName(), raw_file_name);
	ASSERT_EQ(path.GetFullFileName(), full_file_name);
	ASSERT_EQ(path.GetExtension(), extension);
	ASSERT_EQ(path.GetAbsoluteFilePath(), absolute_file_path);
	ASSERT_EQ(path.GetRelativeFilePath(), relative_file_path);
}

TEST(PathTest, RelativeFile1)
{
	std::string path_string		   = "home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string relative_file_path = "home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, RelativeFile2)
{
	std::string path_string		   = "text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/text.txt";
	std::string relative_file_path = "text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, RelativeFile3)
{
	std::string path_string		   = "home/../text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/text.txt";
	std::string relative_file_path = "text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, AbsoluteFile1)
{
	std::string path_string		   = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string relative_file_path = "home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, AbsoluteFile2)
{
	std::string path_string		   = "~/home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = "~/home/text.txt";
	std::string relative_file_path = "~/home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}

TEST(PathTest, AbsoluteFile3)
{
	std::string path_string		   = "//home/text.txt";
	std::string raw_file_name	   = "text";
	std::string full_file_name	   = "text.txt";
	std::string extension		   = "txt";
	std::string absolute_file_path = "//home/text.txt";
	std::string relative_file_path = "//home/text.txt";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path);
}