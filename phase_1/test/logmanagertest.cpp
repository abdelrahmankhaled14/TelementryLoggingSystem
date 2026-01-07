#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "logmanager.hpp"
#include <string>
#include <sstream>
#include "logmessage.hpp"
#include "Ilogsink.hpp"

class MockSink : public Ilogsink {
public:
    MOCK_METHOD(void, write, (const logmessage& msg), (override));
};

class LogManagerTest : public ::testing::Test
{
protected:
    MockSink sink;
    LogManager manager;

    LogManagerTest()
        : sink(),
          manager(&sink) // ✅ pointer + initializer list
    {
    }
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(LogManagerTest, testisempty)
{
    EXPECT_TRUE(manager.isEmpty());
    EXPECT_EQ(manager.getMessageCount(), 0);
}

TEST_F(LogManagerTest, Testlogging)
{
    logmessage msg1("Logger", "2024-01-07", "Context", "INFO", "longText");
    logmessage msg2("Logger", "2024-01-07", "Context", "INFO", "longText");
    logmessage msg3("Logger", "2024-01-07", "Context", "INFO", "longText");
    logmessage msg4("Logger", "2024-01-07", "Context", "INFO", "longText");
    manager.log(msg1);
    manager.log(msg2);
    manager.log(msg3);
    manager.log(msg4);

    EXPECT_EQ(manager.getMessageCount(), 4);
    EXPECT_FALSE(manager.isEmpty());
}
TEST_F(LogManagerTest, FlushCallsWriteTwice)
{
    logmessage msg1("Logger", "2024", "Ctx", "INFO", "Text1");
    logmessage msg2("Logger", "2024", "Ctx", "INFO", "Text2");

    manager.log(msg1);
    manager.log(msg2);

    EXPECT_CALL(sink, write(::testing::_))
        .Times(2);

    manager.flush();
}
TEST_F(LogManagerTest, Testclear)
{
    logmessage msg1("Logger", "2024-01-07", "Context", "INFO", "longText");
    logmessage msg2("Logger", "2024-01-07", "Context", "INFO", "longText");
    manager.log(msg1);
    manager.log(msg2);
    manager.clear();
    EXPECT_TRUE(manager.isEmpty());
}
