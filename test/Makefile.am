CPPFLAGS += -I$(top_srcdir)/deps -I$(top_srcdir)/src

LDADD = $(top_builddir)/libwarped.la

# With a default ext specified, we can skip defining SOURCES for a progarm
# that has only one file
AM_DEFAULT_SOURCE_EXT = .cpp

WARPED_TEST_PROGS = \
    test_AggregateEventStatistics \
    test_CommandLineConfiguration \
    test_Event \
    test_IndividualEventStatistics \
    test_LadderQueue \
    test_CircularQueue \
    test_SplayTree \
    test_LogicalProcess \
    test_LPState \
    test_ProfileGuidedPartitioner \
	test_RandomNumberGenerator \
    test_RoundRobinPartitioner \
    test_serialization \
    test_Simulation \
    test_STLLTSFQueue \
    test_TimeWarpAsynchronousGVTManager \
    test_TimeWarpPeriodicStateManager \
    test_TimeWarpAggressiveOutputManager \
    test_TimeWarpFileStreamManager \
    test_TimeWarpEventSet

noinst_HEADERS = catch.hpp mocks.hpp

TESTS = $(WARPED_TEST_PROGS)
check_PROGRAMS = $(WARPED_TEST_PROGS)

CLEANFILES = test_in.txt test_out1.txt test_out2.txt test_out3.txt profile_guided_stats.test partition0.out partition1.out
