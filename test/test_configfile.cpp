#include "configfile.h"
#include "gtest/gtest.h"

TEST (ConfigfileTest, NoParameters) {
	
	CConfigurations config = CConfigurations();
	ASSERT_EQ (config.Verbosity, 1);
}

TEST (ConfigfileTest, OneParameter) {
	
	std::string ProgramFolderPath = "D:" + FOLDER_SEPARATOR + "user" + FOLDER_SEPARATOR + "Username" + FOLDER_SEPARATOR;
	std::string ProgramFilePath = ProgramFolderPath + "file.exe";
	CConfigurations config = CConfigurations(ProgramFilePath);
	
	ASSERT_EQ (config.Verbosity, 1);
	ASSERT_EQ (config.HasConfiguration("ProgramFilePath"), true);
	ASSERT_EQ (config.GetConfiguration("ProgramFilePath"), ProgramFilePath);
	ASSERT_EQ (config.HasConfiguration("ProgramFolderPath"), true);
	ASSERT_EQ (config.GetConfiguration("ProgramFolderPath"), ProgramFolderPath);
}

TEST (ConfigfileTest, TwoParameters) {
	
	std::string ProgramFolderPath = "D:" + FOLDER_SEPARATOR + "user" + FOLDER_SEPARATOR + "Username" + FOLDER_SEPARATOR;
	std::string ProgramFilePath = ProgramFolderPath + "file.exe";
	std::string ConfigFilePath = "D:" + FOLDER_SEPARATOR + "user" + FOLDER_SEPARATOR + "Username" + FOLDER_SEPARATOR + "config.ini";
	CConfigurations config = CConfigurations(ProgramFilePath, ConfigFilePath);
	
	ASSERT_EQ (config.Verbosity, 1);
	ASSERT_EQ (config.HasConfiguration("ProgramFilePath"), true);
	ASSERT_EQ (config.GetConfiguration("ProgramFilePath"), ProgramFilePath);
	ASSERT_EQ (config.HasConfiguration("ProgramFolderPath"), true);
	ASSERT_EQ (config.GetConfiguration("ProgramFolderPath"), ProgramFolderPath);
}

TEST (ConfigfileTest, GlobalSingleton) {
	std::string ProgramFolderPath = "D:" + FOLDER_SEPARATOR + "user" + FOLDER_SEPARATOR + "Username" + FOLDER_SEPARATOR;
	std::string ProgramFilePath = ProgramFolderPath + "file.exe";
	gConfigurations = CConfigurations(ProgramFilePath);
	
	ASSERT_EQ (gConfigurations.Verbosity, 1);
	ASSERT_EQ (gConfigurations.HasConfiguration("ProgramFilePath"), true);
	ASSERT_EQ (gConfigurations.GetConfiguration("ProgramFilePath"), ProgramFilePath);
	ASSERT_EQ (gConfigurations.HasConfiguration("ProgramFolderPath"), true);
	ASSERT_EQ (gConfigurations.GetConfiguration("ProgramFolderPath"), ProgramFolderPath);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);	
	return RUN_ALL_TESTS();
}
