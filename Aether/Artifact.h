#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <string>

namespace Aether {

	class Artifact;
	typedef std::string STR;		//Human readable Token (unique Artifact ID : UID)
	typedef Artifact* LINK;		
	typedef size_t HASH;
	typedef std::vector<LINK> UAID;
	typedef std::set<LINK> ARTIFACTS;
	typedef std::pair<HASH, LINK> HASHED_LINK;			//ordered set of unique Artifacts [&a1, &a2, &a3]
	typedef std::unordered_map<HASH, LINK> AETHER;		//([un]ordered) set of UAIDs [ &[&a1, &a2, &a3] , &[&b1, &b2, &b3] ]

	const STR linkChar = "::";

	class Artifact
	{
	public:
		static LINK createArtifact(STR NAME) {
			LINK art = findArtifact(NAME);
			if (art == nullptr) {
				art = new Artifact(NAME);
			}
			return art;
		}

		static LINK findArtifact(STR NAME) {
			auto elem = aether.find(Artifact::getHash(NAME));
			return elem != aether.end() ? elem->second : nullptr;
		}
		
		ARTIFACTS* getDependencies() {
			return &dependencies;
		}

		ARTIFACTS* getDependees() {
			return &dependees;
		}

		static void printAether() {
			std::cout << ">>> AETHER ARTIFACTS <<< " << std::endl << std::endl;

			for (AETHER::iterator it = aether.begin(); it != aether.end(); it++)
			{
				ARTIFACTS* dependencies = it->second->getDependencies();
				for (ARTIFACTS::iterator _it = dependencies->begin(); _it != dependencies->end(); _it++)
				{
					//std::cout << dependencies->find(*_it) << std::endl;
				}

				ARTIFACTS* dependees = it->second->getDependees();
				for (ARTIFACTS::iterator _it = dependees->begin(); _it != dependees->end(); _it++)
				{

				}

			}

		}

	protected:

		size_t getHash() {
			return getHash(getName());
		}

		static HASH getHash(STR name) {
			HASH hash = 0;
			std::hash<STR> hasher;
			return hasher(name);
		}

		static AETHER aether;

		Artifact(STR NAME) {
			setNAME(NAME);
			aether.insert(HASHED_LINK(this->getHash(), this));
		}

		STR getName() {
			STR nameRefSequence = ""; //human readable UAID : name1->name2->name3->name4
			for (UAID::iterator it = uniqueArtifactID.begin(); it != uniqueArtifactID.end(); it++) {
				nameRefSequence += (*it)->Name;
				if (it != --uniqueArtifactID.end()) nameRefSequence += linkChar;
			};
			return nameRefSequence;
		}

		LINK setNAME(STR name) {
			this->Name = name;
			STR parent = "";
			size_t ix = -1;
			do {
				ix = name.find_last_of(linkChar);
				if (ix != -1) {
					parent = name.substr(0, ix - 1);

					LINK link = findArtifact(parent); //validates existance of UAIDs
					if (!link) throw std::runtime_error("INVALID LINK");
					link->addDependee(this);
					//addDependency(link);
					uniqueArtifactID.insert(uniqueArtifactID.begin(),link);
					name = name.substr(0, ix - 1);
				}
			} while (ix != -1);
			ix = Name.find_last_of(linkChar);
			if (ix != -1)
				this->Name = Name.substr(ix+linkChar.size()-1);
			uniqueArtifactID.insert(uniqueArtifactID.end(),this);
 			return this;
		}

		void addLabel(LINK label) {
		};
		void addDependency(LINK dependency) {
			dependencies.insert(dependency);
		};
		void addDependee(LINK dependee) {
			dependees.insert(dependee);
		};

		STR Name; //human readable name representing self Name : name4 (hash?)
		UAID uniqueArtifactID; //Name ref sequence [&name1,&name2,&name3,&name4]
		ARTIFACTS labels; //[Type, float]
		ARTIFACTS dependencies;
		ARTIFACTS dependees;
		ARTIFACTS inheritances;
	};

	AETHER Artifact::aether = AETHER();

}


/*namespace std {
	template <>
	struct hash<size_t> {
		size_t operator ()(Aether::STR& value) const {
			return Aether::Artifact::getHash(value);
		}
	};
}*/