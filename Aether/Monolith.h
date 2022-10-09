#pragma once
#include "Artifact.h"

#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

#include <unordered_set>


namespace Aether {
	//Code Generator & Artifact Interpreter
	//static std::unordered_set<STR> labels({ "+", "-", "*", "/", "%", "//", "++", "--", "+=", "-=", "=",}); labels are all that are undefined
	const STR folderLabel = "./";
	const STR fileLabel = "/";
	const STR startBlock = "{\n";
	const STR endBlock = "}\n";

	class Monolith
	{
	public:
		static Monolith init(STR rootFolder) {
			static Monolith instance = Monolith(rootFolder);
			return instance;
		}

		~Monolith() = default;
	private:
		int blockCount = 0;
		bool lastFolder = false;

		STR root;
		STR monolith;
		std::vector<STR> folderStack;
		Monolith() = default;
		Monolith(STR rootFolder) {
			//Initialize codespace:
			std::cout << "INIT MONOLITH" << std::endl << std::endl;
			root = rootFolder;
			std::cout << "Root Folder: " + root << std::endl;
			parseFolder(root);
			printMonolith();
			Artifact::printAether();
			saveMonolith();
	
		}

		void printMonolith()
		{
			std::cout << "> > > Monolith < < < " << std::endl << std::endl;
			std::cout << monolith << std::endl;
		}

		void saveMonolith()
		{
			std::ofstream outfile;
			outfile.open("source.monolith");
			outfile << monolith << std::endl;
			outfile.close();
		}

		void parseDirectory(STR dir) {
			for (const auto& entry : fs::directory_iterator(dir)) {
				STR path = entry.path().string();
				parseObj(path);
			}
		}

		void parseObj(STR object) {
			//std::cout << "Parse object: " + object << std::endl;
			if (fs::is_regular_file(object)) {
				parseFile(object);
			}
			else {
				parseFolder(object);
			}
		}

		void parseFile(STR object) {
			if (lastFolder) blockCount += 1;
			STR NAME = object.substr(object.find_last_of('\\') + 1);
			STR obj = fileLabel + NAME + startBlock;

			createFileArtifact(NAME);

			appendIddented(obj);
			appendIddented("\n", 1);
			appendIddented(endBlock, -1);
			lastFolder = false;
		}

		void parseFolder(STR object) {

			if (lastFolder) blockCount += 1;
			STR NAME = object.substr(object.find_last_of('\\') + 1);
			STR obj = folderLabel + NAME + startBlock;

			createFolderArtifact(NAME);

			appendIddented(obj);
			lastFolder = true;
			parseDirectory(object);
			appendIddented(endBlock, -1);
			folderStack.pop_back();
		}

		void createFolderArtifact(STR NAME) {
			folderStack.push_back(NAME);
			STR folderID;
			for (int i = 0; i < folderStack.size(); i++) {
				folderID += folderStack[i];
				if (i < folderStack.size() - 1) folderID += Aether::linkChar;
			}
			Artifact::createArtifact(folderID);
		}

		void createFileArtifact(STR name) {
			STR folderID;
			for (int i = 0; i < folderStack.size(); i++) {
				folderID += folderStack[i];
				if (i < folderStack.size() - 1) folderID += Aether::linkChar;
			}
			Artifact::createArtifact(folderID + Aether::linkChar + name);
		}

		void appendIddented(STR value, int offset = 0) {
			blockCount += offset;
			if (blockCount < 0) blockCount = 0;
			for (int i = 0; i < blockCount; i++)
				monolith += ('\t');
			monolith.append(value);
		}

		STR dumpFileToStr(STR path) {
			std::ifstream t(path);
			std::string str;

			t.seekg(0, std::ios::end);
			str.reserve(t.tellg());
			t.seekg(0, std::ios::beg);

			str.assign((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());
			return str;
		}
	};
}
