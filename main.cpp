#define _CRT_SECURE_NO_WARNINGS

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include <stdio.h>
#include <random>
#include "math.h"

#define SEED() 1337 // set to 0 for non determinism

static const int c_imageSize = 128;

float CalculateScore(const std::vector<float2>& points, size_t pointIndex)
{
	float ret = FLT_MAX;

	for (size_t i = 0; i < points.size(); ++i)
	{
		if (i == pointIndex)
			continue;
		ret = std::min(ret, LengthToroidal(points[i] - points[pointIndex]));
	}

	return ret;
}

void SavePoints(const char* outFileNameBase, int index, const std::vector<float2>& points)
{
	// Save an image
	{
		std::vector<unsigned char> pixels(c_imageSize * c_imageSize, 255);
		for (const float2& p : points)
		{
			int x = Clamp(int(p[0] * float(c_imageSize)), 0, c_imageSize - 1);
			int y = Clamp(int(p[1] * float(c_imageSize)), 0, c_imageSize - 1);

			pixels[y * c_imageSize + x] = 0;
		}

		char fileName[2048];
		sprintf_s(fileName, "%s.%i.png", outFileNameBase, index);
		stbi_write_png(fileName, c_imageSize, c_imageSize, 1, pixels.data(), 0);
	}

	// Save a text file
	{
		char fileName[2048];
		sprintf_s(fileName, "%s.%i.txt", outFileNameBase, index);

		FILE* file = nullptr;
		fopen_s(&file, fileName, "wb");

		for (const float2& p : points)
			fprintf(file, "%f, %f\n", p[0], p[1]);
		fclose(file);
	}
}

void DoTest(const char* outFileNameBase, size_t startCount, size_t endCount, unsigned int seed)
{
	printf("%s... %zu to %zu\n", outFileNameBase, startCount, endCount);

	// initialize the points
	std::mt19937 rng(seed);
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	std::vector<float2> points(startCount);
	for (float2& f : points)
	{
		f[0] = dist(rng);
		f[1] = dist(rng);
	}

	// create memory to store the scores of each point
	std::vector<float> pointScores(startCount, 0.0f);

	// Save the initial state
	SavePoints(outFileNameBase, 0, points);

	while (points.size() > endCount)
	{
		printf("\r%zu     ", points.size());

		// Calculate the scores for each point
		#pragma omp parallel for
		for (int candidateIndex = 0; candidateIndex < (int)points.size(); ++candidateIndex)
			pointScores[candidateIndex] = CalculateScore(points, candidateIndex);

		// find the point with the worst score
		float worstCandidateScore = FLT_MAX;
		size_t worstCandidateIndex = 0;
		for (size_t candidateIndex = 0; candidateIndex < points.size(); ++candidateIndex)
		{
			if (pointScores[candidateIndex] < worstCandidateScore)
			{
				worstCandidateIndex = candidateIndex;
				worstCandidateScore = pointScores[candidateIndex];
			}
		}

		points.erase(points.begin() + worstCandidateIndex);
	}
	printf("\r%zu     \n", points.size());

	SavePoints(outFileNameBase, 1, points);
}

int main(int argc, char** argv)
{
	unsigned int seed = SEED();
	if (!seed)
	{
		std::random_device rd;
		seed = rd();
	}

	DoTest("out/5000to50", 5000, 50, seed);
	DoTest("out/500to50", 500, 50, seed);
	DoTest("out/5000to250", 5000, 250, seed);
	DoTest("out/5000to500", 5000, 500, seed);

	return 0;
}


/*
TODO:
* calculate distance toroidally
* DFT the point sets. with python? maybe use blob to iterate the files and DFT them all instead of specifying manually.
? can it support density? the paper talks about that i think.
* could make this a command line utility perhaps. doesn't really make sense to unless it's high quality and re-usable though. it would also have to take an input point set.
* write out to a text file (.h?) and also draw the image. Maybe show both starting and final image?
* if you are trying to find the point with a closest, closest point, you will find both points in the pair. maybe read the paper to see what they do. std dev or something?
* could parallelize score calculation of each point.
* could try initializing to stratified points instead of pure white noise.

Notes:
* using a reverse mitchell's best candidate to find the points to remove.
 * could do more like void and cluster where you splat down gaussian energy to find the densest place. kind of caches energy values spatially.
* motivation - sometimes you can't choose your points, but you can pick a subset of the points
* the more points you start with, the better the results you can get
* "Virtual Blue Noise Lighting" https://dl.acm.org/doi/10.1145/3543872 does something like this (should read more deeply)
* compare vs gaussian blue noise? could even provide the binaries, of the public stuff! i expect it doesn't do as well

* This is the paper that introduced this, and supports adaptive sampling (varying density)
 * http://www.cemyuksel.com/research/sampleelimination/

*/