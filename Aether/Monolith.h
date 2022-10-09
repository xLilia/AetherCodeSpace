#pragma once
#include "Artifact.h"

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <unordered_set>


namespace Aether {
	//Code Generator & Artifact Interpreter
	static std::unordered_set<STR> filterFileExt({ ".h", ".cpp", ".hpp", ".c"});
	static std::unordered_set<STR> filterFolderExt({ "." });
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
		STR sourceMonolith;
		std::vector<STR> folderStack;
		Monolith() = default;
		Monolith(STR rootFolder) {
			//Initialize codespace:
			std::cout << "INIT MONOLITH" << std::endl << std::endl;
			root = rootFolder;
			std::cout << "Root Folder: " + root << std::endl;
			parseFolder(root);
			printMonolith();
			saveMonolith();
			Artifact::printAether();
			Artifact::saveAether();
		}

		void printMonolith()
		{
			std::cout << ">>> Monolith <<< " << std::endl << std::endl;
			std::cout << sourceMonolith << std::endl;
		}

		void saveMonolith()
		{
			std::ofstream outfile;
			outfile.open("source.monolith");
			outfile << sourceMonolith << std::endl;
			outfile.close();
		}

		void parseDirectory(STR dir) {
			for (const auto& entry : fs::directory_iterator(dir)) {
				STR path = entry.path().string();
				parseObj(path);
			}
		}

		void parseObj(STR object) {
			std::cout << "Parse object: " + object << std::endl;
			if (fs::is_regular_file(object)) {
				size_t ix = object.find_last_of('.');
				if (ix != -1) {
					STR ext = object.substr(ix);
					if (filterFileExt.find(ext) == filterFileExt.end()) return;
					parseFile(object);
				}
			}
			else if(fs::is_directory(object)) {
				parseFolder(object);
			}
		}

		void parseFile(STR object) {
			if (lastFolder) blockCount += 1;
			STR NAME = object.substr(object.find_last_of('\\') + 1);
			STR obj = fileLabel + NAME + startBlock;

			createFileArtifact(NAME);

			appendIddented(obj);

			STR fileContent = dumpFileToStr(object);
			appendIddented(fileContent + "\n", 1);

			appendIddented(endBlock, -1);
			lastFolder = false;
		}

		void parseFolder(STR object) {
			STR NAME = object.substr(object.find_last_of('\\') + 1);
			if (NAME.find(".") != -1) { return; };

			if (lastFolder) blockCount += 1;
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
				sourceMonolith += ('\t');
			sourceMonolith.append(value);
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
