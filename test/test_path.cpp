#include "path.h"
#include "macros_hell.h"
#include "configfile.h"
#include "gtest/gtest.h"

TEST (PathTest, RelativeFolder1)
{
	//     "home/"
	const std::string PathString = "home" + FOLDER_SEPARATOR;
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_EQ (path.GetRelativeFolderPath(), PathString);
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, RelativeFolder2)
{
	//     "etc/../home/"
	const std::string PathString = "etc" + FOLDER_SEPARATOR + ".." + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR;
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_EQ (path.GetRelativeFolderPath(), "home" + FOLDER_SEPARATOR);
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, AbsoluteFolder1)
{
	//     "C:\home\"
	const std::string PathString = "C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR;
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), PathString);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_EQ (path.GetRelativeFolderPath(), PathString);
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, AbsoluteFolder2)
{
	 //     "~\home\"
	const std::string PathString = "~" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR;
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), PathString);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_EQ (path.GetRelativeFolderPath(), PathString);
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, AbsoluteFolder3)
{
	//     "\\computer\home\"
	const std::string PathString = FOLDER_SEPARATOR + FOLDER_SEPARATOR + "computer" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR;
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), PathString);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_EQ (path.GetRelativeFolderPath(), PathString);
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, RelativeFile1)
{
	//     "home/text.txt"
	const std::string PathString = "home" + FOLDER_SEPARATOR + "text.txt";
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_EQ (path.GetRelativeFilePath(), PathString);
}

TEST (PathTest, RelativeFile2)
{
	//     "text.txt"
	const std::string PathString = "text.txt";
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "text.txt");
	ASSERT_EQ (path.GetRelativeFilePath(), PathString);
}

TEST (PathTest, RelativeFile3)
{
	//     "home/../text.txt"
	const std::string PathString = "home" + FOLDER_SEPARATOR + ".." + FOLDER_SEPARATOR + "text.txt";
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "text.txt");
	ASSERT_EQ (path.GetRelativeFilePath(), "text.txt");
}

TEST (PathTest, AbsoluteFile1)
{
	//     "C:/home/text.txt"
	const std::string PathString = "C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt";
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), "C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_EQ (path.GetRelativeFilePath(), PathString);
}

TEST (PathTest, AbsoluteFile2)
{
	//     "~/home/text.txt"
	const std::string PathString = "~" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt";
	CPath path(PathString);
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), "~" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_EQ (path.GetRelativeFilePath(), PathString);
}

TEST (PathTest, AbsoluteFile3)
{
	//     "//home/text.txt"
	const std::string PathString = FOLDER_SEPARATOR + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt";
	CPath path(PathString);
		
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), FOLDER_SEPARATOR + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_EQ (path.GetRelativeFilePath(), PathString);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	gConfigurations = CConfigurations(argv[0]);
	
	return RUN_ALL_TESTS();
}
