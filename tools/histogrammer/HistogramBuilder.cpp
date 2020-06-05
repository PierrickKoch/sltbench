#include "HistogramBuilder.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>


void BuildHistogramFor(void(*fun)(), const std::string& filename)
{
	// get results
	std::vector<int64_t> results;
	const size_t count = 1000;
	results.resize(count, 0);
	for (size_t i = 0; i < count; ++i)
	{
		auto start_time = std::chrono::high_resolution_clock::now();
		fun();
		auto final_time = std::chrono::high_resolution_clock::now();
		results[i] = (final_time - start_time).count();
	}
	std::sort(results.begin(), results.end());

	// out them to .dat-file
	{
		const auto datfile = filename + ".dat";
		std::ofstream of(datfile);
		if (!of)
			throw std::runtime_error("ERROR: failed to open " + datfile + " on write");

		std::copy(results.begin(), results.end(), std::ostream_iterator<int64_t>(of, "\n"));
	}

	// prepare gnuplot histogram builder script
	{
		const auto pltfile = filename + ".plt";
		std::ofstream of(pltfile);
		if (!of)
			throw std::runtime_error("ERROR: failed to open " + pltfile + " on write");

		of << "reset\n";
		of << "n=100\n";
		of << "max=" << results.back() << '\n';
		of << "min=" << results.front() << '\n';
		of << "width=(max-min)/n\n";
		of << "hist(x, width) = width*floor(x / width) + width / 2.0\n";
		of << "set term png\n";
		of << "set output \"" << filename + ".png" << "\"\n";
		of << "set xrange[min:max]\n";
		of << "set yrange[0:]\n";
		of << "set offset graph 0.05, 0.05, 0.05, 0.0\n";
		of << "set xtics min, (max - min) / 5, max\n";
		of << "set boxwidth width*0.9\n";
		of << "set style fill solid 0.5\n";
		of << "set tics out nomirror\n";
		of << "plot \"" << filename + ".dat" << "\" u(hist($1, width)) :(1.0) smooth freq w boxes lc rgb\"blue\" notitle\n";
	}
}

