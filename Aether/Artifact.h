#pragma once
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <string>

namespace Aether {

	class Artifact;
	typedef std::string STR;		//Human readable Token (unique Artifact ID : UID)
	typedef Artifact* LINK;		
	typedef std::set<LINK> ARTIFACTS;			
	typedef size_t HASH;
	typedef std::pair<HASH, LINK> HASHED_LINK;			//ordered set of unique Artifacts [&a1, &a2, &a3]
	typedef std::unordered_map<HASH, LINK> AETHER;		//([un]ordered) set of UAIDs [ &[&a1, &a2, &a3] , &[&b1, &b2, &b3] ]

	const STR dependencyChar = "->";

	class Artifact
	{
	public:
		static LINK createArtifact(STR NAME) {
			LINK art = findArtifact(NAME);
			if (art == nullptr) {
				art = new Artifact(NAME);
				aether.insert(HASHED_LINK(art->getHash(NAME), art));
			}
			return art;
		}

		static LINK findArtifact(STR NAME) {
			auto elem = aether.find(Artifact::getHash(NAME));
			if (elem != aether.end()) {
				return elem->second;
			}
			return nullptr;
		}

		size_t getHash() {
			return getHash(name);
		}

		static HASH getHash(STR name) {
			std::hash<STR> hasher;
			return hasher(name);
		}

	protected:
		static AETHER aether;

		Artifact(STR NAME) {
			setNAME(NAME);
		}

		LINK setNAME(STR name) {
			this->name = name;
			STR parent = "";
			size_t ix = -1;
			do {
				ix = name.find_last_of(dependencyChar);
				if (ix != -1) {
					parent = name.substr(0, ix - 1);
					if (parent == this->name) continue;

					LINK link = findArtifact(parent);
					if (link == nullptr) {
						link = createArtifact(parent);
					}
					link->addDependee(this);
					addDependency(link);

					name = name.substr(0, ix - 1);
				}
			} while (ix != -1);
			//UAID = this;
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

		STR name; //com->phi->vec3
		//LINK UAID; //com->phi->vec3
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