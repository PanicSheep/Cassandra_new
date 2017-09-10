#include "path.h"
#include "macros_hell.h"
#include "gtest/gtest.h"

TEST (PathTest, RelativeFolder1) {
	CPath path = CPath("home" + FOLDER_SEPARATOR); //     "home/"
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_ANY_THROW (path.GetRelativeFolderPath());
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, RelativeFolder2) {
	CPath path = CPath("etc" + FOLDER_SEPARATOR + ".." + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR); //     "etc/../home/"
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_ANY_THROW (path.GetRelativeFolderPath());
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, AbsoluteFolder1) {
	CPath path = CPath("C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR); //     "C:\home\"
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), "C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_ANY_THROW (path.GetRelativeFolderPath());
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, AbsoluteFolder2) {
	CPath path = CPath("." + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR); //     ".\home\"
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), "." + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_ANY_THROW (path.GetRelativeFolderPath());
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, AbsoluteFolder3) {
	CPath path = CPath(FOLDER_SEPARATOR + FOLDER_SEPARATOR + "computer" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR); //     "\\computer\home\"
	
	ASSERT_EQ (path.IsFile(), false);
	ASSERT_EQ (path.IsFolder(), true);
	
	ASSERT_EQ (path.GetAbsoluteFolderPath(), FOLDER_SEPARATOR + FOLDER_SEPARATOR + "computer" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR);
	ASSERT_EQ (path.GetFolderName(), "home");
	ASSERT_ANY_THROW (path.GetRelativeFolderPath());
	
	ASSERT_DEATH (path.GetFileName(), "(.*?)");
	ASSERT_DEATH (path.GetFileNameExtension(), "(.*?)");
	ASSERT_DEATH (path.GetAbsoluteFilePath(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFilePath(), "(.*?)");
}

TEST (PathTest, RelativeFile1) {
	CPath path = CPath("home" + FOLDER_SEPARATOR + "text.txt"); //     "home/text.txt"
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_ANY_THROW (path.GetRelativeFilePath());
}

TEST (PathTest, RelativeFile2) {
	CPath path = CPath("text.txt"); //     "text.txt"
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "text.txt");
	ASSERT_ANY_THROW (path.GetRelativeFilePath());
}

TEST (PathTest, RelativeFile3) {
	CPath path = CPath("home" + FOLDER_SEPARATOR + ".." + FOLDER_SEPARATOR + "text.txt"); //     "home/../text.txt"
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), GetCurrentWorkingDirectory() + FOLDER_SEPARATOR + "text.txt");
	ASSERT_ANY_THROW (path.GetRelativeFilePath());
}

TEST (PathTest, AbsoluteFile1) {
	CPath path = CPath("C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt"); //     "C:/home/text.txt"
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), "C:" + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_ANY_THROW (path.GetRelativeFilePath());
}

TEST (PathTest, AbsoluteFile2) {
	CPath path = CPath("." + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt"); //     "./home/text.txt"
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), "." + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_ANY_THROW (path.GetRelativeFilePath());
}

TEST (PathTest, AbsoluteFile3) {
	CPath path = CPath(FOLDER_SEPARATOR + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt"); //     "//home/text.txt"
	
	ASSERT_EQ (path.IsFile(), true);
	ASSERT_EQ (path.IsFolder(), false);
	
	ASSERT_DEATH (path.GetAbsoluteFolderPath(), "(.*?)");
	ASSERT_DEATH (path.GetFolderName(), "(.*?)");
	ASSERT_DEATH (path.GetRelativeFolderPath(), "(.*?)");
	
	ASSERT_EQ (path.GetFileName(), "text.txt");
	ASSERT_EQ (path.GetFileNameExtension(), "txt");
	ASSERT_EQ (path.GetAbsoluteFilePath(), FOLDER_SEPARATOR + FOLDER_SEPARATOR + "home" + FOLDER_SEPARATOR + "text.txt");
	ASSERT_ANY_THROW (path.GetRelativeFilePath());
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	
	gConfigurations = CConfigurations(argv[0]);
	
	return RUN_ALL_TESTS();
}
