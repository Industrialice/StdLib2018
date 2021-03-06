#include "_PreHeader.hpp"
#include "UnitTestsLogger.hpp"

using namespace StdLib;

void LoggerTests()
{
	auto logger = StdLib::Logger<>();

    bool isShouldBeCalled = true;
    LogLevels::LogLevel currentLevel = LogLevels::Attention;
    std::string_view currentReference;
    auto testDest = [&isShouldBeCalled, &currentLevel, &currentReference](LogLevels::LogLevel level, StringViewNullTerminated message)
    {
        UTest(Equal, isShouldBeCalled, true);
        UTest(Equal, currentLevel, level);
        UTest(Equal, currentReference, message);
    };

    auto handle = logger.OnMessage(testDest);
    
    currentReference = "test";
    logger.Attention("test");

    currentLevel = LogLevels::Info;
    currentReference = "123_12";
    logger.Info("%s_%i", "123", 12);

    logger.IsEnabled(false);
    isShouldBeCalled = false;
    logger.Info("");

    auto logger2 = Logger<std::string, true>();

    isShouldBeCalled = true;
    auto testDest2 = [&isShouldBeCalled, &currentLevel, &currentReference](LogLevels::LogLevel level, StringViewNullTerminated message, const std::string &meta)
    {
        UTest(Equal, isShouldBeCalled, true);
        UTest(Equal, currentLevel, level);
        UTest(Equal, currentReference, message);
        UTest(Equal, meta, std::string{"meta test"});
    };
    auto handle2 = logger2.OnMessage(testDest2);

    currentReference = "testing meta";
    logger2.Info(std::string{"meta test"}, "testing meta");

	UnitTestsLogger::Message("finished logger tests\n");
}