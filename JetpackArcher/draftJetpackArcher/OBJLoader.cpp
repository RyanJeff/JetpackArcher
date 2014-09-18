#include "OBJLoader.h"
#include <fstream>

const UINT OBJLoader::MAX_VERTICES = 2000000;


std::wistream& operator>>(std::wistream& is, XMFLOAT3& v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

std::wistream& operator>>(std::wistream& is, XMFLOAT2& v)
{
	is >> v.x >> v.y;
	return is;
}

bool operator == (const XMFLOAT3& lh, const XMFLOAT3& rh)
{
	return lh.x == rh.x && lh.y == rh.y && lh.z == rh.z;
}

bool operator == (const XMFLOAT2& lh, const XMFLOAT2& rh)
{
	return lh.x == rh.x && lh.y == rh.y;
}

OBJLoader::OBJLoader(void)
{
}


OBJLoader::~OBJLoader(void)
{
}

bool OBJLoader::LoadOBJ(ID3D11Device* device, std::string filename, 
		std::vector<Vertex::NormalTexVertex>& vertices, 
		std::vector<UINT>& indices, 
		std::vector<MeshGeometry::Subset>& subsets,
		bool isRHS, bool uvFlipped)
{
	std::vector<std::vector<UINT>> subgroupIndices;
	std::vector<UINT> subgroup;

	subgroupIndices.push_back(subgroup);

	std::vector<XMFLOAT3> vertPos;
	vertPos.reserve(MAX_VERTICES);

	std::vector<XMFLOAT3> normals;
	vertPos.reserve(MAX_VERTICES);

	std::vector<XMFLOAT2> uvs;
	uvs.reserve(MAX_VERTICES);

	std::map<UINT,HashEntry*> hashTable;

	std::wifstream fin(filename.c_str());

	if(!fin)
	{
		OutputDebugString(L"Error loading mesh file.");
		return false;
	}

	UINT activeSubgroup = 0;

	std::wstring symbol;
	while(fin >> symbol)
	{

		if(symbol == L"#")
		{
		}
		else if(symbol == L"mtllib")
		{
			fin >> symbol;
			LoadMTL(device, symbol, subsets);

			//init the subgroup vectors
			for(int i = 1; i < subsets.size(); ++i)
			{
				subgroupIndices.push_back(subgroup);
			}
		}
		else if(symbol == L"v")
		{
			XMFLOAT3 newPos;
			fin >> newPos;

			vertPos.push_back(newPos);
		}
		else if(symbol == L"vn")
		{
			XMFLOAT3 newNorm;
			fin >> newNorm;

			normals.push_back(newNorm);
		}
		else if(symbol == L"vt")
		{
			XMFLOAT2 newUV;
			fin >> newUV;
			uvs.push_back(newUV);
		}
		else if(symbol == L"f")
		{
			for(int i = 0; i < 4; ++i)
			{
				if(fin.peek() == '\n')
				{
					break;
				}

				UINT tempPosIndex = 0;
				UINT tempTexIndex = 0;
				fin >> tempPosIndex;

				Vertex::NormalTexVertex newVertex;
				newVertex.pos = vertPos[tempPosIndex - 1];
				if(fin.peek() == '/')
				{
					fin.ignore();
					if(fin.peek() != '/')
					{
						fin >> tempTexIndex;
						newVertex.tex = uvs[tempTexIndex - 1];

						if(uvFlipped)
						{
							newVertex.tex.y = 1 - newVertex.tex.y;
						}
					}
					
					if(fin.peek() == '/')
					{
						fin.ignore();
						if(fin.peek() != '\n')
						{
							UINT tempNormalIndex = 0;
							fin >> tempNormalIndex;
							newVertex.normal = normals[tempNormalIndex - 1];
						}
					}
					
				}

				if(i == 3)
				{
					if(isRHS)
					{
						UINT subgroupSize = subgroupIndices[activeSubgroup].size();
						subgroupIndices[activeSubgroup].push_back
							(subgroupIndices[activeSubgroup][subgroupSize - 2]);
						subgroupIndices[activeSubgroup].push_back
							(subgroupIndices[activeSubgroup][subgroupSize - 3]);
					}
					else
					{
						UINT subgroupSize = subgroupIndices[activeSubgroup].size();
						subgroupIndices[activeSubgroup].push_back
							(subgroupIndices[activeSubgroup][subgroupSize - 3]);
						subgroupIndices[activeSubgroup].push_back
							(subgroupIndices[activeSubgroup][subgroupSize - 1]);
					}
				}

				subgroupIndices[activeSubgroup].push_back(
				AddVertex(newVertex, vertices, hashTable, tempPosIndex));

				if(i == 2 && isRHS)
				{
					UINT temp = subgroupIndices[activeSubgroup][subgroupIndices[activeSubgroup].size() - 2];
					subgroupIndices[activeSubgroup][subgroupIndices[activeSubgroup].size() - 2] = 
						subgroupIndices[activeSubgroup][subgroupIndices[activeSubgroup].size() - 1];
					subgroupIndices[activeSubgroup][subgroupIndices[activeSubgroup].size() - 1] = temp;
				}
			}
		}
		else if(symbol == L"usemtl")
		{
			std::wstring subsetName;
			fin >> subsetName;
			activeSubgroup = GetSubset(subsetName, subsets);
		}

		std::getline(fin, symbol);
	}
	
	for(std::map<UINT, HashEntry*>::iterator it = hashTable.begin();
		it != hashTable.end(); ++it)
	{

		HashEntry* curr = (*it).second;
		while(curr != NULL)
		{
			HashEntry* next = curr->pNext;
			delete curr;
			curr = next;
		}		
	}
	int indexCount = 0;
	//combine subgroup indices into one group
	for(int i = 0; i < subgroupIndices.size(); ++i)
	{
		indexCount += subgroupIndices[i].size();
	}

	if(indexCount > MAX_VERTICES)
	{
		return false;
	}

	indices.reserve(indexCount);

	for(int i = 0; i < subgroupIndices.size(); ++i)
	{
		subsets[i].faceCount = subgroupIndices[i].size() / 3;
		subsets[i].faceStartIndex = indices.size() / 3;
		subsets[i].id = i;
		subsets[i].vertexStart = 0;
		subsets[i].vertexCount = vertices.size();

		indices.insert(indices.end(), subgroupIndices[i].begin(), 
			subgroupIndices[i].end());
	}

	return true;
}

UINT OBJLoader::AddVertex(Vertex::NormalTexVertex vertex, std::vector<Vertex::NormalTexVertex>& vertBuf,
						  std::map<UINT, HashEntry*>& hashTable, UINT hashValue)
{
	HashEntry* currEntry = hashTable[hashValue];
	HashEntry* prevEntry = NULL;
	while(currEntry != NULL)
	{
		if(vertex.pos == vertBuf[currEntry->index].pos && 
			vertex.tex == vertBuf[currEntry->index].tex && 
			vertex.normal == vertBuf[currEntry->index].normal)
		{
			return currEntry->index;
		}
		prevEntry = currEntry;
		currEntry = currEntry->pNext;
	}

	currEntry = new HashEntry();
	currEntry->index = vertBuf.size(); 
	currEntry->pNext = NULL;

	if(prevEntry != NULL)
	{
		prevEntry->pNext = currEntry;
	}
	else
	{
		hashTable[hashValue] = currEntry;
	}

	vertBuf.push_back(vertex);
	
	return currEntry->index;
}

void OBJLoader::LoadMTL(ID3D11Device* device, std::wstring filename, std::vector<MeshGeometry::Subset>& subsets)
{
	std::wifstream fin(filename.c_str());

	if(!fin)
	{
		OutputDebugString(L"Error loading material file.");
	}

	std::wstring symbol;
	MeshGeometry::Subset newData;
	while(fin >> symbol)
	{
		if(symbol == L"#")
		{

		}
		else if(symbol == L"map_Kd")
		{
			fin >> symbol;
			
			HR(D3DX11CreateShaderResourceViewFromFile(device, 
				symbol.c_str(), 0, 0, &newData.srv, 0));
			subsets.push_back(newData);
			newData = MeshGeometry::Subset();
		}
		else if(symbol == L"newmtl")
		{
			fin >> symbol;
			newData.name = symbol;
		}

		std::getline(fin, symbol);
	}
}

UINT OBJLoader::GetSubset(std::wstring name, const std::vector<MeshGeometry::Subset>& subsets)
{
	for(int i = 0; i < subsets.size(); ++i)
	{
		if(subsets[i].name == name)
		{
			return i;
		}
	}

	return 0;
}