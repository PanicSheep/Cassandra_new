#include "pch.h"
#include "Path.h"
#include "Utility.h"

TEST(Path, ProcessFolderUps1) { ASSERT_EQ(CPath(R"(X:/home/home/../)"), CPath(R"(X:/home/)")); }
TEST(Path, ProcessFolderUps2) { ASSERT_EQ(CPath(R"(X:\home\home\..\)"), CPath(R"(X:\home\)")); }
TEST(Path, ProcessFolderUps3) { ASSERT_EQ(CPath(R"(X:/home/../home/)"), CPath(R"(X:/home/)")); }
TEST(Path, ProcessFolderUps4) { ASSERT_EQ(CPath(R"(X:\home\..\home\)"), CPath(R"(X:\home\)")); }

void IsFolder(const CPath& path)
{
	ASSERT_EQ(path.IsFolder(), true);
	ASSERT_EQ(path.IsFile(), false);
}

TEST(Path, IsFolder01) { IsFolder(CPath(R"(X:/home/)")); }
TEST(Path, IsFolder02) { IsFolder(CPath(R"(X:\home\)")); }
TEST(Path, IsFolder03) { IsFolder(CPath(R"(//computer/home/)")); }
TEST(Path, IsFolder04) { IsFolder(CPath(R"(\\computer\home\)")); }
TEST(Path, IsFolder05) { IsFolder(CPath(R"(~/home/)")); }
TEST(Path, IsFolder06) { IsFolder(CPath(R"(~\home\)")); }
TEST(Path, IsFolder07) { IsFolder(CPath(R"(/home/)")); }
TEST(Path, IsFolder08) { IsFolder(CPath(R"(\home\)")); }
TEST(Path, IsFolder09) { IsFolder(CPath(R"(home/)")); }
TEST(Path, IsFolder10) { IsFolder(CPath(R"(home\)")); }

void GetFolderName(std::string folder_name, const CPath& path)
{
	replace_all(folder_name, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	ASSERT_EQ(path.GetFolderName(), folder_name);
}

TEST(Path, GetFolderName01) { GetFolderName(R"(home/)", CPath(R"(X:/home/)")); }
TEST(Path, GetFolderName02) { GetFolderName(R"(home\)", CPath(R"(X:\home\)")); }
TEST(Path, GetFolderName03) { GetFolderName(R"(home/)", CPath(R"(//computer/home/)")); }
TEST(Path, GetFolderName04) { GetFolderName(R"(home\)", CPath(R"(\\computer\home\)")); }
TEST(Path, GetFolderName05) { GetFolderName(R"(home/)", CPath(R"(~/home/)")); }
TEST(Path, GetFolderName06) { GetFolderName(R"(home\)", CPath(R"(~\home\)")); }
TEST(Path, GetFolderName07) { GetFolderName(R"(home/)", CPath(R"(/home/)")); }
TEST(Path, GetFolderName08) { GetFolderName(R"(home\)", CPath(R"(\home\)")); }
TEST(Path, GetFolderName09) { GetFolderName(R"(home/)", CPath(R"(home/)")); }
TEST(Path, GetFolderName10) { GetFolderName(R"(home\)", CPath(R"(home\)")); }
TEST(Path, GetFolderName11) { GetFolderName(R"(home/)", CPath(R"(X:/home/file)")); }
TEST(Path, GetFolderName12) { GetFolderName(R"(home/)", CPath(R"(X:/home/file.txt)")); }
TEST(Path, GetFolderName13) { GetFolderName(R"(home\)", CPath(R"(X:\home\file)")); }
TEST(Path, GetFolderName14) { GetFolderName(R"(home\)", CPath(R"(X:\home\file.txt)")); }
TEST(Path, GetFolderName15) { GetFolderName(R"(home/)", CPath(R"(//computer/home/file)")); }
TEST(Path, GetFolderName16) { GetFolderName(R"(home/)", CPath(R"(//computer/home/file.txt)")); }
TEST(Path, GetFolderName17) { GetFolderName(R"(home\)", CPath(R"(\\computer\home\file)")); }
TEST(Path, GetFolderName18) { GetFolderName(R"(home\)", CPath(R"(\\computer\home\file.txt)")); }
TEST(Path, GetFolderName19) { GetFolderName(R"(home/)", CPath(R"(~/home/file)")); }
TEST(Path, GetFolderName20) { GetFolderName(R"(home/)", CPath(R"(~/home/file.txt)")); }
TEST(Path, GetFolderName21) { GetFolderName(R"(home\)", CPath(R"(~\home\file)")); }
TEST(Path, GetFolderName22) { GetFolderName(R"(home\)", CPath(R"(~\home\file.txt)")); }
TEST(Path, GetFolderName23) { GetFolderName(R"(home/)", CPath(R"(/home/file)")); }
TEST(Path, GetFolderName24) { GetFolderName(R"(home/)", CPath(R"(/home/file.txt)")); }
TEST(Path, GetFolderName25) { GetFolderName(R"(home\)", CPath(R"(\home\file)")); }
TEST(Path, GetFolderName26) { GetFolderName(R"(home\)", CPath(R"(\home\file.txt)")); }
TEST(Path, GetFolderName27) { GetFolderName(R"(home/)", CPath(R"(home/file)")); }
TEST(Path, GetFolderName28) { GetFolderName(R"(home/)", CPath(R"(home/file.txt)")); }
TEST(Path, GetFolderName29) { GetFolderName(R"(home\)", CPath(R"(home\file)")); }
TEST(Path, GetFolderName30) { GetFolderName(R"(home\)", CPath(R"(home\file.txt)")); }

void GetAbsoluteFolderPath(const CPath& path, std::string absolute_folder_path)
{
	replace_all(absolute_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	ASSERT_EQ(path.GetAbsoluteFolderPath(), absolute_folder_path);
}

TEST(Path, GetAbsoluteFolderPath01) { GetAbsoluteFolderPath(CPath(R"(X:/home/)"), R"(X:/home/)"); }
TEST(Path, GetAbsoluteFolderPath02) { GetAbsoluteFolderPath(CPath(R"(X:\home\)"), R"(X:\home\)"); }
TEST(Path, GetAbsoluteFolderPath03) { GetAbsoluteFolderPath(CPath(R"(//computer/home/)"), R"(//computer/home/)"); }
TEST(Path, GetAbsoluteFolderPath04) { GetAbsoluteFolderPath(CPath(R"(\\computer\home\)"), R"(\\computer\home\)"); }
TEST(Path, GetAbsoluteFolderPath05) { GetAbsoluteFolderPath(CPath(R"(~/home/)"), R"(~/home/)"); }
TEST(Path, GetAbsoluteFolderPath06) { GetAbsoluteFolderPath(CPath(R"(~\home\)"), R"(~\home\)"); }
TEST(Path, GetAbsoluteFolderPath07) { GetAbsoluteFolderPath(CPath(R"(/home/)"), GetCurrentWorkingDirectory() + R"(/home/)"); }
TEST(Path, GetAbsoluteFolderPath08) { GetAbsoluteFolderPath(CPath(R"(\home\)"), GetCurrentWorkingDirectory() + R"(\home\)"); }
TEST(Path, GetAbsoluteFolderPath09) { GetAbsoluteFolderPath(CPath(R"(home/)"), GetCurrentWorkingDirectory() + R"(/home/)"); }
TEST(Path, GetAbsoluteFolderPath10) { GetAbsoluteFolderPath(CPath(R"(home\)"), GetCurrentWorkingDirectory() + R"(\home\)"); }
TEST(Path, GetAbsoluteFolderPath11) { GetAbsoluteFolderPath(CPath(R"(X:/home/file)"), R"(X:/home/)"); }
TEST(Path, GetAbsoluteFolderPath12) { GetAbsoluteFolderPath(CPath(R"(X:/home/file.txt)"), R"(X:\home\)"); }
TEST(Path, GetAbsoluteFolderPath13) { GetAbsoluteFolderPath(CPath(R"(X:\home\file)"), R"(X:/home/)"); }
TEST(Path, GetAbsoluteFolderPath14) { GetAbsoluteFolderPath(CPath(R"(X:\home\file.txt)"), R"(X:\home\)"); }
TEST(Path, GetAbsoluteFolderPath15) { GetAbsoluteFolderPath(CPath(R"(//computer/home/file)"), R"(//computer/home/)"); }
TEST(Path, GetAbsoluteFolderPath16) { GetAbsoluteFolderPath(CPath(R"(//computer/home/file.txt)"), R"(//computer/home/)"); }
TEST(Path, GetAbsoluteFolderPath17) { GetAbsoluteFolderPath(CPath(R"(\\computer\home\file)"), R"(\\computer\home\)"); }
TEST(Path, GetAbsoluteFolderPath18) { GetAbsoluteFolderPath(CPath(R"(\\computer\home\file.txt)"), R"(\\computer\home\)"); }
TEST(Path, GetAbsoluteFolderPath19) { GetAbsoluteFolderPath(CPath(R"(~/home/file)"), R"(~/home/)"); }
TEST(Path, GetAbsoluteFolderPath20) { GetAbsoluteFolderPath(CPath(R"(~/home/file.txt)"), R"(~/home/)"); }
TEST(Path, GetAbsoluteFolderPath21) { GetAbsoluteFolderPath(CPath(R"(~\home\file)"), R"(~\home\)"); }
TEST(Path, GetAbsoluteFolderPath22) { GetAbsoluteFolderPath(CPath(R"(~\home\file.txt)"), R"(~\home\)"); }
TEST(Path, GetAbsoluteFolderPath23) { GetAbsoluteFolderPath(CPath(R"(/home/file)"), GetCurrentWorkingDirectory() + R"(/home/)"); }
TEST(Path, GetAbsoluteFolderPath24) { GetAbsoluteFolderPath(CPath(R"(/home/file.txt)"), GetCurrentWorkingDirectory() + R"(/home/)"); }
TEST(Path, GetAbsoluteFolderPath25) { GetAbsoluteFolderPath(CPath(R"(\home\file)"), GetCurrentWorkingDirectory() + R"(\home\)"); }
TEST(Path, GetAbsoluteFolderPath26) { GetAbsoluteFolderPath(CPath(R"(\home\file.txt)"), GetCurrentWorkingDirectory() + R"(\home\)"); }
TEST(Path, GetAbsoluteFolderPath27) { GetAbsoluteFolderPath(CPath(R"(home/file)"), GetCurrentWorkingDirectory() + R"(/home/)"); }
TEST(Path, GetAbsoluteFolderPath28) { GetAbsoluteFolderPath(CPath(R"(home/file.txt)"), GetCurrentWorkingDirectory() + R"(/home/)"); }
TEST(Path, GetAbsoluteFolderPath29) { GetAbsoluteFolderPath(CPath(R"(home\file)"), GetCurrentWorkingDirectory() + R"(\home\)"); }
TEST(Path, GetAbsoluteFolderPath30) { GetAbsoluteFolderPath(CPath(R"(home\file.txt)"), GetCurrentWorkingDirectory() + R"(\home\)"); }

void GetRelativeFolderPath(const CPath& path, std::string relative_folder_path)
{
	replace_all(relative_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	ASSERT_EQ(path.GetRelativeFolderPath(), relative_folder_path);
}

TEST(Path, GetRelativeFolderPath01) { GetRelativeFolderPath(CPath(R"(X:/home/)"), R"(X:/home/)"); }
TEST(Path, GetRelativeFolderPath02) { GetRelativeFolderPath(CPath(R"(X:\home\)"), R"(X:\home\)"); }
TEST(Path, GetRelativeFolderPath03) { GetRelativeFolderPath(CPath(R"(//computer/home/)"), R"(//computer/home/)"); }
TEST(Path, GetRelativeFolderPath04) { GetRelativeFolderPath(CPath(R"(\\computer\home\)"), R"(\\computer\home\)"); }
TEST(Path, GetRelativeFolderPath05) { GetRelativeFolderPath(CPath(R"(~/home/)"), R"(~/home/)"); }
TEST(Path, GetRelativeFolderPath06) { GetRelativeFolderPath(CPath(R"(~\home\)"), R"(~\home\)"); }
TEST(Path, GetRelativeFolderPath07) { GetRelativeFolderPath(CPath(R"(/home/)"), R"(home/)"); }
TEST(Path, GetRelativeFolderPath08) { GetRelativeFolderPath(CPath(R"(\home\)"), R"(home\)"); }
TEST(Path, GetRelativeFolderPath09) { GetRelativeFolderPath(CPath(R"(home/)"), R"(home/)"); }
TEST(Path, GetRelativeFolderPath10) { GetRelativeFolderPath(CPath(R"(home\)"), R"(home\)"); }
TEST(Path, GetRelativeFolderPath11) { GetRelativeFolderPath(CPath(R"(X:/home/file)"), R"(X:/home/)"); }
TEST(Path, GetRelativeFolderPath12) { GetRelativeFolderPath(CPath(R"(X:/home/file.txt)"), R"(X:\home\)"); }
TEST(Path, GetRelativeFolderPath13) { GetRelativeFolderPath(CPath(R"(X:\home\file)"), R"(X:/home/)"); }
TEST(Path, GetRelativeFolderPath14) { GetRelativeFolderPath(CPath(R"(X:\home\file.txt)"), R"(X:\home\)"); }
TEST(Path, GetRelativeFolderPath15) { GetRelativeFolderPath(CPath(R"(//computer/home/file)"), R"(//computer/home/)"); }
TEST(Path, GetRelativeFolderPath16) { GetRelativeFolderPath(CPath(R"(//computer/home/file.txt)"), R"(//computer/home/)"); }
TEST(Path, GetRelativeFolderPath17) { GetRelativeFolderPath(CPath(R"(\\computer\home\file)"), R"(\\computer\home\)"); }
TEST(Path, GetRelativeFolderPath18) { GetRelativeFolderPath(CPath(R"(\\computer\home\file.txt)"), R"(\\computer\home\)"); }
TEST(Path, GetRelativeFolderPath19) { GetRelativeFolderPath(CPath(R"(~/home/file)"), R"(~/home/)"); }
TEST(Path, GetRelativeFolderPath20) { GetRelativeFolderPath(CPath(R"(~/home/file.txt)"), R"(~/home/)"); }
TEST(Path, GetRelativeFolderPath21) { GetRelativeFolderPath(CPath(R"(~\home\file)"), R"(~\home\)"); }
TEST(Path, GetRelativeFolderPath22) { GetRelativeFolderPath(CPath(R"(~\home\file.txt)"), R"(~\home\)"); }
TEST(Path, GetRelativeFolderPath23) { GetRelativeFolderPath(CPath(R"(/home/file)"), R"(home/)"); }
TEST(Path, GetRelativeFolderPath24) { GetRelativeFolderPath(CPath(R"(/home/file.txt)"), R"(home/)"); }
TEST(Path, GetRelativeFolderPath25) { GetRelativeFolderPath(CPath(R"(\home\file)"), R"(home\)"); }
TEST(Path, GetRelativeFolderPath26) { GetRelativeFolderPath(CPath(R"(\home\file.txt)"), R"(home\)"); }
TEST(Path, GetRelativeFolderPath27) { GetRelativeFolderPath(CPath(R"(home/file)"), R"(home/)"); }
TEST(Path, GetRelativeFolderPath28) { GetRelativeFolderPath(CPath(R"(home/file.txt)"), R"(home/)"); }
TEST(Path, GetRelativeFolderPath29) { GetRelativeFolderPath(CPath(R"(home\file)"), R"(home\)"); }
TEST(Path, GetRelativeFolderPath30) { GetRelativeFolderPath(CPath(R"(home\file.txt)"), R"(home\)"); }

void IsFile(const CPath& path)
{
	ASSERT_EQ(path.IsFile(), true);
	ASSERT_EQ(path.IsFolder(), false);
}

TEST(Path, IsFile01) { IsFile(CPath(R"(X:/home/file)")); }
TEST(Path, IsFile02) { IsFile(CPath(R"(X:/home/file.txt)")); }
TEST(Path, IsFile03) { IsFile(CPath(R"(X:\home\file)")); }
TEST(Path, IsFile04) { IsFile(CPath(R"(X:\home\file.txt)")); }
TEST(Path, IsFile05) { IsFile(CPath(R"(//computer/home/file)")); }
TEST(Path, IsFile06) { IsFile(CPath(R"(//computer/home/file.txt)")); }
TEST(Path, IsFile07) { IsFile(CPath(R"(\\computer\home\file)")); }
TEST(Path, IsFile08) { IsFile(CPath(R"(\\computer\home\file.txt)")); }
TEST(Path, IsFile09) { IsFile(CPath(R"(~/home/file)")); }
TEST(Path, IsFile10) { IsFile(CPath(R"(~/home/file.txt)")); }
TEST(Path, IsFile11) { IsFile(CPath(R"(~\home\file)")); }
TEST(Path, IsFile12) { IsFile(CPath(R"(~\home\file.txt)")); }
TEST(Path, IsFile13) { IsFile(CPath(R"(/home/file)")); }
TEST(Path, IsFile14) { IsFile(CPath(R"(/home/file.txt)")); }
TEST(Path, IsFile15) { IsFile(CPath(R"(\home\file)")); }
TEST(Path, IsFile16) { IsFile(CPath(R"(\home\file.txt)")); }
TEST(Path, IsFile17) { IsFile(CPath(R"(home/file)")); }
TEST(Path, IsFile18) { IsFile(CPath(R"(home/file.txt)")); }
TEST(Path, IsFile19) { IsFile(CPath(R"(home\file)")); }
TEST(Path, IsFile20) { IsFile(CPath(R"(home\file.txt)")); }

void GetRawFileName(const CPath& path, const std::string& raw_file_name)
{
	ASSERT_EQ(path.GetRawFileName(), raw_file_name);
}

TEST(Path, GetRawFileName01) { GetRawFileName(CPath(R"(X:/home/file)"), "file"); }
TEST(Path, GetRawFileName02) { GetRawFileName(CPath(R"(X:/home/file.txt)"), "file"); }
TEST(Path, GetRawFileName03) { GetRawFileName(CPath(R"(X:\home\file)"), "file"); }
TEST(Path, GetRawFileName04) { GetRawFileName(CPath(R"(X:\home\file.txt)"), "file"); }
TEST(Path, GetRawFileName05) { GetRawFileName(CPath(R"(//computer/home/file)"), "file"); }
TEST(Path, GetRawFileName06) { GetRawFileName(CPath(R"(//computer/home/file.txt)"), "file"); }
TEST(Path, GetRawFileName07) { GetRawFileName(CPath(R"(\\computer\home\file)"), "file"); }
TEST(Path, GetRawFileName08) { GetRawFileName(CPath(R"(\\computer\home\file.txt)"), "file"); }
TEST(Path, GetRawFileName09) { GetRawFileName(CPath(R"(~/home/file)"), "file"); }
TEST(Path, GetRawFileName10) { GetRawFileName(CPath(R"(~/home/file.txt)"), "file"); }
TEST(Path, GetRawFileName11) { GetRawFileName(CPath(R"(~\home\file)"), "file"); }
TEST(Path, GetRawFileName12) { GetRawFileName(CPath(R"(~\home\file.txt)"), "file"); }
TEST(Path, GetRawFileName13) { GetRawFileName(CPath(R"(/home/file)"), "file"); }
TEST(Path, GetRawFileName14) { GetRawFileName(CPath(R"(/home/file.txt)"), "file"); }
TEST(Path, GetRawFileName15) { GetRawFileName(CPath(R"(\home\file)"), "file"); }
TEST(Path, GetRawFileName16) { GetRawFileName(CPath(R"(\home\file.txt)"), "file"); }
TEST(Path, GetRawFileName17) { GetRawFileName(CPath(R"(home/file)"), "file"); }
TEST(Path, GetRawFileName18) { GetRawFileName(CPath(R"(home/file.txt)"), "file"); }
TEST(Path, GetRawFileName19) { GetRawFileName(CPath(R"(home\file)"), "file"); }
TEST(Path, GetRawFileName20) { GetRawFileName(CPath(R"(home\file.txt)"), "file"); }

void GetFullFileName(const CPath& path, const std::string& raw_file_name)
{
	ASSERT_EQ(path.GetFullFileName(), raw_file_name);
}

TEST(Path, GetFullFileName01) { GetFullFileName(CPath(R"(X:/home/file)"), "file"); }
TEST(Path, GetFullFileName02) { GetFullFileName(CPath(R"(X:/home/file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName03) { GetFullFileName(CPath(R"(X:\home\file)"), "file"); }
TEST(Path, GetFullFileName04) { GetFullFileName(CPath(R"(X:\home\file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName05) { GetFullFileName(CPath(R"(//computer/home/file)"), "file"); }
TEST(Path, GetFullFileName06) { GetFullFileName(CPath(R"(//computer/home/file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName07) { GetFullFileName(CPath(R"(\\computer\home\file)"), "file"); }
TEST(Path, GetFullFileName08) { GetFullFileName(CPath(R"(\\computer\home\file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName09) { GetFullFileName(CPath(R"(~/home/file)"), "file"); }
TEST(Path, GetFullFileName10) { GetFullFileName(CPath(R"(~/home/file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName11) { GetFullFileName(CPath(R"(~\home\file)"), "file"); }
TEST(Path, GetFullFileName12) { GetFullFileName(CPath(R"(~\home\file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName13) { GetFullFileName(CPath(R"(/home/file)"), "file"); }
TEST(Path, GetFullFileName14) { GetFullFileName(CPath(R"(/home/file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName15) { GetFullFileName(CPath(R"(\home\file)"), "file"); }
TEST(Path, GetFullFileName16) { GetFullFileName(CPath(R"(\home\file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName17) { GetFullFileName(CPath(R"(home/file)"), "file"); }
TEST(Path, GetFullFileName18) { GetFullFileName(CPath(R"(home/file.txt)"), "file.txt"); }
TEST(Path, GetFullFileName19) { GetFullFileName(CPath(R"(home\file)"), "file"); }
TEST(Path, GetFullFileName20) { GetFullFileName(CPath(R"(home\file.txt)"), "file.txt"); }

void GetExtension(const CPath& path, const std::string& raw_file_name)
{
	ASSERT_EQ(path.GetExtension(), raw_file_name);
}

TEST(Path, GetExtension01) { GetExtension(CPath(R"(X:/home/file)"), ""); }
TEST(Path, GetExtension02) { GetExtension(CPath(R"(X:/home/file.txt)"), "txt"); }
TEST(Path, GetExtension03) { GetExtension(CPath(R"(X:\home\file)"), ""); }
TEST(Path, GetExtension04) { GetExtension(CPath(R"(X:\home\file.txt)"), "txt"); }
TEST(Path, GetExtension05) { GetExtension(CPath(R"(//computer/home/file)"), ""); }
TEST(Path, GetExtension06) { GetExtension(CPath(R"(//computer/home/file.txt)"), "txt"); }
TEST(Path, GetExtension07) { GetExtension(CPath(R"(\\computer\home\file)"), ""); }
TEST(Path, GetExtension08) { GetExtension(CPath(R"(\\computer\home\file.txt)"), "txt"); }
TEST(Path, GetExtension09) { GetExtension(CPath(R"(~/home/file)"), ""); }
TEST(Path, GetExtension10) { GetExtension(CPath(R"(~/home/file.txt)"), "txt"); }
TEST(Path, GetExtension11) { GetExtension(CPath(R"(~\home\file)"), ""); }
TEST(Path, GetExtension12) { GetExtension(CPath(R"(~\home\file.txt)"), "txt"); }
TEST(Path, GetExtension13) { GetExtension(CPath(R"(/home/file)"), ""); }
TEST(Path, GetExtension14) { GetExtension(CPath(R"(/home/file.txt)"), "txt"); }
TEST(Path, GetExtension15) { GetExtension(CPath(R"(\home\file)"), ""); }
TEST(Path, GetExtension16) { GetExtension(CPath(R"(\home\file.txt)"), "txt"); }
TEST(Path, GetExtension17) { GetExtension(CPath(R"(home/file)"), ""); }
TEST(Path, GetExtension18) { GetExtension(CPath(R"(home/file.txt)"), "txt"); }
TEST(Path, GetExtension19) { GetExtension(CPath(R"(home\file)"), ""); }
TEST(Path, GetExtension20) { GetExtension(CPath(R"(home\file.txt)"), "txt"); }

void GetAbsoluteFilePath(const CPath& path, std::string absolute_file_path)
{
	replace_all(absolute_file_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	ASSERT_EQ(path.GetAbsoluteFilePath(), absolute_file_path);
}

TEST(Path, GetAbsoluteFilePath01) { GetAbsoluteFilePath(CPath(R"(X:/home/file)"), R"(X:/home/file)"); }
TEST(Path, GetAbsoluteFilePath02) { GetAbsoluteFilePath(CPath(R"(X:/home/file.txt)"), R"(X:/home/file.txt)"); }
TEST(Path, GetAbsoluteFilePath03) { GetAbsoluteFilePath(CPath(R"(X:\home\file)"), R"(X:\home\file)"); }
TEST(Path, GetAbsoluteFilePath04) { GetAbsoluteFilePath(CPath(R"(X:\home\file.txt)"), R"(X:\home\file.txt)"); }
TEST(Path, GetAbsoluteFilePath05) { GetAbsoluteFilePath(CPath(R"(//computer/home/file)"), R"(//computer/home/file)"); }
TEST(Path, GetAbsoluteFilePath06) { GetAbsoluteFilePath(CPath(R"(//computer/home/file.txt)"), R"(//computer/home/file.txt)"); }
TEST(Path, GetAbsoluteFilePath07) { GetAbsoluteFilePath(CPath(R"(\\computer\home\file)"), R"(\\computer\home\file)"); }
TEST(Path, GetAbsoluteFilePath08) { GetAbsoluteFilePath(CPath(R"(\\computer\home\file.txt)"), R"(\\computer\home\file.txt)"); }
TEST(Path, GetAbsoluteFilePath09) { GetAbsoluteFilePath(CPath(R"(~/home/file)"), R"(~/home/file)"); }
TEST(Path, GetAbsoluteFilePath10) { GetAbsoluteFilePath(CPath(R"(~/home/file.txt)"), R"(~/home/file.txt)"); }
TEST(Path, GetAbsoluteFilePath11) { GetAbsoluteFilePath(CPath(R"(~\home\file)"), R"(~\home\file)"); }
TEST(Path, GetAbsoluteFilePath12) { GetAbsoluteFilePath(CPath(R"(~\home\file.txt)"), R"(~\home\file.txt)"); }
TEST(Path, GetAbsoluteFilePath13) { GetAbsoluteFilePath(CPath(R"(/home/file)"), GetCurrentWorkingDirectory() + R"(/home/file)"); }
TEST(Path, GetAbsoluteFilePath14) { GetAbsoluteFilePath(CPath(R"(/home/file.txt)"), GetCurrentWorkingDirectory() + R"(/home/file.txt)"); }
TEST(Path, GetAbsoluteFilePath15) { GetAbsoluteFilePath(CPath(R"(\home\file)"), GetCurrentWorkingDirectory() + R"(\home\file)"); }
TEST(Path, GetAbsoluteFilePath16) { GetAbsoluteFilePath(CPath(R"(\home\file.txt)"), GetCurrentWorkingDirectory() + R"(\home\file.txt)"); }
TEST(Path, GetAbsoluteFilePath17) { GetAbsoluteFilePath(CPath(R"(home/file)"), GetCurrentWorkingDirectory() + R"(/home/file)"); }
TEST(Path, GetAbsoluteFilePath18) { GetAbsoluteFilePath(CPath(R"(home/file.txt)"), GetCurrentWorkingDirectory() + R"(/home/file.txt)"); }
TEST(Path, GetAbsoluteFilePath19) { GetAbsoluteFilePath(CPath(R"(home\file)"), GetCurrentWorkingDirectory() + R"(\home\file)"); }
TEST(Path, GetAbsoluteFilePath20) { GetAbsoluteFilePath(CPath(R"(home\file.txt)"), GetCurrentWorkingDirectory() + R"(\home\file.txt)"); }

void GetRelativeFilePath(const CPath& path, std::string relative_file_path)
{
	replace_all(relative_file_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	ASSERT_EQ(path.GetRelativeFilePath(), relative_file_path);
}

TEST(Path, GetRelativeFilePath01) { GetRelativeFilePath(CPath(R"(//computer/home/file)"), R"(//computer/home/file)"); }
TEST(Path, GetRelativeFilePath02) { GetRelativeFilePath(CPath(R"(//computer/home/file.txt)"), R"(//computer/home/file.txt)"); }
TEST(Path, GetRelativeFilePath03) { GetRelativeFilePath(CPath(R"(\\computer\home\file)"), R"(\\computer\home\file)"); }
TEST(Path, GetRelativeFilePath04) { GetRelativeFilePath(CPath(R"(\\computer\home\file.txt)"), R"(\\computer\home\file.txt)"); }
TEST(Path, GetRelativeFilePath05) { GetRelativeFilePath(CPath(R"(~/home/file)"), R"(~/home/file)"); }
TEST(Path, GetRelativeFilePath06) { GetRelativeFilePath(CPath(R"(~/home/file.txt)"), R"(~/home/file.txt)"); }
TEST(Path, GetRelativeFilePath07) { GetRelativeFilePath(CPath(R"(~\home\file)"), R"(~\home\file)"); }
TEST(Path, GetRelativeFilePath08) { GetRelativeFilePath(CPath(R"(~\home\file.txt)"), R"(~\home\file.txt)"); }
TEST(Path, GetRelativeFilePath09) { GetRelativeFilePath(CPath(R"(/home/file)"), R"(home/file)"); }
TEST(Path, GetRelativeFilePath10) { GetRelativeFilePath(CPath(R"(/home/file.txt)"), R"(home/file.txt)"); }
TEST(Path, GetRelativeFilePath11) { GetRelativeFilePath(CPath(R"(\home\file)"), R"(home\file)"); }
TEST(Path, GetRelativeFilePath12) { GetRelativeFilePath(CPath(R"(\home\file.txt)"), R"(home\file.txt)"); }

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
}

TEST(Path, Folder_Relative1)
{
	std::string path_string          = "home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home/";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(Path, Folder_Relative2)
{
	std::string path_string          = "etc/../home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home/";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(Path, Folder_Absolute1)
{
	std::string path_string          = GetCurrentWorkingDirectory() + "/home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string folder_name          = "home/";
	std::string relative_folder_path = "home/";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(Path, Folder_Absolute2)
{
	std::string path_string          = R"(~\home\)";
	std::string absolute_folder_path = R"(~\home\)";
	std::string folder_name          = R"(home\)";
	std::string relative_folder_path = R"(~\home\)";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}

TEST(Path, Folder_Absolute3)
{
	std::string path_string          = R"(\\computer\home\)";
	std::string absolute_folder_path = R"(\\computer\home\)";
	std::string folder_name          = R"(home\)";
	std::string relative_folder_path = R"(\\computer\home\)";

	Test_Folder(path_string, absolute_folder_path, folder_name, relative_folder_path);
}


void Test_File(
	std::string path_string,
	std::string raw_file_name,
	std::string full_file_name,
	std::string extension,
	std::string absolute_file_path,
	std::string relative_file_path,
	std::string folder_path,
	std::string absolute_folder_path,
	std::string relative_folder_path)
{
	replace_all(path_string         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(raw_file_name       , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(full_file_name      , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(extension           , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(absolute_file_path  , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(relative_file_path  , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(folder_path         , WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(absolute_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);
	replace_all(relative_folder_path, WRONG_FOLDER_SEPARATOR, FOLDER_SEPARATOR);

	CPath path(path_string);

	ASSERT_EQ(path.IsFile(), true);
	ASSERT_EQ(path.IsFolder(), false);

	ASSERT_EQ(path.GetRawFileName(), raw_file_name);
	ASSERT_EQ(path.GetFullFileName(), full_file_name);
	ASSERT_EQ(path.GetExtension(), extension);
	ASSERT_EQ(path.GetAbsoluteFilePath(), absolute_file_path);
	ASSERT_EQ(path.GetRelativeFilePath(), relative_file_path);

	ASSERT_EQ(path.GetFolderName(), folder_path);
	ASSERT_EQ(path.GetAbsoluteFolderPath(), absolute_folder_path);
	ASSERT_EQ(path.GetRelativeFolderPath(), relative_folder_path);
}

TEST(Path, File_Relative1)
{
	std::string path_string		     = "home/text.txt";
	std::string raw_file_name	     = "text";
	std::string full_file_name	     = "text.txt";
	std::string extension		     = "txt";
	std::string absolute_file_path   = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string relative_file_path   = "home/text.txt";
	std::string folder_path			 = "home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string relative_folder_path = "home/";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path, folder_path, absolute_folder_path, relative_folder_path);
}

TEST(Path, File_Relative2)
{
	std::string path_string		     = "text.txt";
	std::string raw_file_name	     = "text";
	std::string full_file_name	     = "text.txt";
	std::string extension		     = "txt";
	std::string absolute_file_path   = GetCurrentWorkingDirectory() + "/text.txt";
	std::string relative_file_path   = "text.txt";
	std::string folder_path			 = CPath(GetCurrentWorkingDirectory() + "/").GetFolderName();
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/";
	std::string relative_folder_path = "";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path, folder_path, absolute_folder_path, relative_folder_path);
}

TEST(Path, File_Relative3)
{
	std::string path_string		     = "home/../text.txt";
	std::string raw_file_name	     = "text";
	std::string full_file_name	     = "text.txt";
	std::string extension		     = "txt";
	std::string absolute_file_path   = GetCurrentWorkingDirectory() + "/text.txt";
	std::string relative_file_path   = "text.txt";
	std::string folder_path			 = CPath(GetCurrentWorkingDirectory() + "/").GetFolderName();
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/";
	std::string relative_folder_path = "";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path, folder_path, absolute_folder_path, relative_folder_path);
}

TEST(Path, File_Absolute1)
{
	std::string path_string		     = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string raw_file_name	     = "text";
	std::string full_file_name	     = "text.txt";
	std::string extension		     = "txt";
	std::string absolute_file_path   = GetCurrentWorkingDirectory() + "/home/text.txt";
	std::string relative_file_path   = "home/text.txt";
	std::string folder_path			 = "home/";
	std::string absolute_folder_path = GetCurrentWorkingDirectory() + "/home/";
	std::string relative_folder_path = "home/";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path, folder_path, absolute_folder_path, relative_folder_path);
}

TEST(Path, File_Absolute2)
{
	std::string path_string		     = "~/home/text.txt";
	std::string raw_file_name	     = "text";
	std::string full_file_name	     = "text.txt";
	std::string extension		     = "txt";
	std::string absolute_file_path   = "~/home/text.txt";
	std::string relative_file_path   = "~/home/text.txt";
	std::string folder_path			 = "home/";
	std::string absolute_folder_path = "~/home/";
	std::string relative_folder_path = "~/home/";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path, folder_path, absolute_folder_path, relative_folder_path);
}

TEST(Path, File_Absolute3)
{
	std::string path_string		     = "//home/text.txt";
	std::string raw_file_name	     = "text";
	std::string full_file_name	     = "text.txt";
	std::string extension		     = "txt";
	std::string absolute_file_path   = "//home/text.txt";
	std::string relative_file_path   = "//home/text.txt";
	std::string folder_path			 = "home/";
	std::string absolute_folder_path = "//home/";
	std::string relative_folder_path = "//home/";

	Test_File(path_string, raw_file_name, full_file_name, extension, absolute_file_path, relative_file_path, folder_path, absolute_folder_path, relative_folder_path);
}