#pragma once

#include "d3dUtil.h"
#include <map>
#include "Vertex.h"
#include "MeshGeometry.h"

class OBJLoader
{
	struct HashEntry
	{
		HashEntry* pNext;
		int index; //the index of the vertex this entry represents
	};

public: 

	static const UINT MAX_VERTICES;

public:
	OBJLoader(void);
	~OBJLoader(void);

	static bool LoadOBJ(ID3D11Device* device, std::string filename, 
		std::vector<Vertex::NormalTexVertex>& verices, 
		std::vector<UINT>& indices, 
		std::vector<MeshGeometry::Subset>& subsets,
		bool isRHS, bool uvFlipped);


private:
	static void LoadMTL(ID3D11Device* device, std::wstring filename, std::vector<MeshGeometry::Subset>& subsets);

	static UINT AddVertex(Vertex::NormalTexVertex, std::vector<Vertex::NormalTexVertex>& vertBuf,
						  std::map<UINT, HashEntry*>& hashTable, UINT hashvalue);

	static UINT GetSubset(std::wstring name, const std::vector<MeshGeometry::Subset>& subsets);

};

