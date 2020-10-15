#include "PrimitiveHelper.h"

#include <cstdarg>
#include <random>


NW_NAMESPACE_BEGIN

namespace PrimitiveHelper
{
	void FillIndices(Index(&_indices)[], uint32 _count, ...)
	{
		va_list args;
		va_start(args, _count);

		for (uint32 i = 0; i < _count; ++i)
		{
			Index index = va_arg(args, Index);
			_indices[i] = index;
		}

		va_end(args);
	}


	void GenerateQuad(Vertex_P(&_verices)[4], Index(&_indices)[6])
	{
		_verices[0].SetPosition(-0.5f, -0.5f, 0.5f);
		_verices[1].SetPosition(0.5f, -0.5f, 0.5f);
		_verices[2].SetPosition(-0.5f, 0.5f, 0.5f);
		_verices[3].SetPosition(0.5f, 0.5f, 0.5f);

		FillIndices((Index(&)[])_indices, 6, 
			0, 3, 2, 
			0, 1, 3);
	}

	void GenerateQuad(Vertex_C(&_verices)[4], Index(&_indices)[6])
	{
		_verices[0].SetPosition(-0.5f, -0.5f, 0.5f);
		_verices[1].SetPosition(0.5f, -0.5f, 0.5f);
		_verices[2].SetPosition(-0.5f, 0.5f, 0.5f);
		_verices[3].SetPosition(0.5f, 0.5f, 0.5f);

		std::mt19937_64 rng;
		uint64 timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
		rng.seed(ss);

		std::uniform_real_distribution<float> unif(0.0f, 1.0f);

		_verices[0].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[1].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[2].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[3].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);

		FillIndices((Index(&)[])_indices, 6,
			0, 3, 2,
			0, 1, 3);
	}

	void GenerateCube(Vertex_P(&_verices)[8], Index(&_indices)[36])
	{
		_verices[0].SetPosition(-0.5f, -0.5f, 0.5f);
		_verices[1].SetPosition(0.5f, -0.5f, 0.5f);
		_verices[2].SetPosition(-0.5f, 0.5f, 0.5f);
		_verices[3].SetPosition(0.5f, 0.5f, 0.5f);
		_verices[4].SetPosition(-0.5f, -0.5f, -0.5f);
		_verices[5].SetPosition(0.5f, -0.5f, -0.5f);
		_verices[6].SetPosition(-0.5f, 0.5f, -0.5f);
		_verices[7].SetPosition(0.5f, 0.5f, -0.5f);

		FillIndices((Index(&)[])_indices, 36,
			0, 3, 2, 0, 1, 3,				//    6-----7     Y
			1, 7, 3, 1, 5, 7,				//   /|    /|     ^
			5, 4, 7, 6, 7, 4,				//  2-----3 |     |
			0, 6, 4, 0, 2, 6,				//  | 4 --|-5     ---> X
			2, 7, 6, 2, 3, 7,				//  |/    |/     /
			4, 1, 0, 4, 5, 1);
	}

	void GenerateCube(Vertex_C(&_verices)[8], Index(&_indices)[36])
	{
		_verices[0].SetPosition(-0.5f, -0.5f, 0.5f);
		_verices[1].SetPosition(0.5f, -0.5f, 0.5f);
		_verices[2].SetPosition(-0.5f, 0.5f, 0.5f);
		_verices[3].SetPosition(0.5f, 0.5f, 0.5f);
		_verices[4].SetPosition(-0.5f, -0.5f, -0.5f);
		_verices[5].SetPosition(0.5f, -0.5f, -0.5f);
		_verices[6].SetPosition(-0.5f, 0.5f, -0.5f);
		_verices[7].SetPosition(0.5f, 0.5f, -0.5f);

		std::mt19937_64 rng;
		uint64 timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
		rng.seed(ss);

		std::uniform_real_distribution<float> unif(0.0f, 1.0f);

		_verices[0].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[1].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[2].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[3].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[4].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[5].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[6].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);
		_verices[7].SetColor(unif(rng), unif(rng), unif(rng), 1.0f);

		FillIndices((Index(&)[])_indices, 36,
			0, 3, 2, 0, 1, 3,				//    6-----7     Y
			1, 7, 3, 1, 5, 7,				//   /|    /|     ^
			5, 4, 7, 6, 7, 4,				//  2-----3 |     |
			0, 6, 4, 0, 2, 6,				//  | 4 --|-5     ---> X
			2, 7, 6, 2, 3, 7,				//  |/    |/     /
			4, 1, 0, 4, 5, 1);
	}

};

NW_NAMESPACE_END