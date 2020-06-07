#include "gtest/gtest.h"

#include <sltbench/Bench.h>


namespace {

size_t g_fixtures_made_count = 0;
size_t g_make_fixture_arg = 0;

int make_fixture(const size_t& arg)
{
	++g_fixtures_made_count;
	g_make_fixture_arg = arg;
	return 42;
}

class IncGenerator
{
public:
	typedef size_t ArgType;

	IncGenerator(int, char **) {}

public:
	size_t Generate()
	{
		++i_;
		return i_;
	}

private:
	size_t i_ = 0;
};

void stub_func(int&, const size_t&) {}

size_t g_calls_count = false;

void inc_calls_count(int&, const size_t&)
{
	++g_calls_count;
}

int g_fix = 0;
size_t g_arg = 0;

void set_fix_arg(int& fix, const size_t& arg)
{
	g_fix = fix;
	g_arg = arg;
}

} // namespace

template<typename Fixture, typename Generator>
using BM = sltbench::Benchmark_FB_LAG<Fixture, Generator>;

TEST(Benchmark_FB_LAG, GetNameShouldReturnBenchmarkName)
{
	BM<int, IncGenerator> bm("name", &stub_func, &make_fixture);

	EXPECT_EQ("name", std::string(bm.name));
}

TEST(Benchmark_FB_LAG, MeasureCallsFunction)
{
	g_calls_count = 0;
	g_fixtures_made_count = 0;
	BM<int, IncGenerator> bm("name", &inc_calls_count, &make_fixture);

	bm.Prepare();
	bm.Measure(1u);

	EXPECT_EQ(1u, g_calls_count);
	EXPECT_EQ(1u, g_fixtures_made_count);
}

TEST(Benchmark_FB_LAG, MeasureCallsFunctionExactlyOnce)
{
	g_calls_count = 0;
	g_fixtures_made_count = 0;
	BM<int, IncGenerator> bm("name", &inc_calls_count, &make_fixture);

	bm.Prepare();
	bm.Measure(3u);

	EXPECT_EQ(1u, g_calls_count);
	EXPECT_EQ(1u, g_fixtures_made_count);
}

TEST(Benchmark_FB_LAG, MeasureCallsFunctionWithFixtureValue)
{
	g_fix = 0;
	g_arg = 0;
	BM<int, IncGenerator> bm("name", &set_fix_arg, &make_fixture);

	bm.Prepare();
	bm.Measure(1u);

	EXPECT_EQ(1, g_arg);
	EXPECT_EQ(42, g_fix);
}

TEST(Benchmark_FB_LAG, HasArgsToProcessReturnsTrueAfterPrepare)
{
	BM<int, IncGenerator> bm("name", &stub_func, &make_fixture);

	bm.Prepare();

	EXPECT_TRUE(bm.HasArgsToProcess());
}

TEST(Benchmark_FB_LAG, CurrentArgAsString)
{
	BM<int, IncGenerator> bm("name", &stub_func, make_fixture);

	bm.Prepare();

	EXPECT_EQ("1", bm.CurrentArgAsString());
}
