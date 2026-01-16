#include <gtest/gtest.h>
#include "logmessage.hpp"
#include <string>
#include <sstream>
/*AI genrated */
/**
 * TEST SUITE: LogMessageTest
 * 
 * This test suite comprehensively tests the logmessage class.
 * 
 * Test Coverage:
 * 1. Construction Tests
 * 2. Getter Tests  
 * 3. Setter Tests
 * 4. Edge Case Tests
 * 5. Stream Operator Tests
 * 6. Integration Tests
 */

class LogMessageTest : public ::testing::Test {
protected:
    void SetUp() override {
        testName = "TestLogger";
        testTime = "2024-01-07 10:30:45";
        testContext = "Application::Main";
        testSeverity = "INFO";
        testText = "Test log message";
    }

    void TearDown() override {
        // Clean up if needed
    }

    std::string testName;
    std::string testTime;
    std::string testContext;
    std::string testSeverity;
    std::string testText;
};


// ============================================================================
// CONSTRUCTION TESTS
// ============================================================================

/**
 * TEST: ParameterizedConstructor
 * 
 * PURPOSE: Verify that a logmessage can be created with all fields provided
 * IMPORTANCE: This is the primary way to create log messages
 * EXPECTED: All provided values should be correctly stored
 */
TEST_F(LogMessageTest, ParameterizedConstructor) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getName(), testName);
    EXPECT_EQ(msg.getTime(), testTime);
    EXPECT_EQ(msg.getContext(), testContext);
    EXPECT_EQ(msg.getSeverity(), testSeverity);
    EXPECT_EQ(msg.getText(), testText);
}

/**
 * TEST: ConstructorWithEmptyStrings
 * 
 * PURPOSE: Verify that logmessage handles empty string parameters
 * IMPORTANCE: Some fields might be empty in real usage
 * EXPECTED: Empty strings should be stored without issues
 */
TEST_F(LogMessageTest, ConstructorWithEmptyStrings) {
    logmessage msg("", "", "", "", "");
    
    EXPECT_EQ(msg.getName(), "");
    EXPECT_EQ(msg.getTime(), "");
    EXPECT_EQ(msg.getContext(), "");
    EXPECT_EQ(msg.getSeverity(), "");
    EXPECT_EQ(msg.getText(), "");
}

/**
 * TEST: ConstructorWithMixedEmptyFields
 * 
 * PURPOSE: Verify mix of empty and non-empty fields
 * IMPORTANCE: Common to have some fields empty
 * EXPECTED: Each field stores its value independently
 */
TEST_F(LogMessageTest, ConstructorWithMixedEmptyFields) {
    logmessage msg("Logger", "", "Context", "", "Message");
    
    EXPECT_EQ(msg.getName(), "Logger");
    EXPECT_EQ(msg.getTime(), "");
    EXPECT_EQ(msg.getContext(), "Context");
    EXPECT_EQ(msg.getSeverity(), "");
    EXPECT_EQ(msg.getText(), "Message");
}


// ============================================================================
// GETTER TESTS
// ============================================================================

/**
 * TEST: GetName
 * 
 * PURPOSE: Verify getName() returns the correct value
 * IMPORTANCE: Getters must return exactly what was stored
 */
TEST_F(LogMessageTest, GetName) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    std::string result = msg.getName();
    
    EXPECT_EQ(result, testName);
    EXPECT_STREQ(result.c_str(), testName.c_str());
}

/**
 * TEST: GetTime
 * 
 * PURPOSE: Verify getTime() returns the correct timestamp
 * IMPORTANCE: Timestamps are critical for log analysis
 */
TEST_F(LogMessageTest, GetTime) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getTime(), testTime);
}

/**
 * TEST: GetContext
 * 
 * PURPOSE: Verify getContext() returns the correct context
 * IMPORTANCE: Context identifies where the log originated
 */
TEST_F(LogMessageTest, GetContext) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getContext(), testContext);
}

/**
 * TEST: GetSeverity
 * 
 * PURPOSE: Verify getSeverity() returns the correct severity
 * IMPORTANCE: Severity is used for filtering and prioritizing
 */
TEST_F(LogMessageTest, GetSeverity) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getSeverity(), testSeverity);
}

/**
 * TEST: GetText
 * 
 * PURPOSE: Verify getText() returns the correct message text
 * IMPORTANCE: The message text is the actual log content
 */
TEST_F(LogMessageTest, GetText) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getText(), testText);
}

/**
 * TEST: AllGettersAfterConstruction
 * 
 * PURPOSE: Verify all getters work correctly together
 * IMPORTANCE: Ensures no getter interferes with another
 */
TEST_F(LogMessageTest, AllGettersAfterConstruction) {
    logmessage msg("App", "12:00:00", "Module", "ERROR", "Failed");
    
    EXPECT_EQ(msg.getName(), "App");
    EXPECT_EQ(msg.getTime(), "12:00:00");
    EXPECT_EQ(msg.getContext(), "Module");
    EXPECT_EQ(msg.getSeverity(), "ERROR");
    EXPECT_EQ(msg.getText(), "Failed");
}


// ============================================================================
// SETTER TESTS
// ============================================================================

/**
 * TEST: SetName
 * 
 * PURPOSE: Verify setName() correctly updates the name field
 * IMPORTANCE: Setters allow modification after creation
 * EXPECTED: New value should replace the old value
 */
TEST_F(LogMessageTest, SetName) {
    logmessage msg("OldName", testTime, testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getName(), "OldName");
    
    msg.setName("NewName");
    
    EXPECT_EQ(msg.getName(), "NewName");
    EXPECT_EQ(msg.getTime(), testTime);
    EXPECT_EQ(msg.getContext(), testContext);
}

/**
 * TEST: SetTime
 * 
 * PURPOSE: Verify setTime() correctly updates the timestamp
 * IMPORTANCE: Time may need to be updated or corrected
 */
TEST_F(LogMessageTest, SetTime) {
    logmessage msg(testName, "Old Time", testContext, testSeverity, testText);
    
    EXPECT_EQ(msg.getTime(), "Old Time");
    
    msg.setTime("2024-01-07 11:45:00");
    
    EXPECT_EQ(msg.getTime(), "2024-01-07 11:45:00");
    EXPECT_EQ(msg.getName(), testName);
}

/**
 * TEST: SetContext
 * 
 * PURPOSE: Verify setContext() correctly updates the context
 * IMPORTANCE: Context may need to be refined after creation
 */
TEST_F(LogMessageTest, SetContext) {
    logmessage msg(testName, testTime, "OldContext", testSeverity, testText);
    
    msg.setContext("NewContext");
    
    EXPECT_EQ(msg.getContext(), "NewContext");
}

/**
 * TEST: SetSeverity
 * 
 * PURPOSE: Verify setSeverity() correctly updates the severity
 * IMPORTANCE: Severity levels may be adjusted based on context
 */
TEST_F(LogMessageTest, SetSeverity) {
    logmessage msg(testName, testTime, testContext, "INFO", testText);
    
    EXPECT_EQ(msg.getSeverity(), "INFO");
    
    msg.setSeverity("ERROR");
    EXPECT_EQ(msg.getSeverity(), "ERROR");
    
    msg.setSeverity("DEBUG");
    EXPECT_EQ(msg.getSeverity(), "DEBUG");
    
    msg.setSeverity("WARNING");
    EXPECT_EQ(msg.getSeverity(), "WARNING");
}

/**
 * TEST: SetText
 * 
 * PURPOSE: Verify setText() correctly updates the message text
 * IMPORTANCE: Message text is the most likely field to be modified
 */
TEST_F(LogMessageTest, SetText) {
    logmessage msg(testName, testTime, testContext, testSeverity, "Old message");
    
    EXPECT_EQ(msg.getText(), "Old message");
    
    msg.setText("New message");
    
    EXPECT_EQ(msg.getText(), "New message");
}

/**
 * TEST: SetterWithEmptyString
 * 
 * PURPOSE: Verify setters can set fields to empty strings
 * IMPORTANCE: Sometimes fields need to be cleared
 * EXPECTED: Setting empty string should work without issues
 */
TEST_F(LogMessageTest, SetterWithEmptyString) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    msg.setName("");
    EXPECT_EQ(msg.getName(), "");
    
    msg.setTime("");
    EXPECT_EQ(msg.getTime(), "");
    
    msg.setContext("");
    EXPECT_EQ(msg.getContext(), "");
    
    msg.setSeverity("");
    EXPECT_EQ(msg.getSeverity(), "");
    
    msg.setText("");
    EXPECT_EQ(msg.getText(), "");
}

/**
 * TEST: MultipleSettersOnSameField
 * 
 * PURPOSE: Verify that a field can be set multiple times
 * IMPORTANCE: Fields may be updated multiple times in dynamic scenarios
 * EXPECTED: Each call should overwrite the previous value
 */
TEST_F(LogMessageTest, MultipleSettersOnSameField) {
    logmessage msg(testName, testTime, testContext, testSeverity, testText);
    
    msg.setText("First");
    EXPECT_EQ(msg.getText(), "First");
    
    msg.setText("Second");
    EXPECT_EQ(msg.getText(), "Second");
    
    msg.setText("Third");
    EXPECT_EQ(msg.getText(), "Third");
}

/**
 * TEST: AllSetters
 * 
 * PURPOSE: Verify all setters work correctly together
 * IMPORTANCE: Ensures no setter interferes with others
 */
TEST_F(LogMessageTest, AllSetters) {
    logmessage msg("", "", "", "", "");
    
    msg.setName("NewLogger");
    msg.setTime("13:30:00");
    msg.setContext("NewContext");
    msg.setSeverity("CRITICAL");
    msg.setText("New message text");
    
    EXPECT_EQ(msg.getName(), "NewLogger");
    EXPECT_EQ(msg.getTime(), "13:30:00");
    EXPECT_EQ(msg.getContext(), "NewContext");
    EXPECT_EQ(msg.getSeverity(), "CRITICAL");
    EXPECT_EQ(msg.getText(), "New message text");
}


// ============================================================================
// EDGE CASE TESTS
// ============================================================================

/**
 * TEST: VeryLongStrings
 * 
 * PURPOSE: Verify logmessage handles very long strings
 * IMPORTANCE: Log messages can be very long (stack traces, data dumps)
 * EXPECTED: Long strings should be stored and retrieved in full
 */
TEST_F(LogMessageTest, VeryLongStrings) {
    std::string longText(10000, 'A');
    
    logmessage msg("Logger", "2024-01-07", "Context", "INFO", longText);
    
    EXPECT_EQ(msg.getText(), longText);
    EXPECT_EQ(msg.getText().length(), 10000);
}

/**
 * TEST: SpecialCharactersInStrings
 * 
 * PURPOSE: Verify logmessage handles special characters
 * IMPORTANCE: Logs often contain newlines, tabs, quotes, backslashes
 * EXPECTED: All special characters should be preserved
 */
TEST_F(LogMessageTest, SpecialCharactersInStrings) {
    std::string specialText = "Line1\nLine2\tTabbed\r\nWindows";
    std::string quotesText = "He said \"Hello\" and she said 'Hi'";
    std::string pathText = "C:\\Users\\Test\\file.txt";
    
    logmessage msg1("Logger", "Time", "Context", "INFO", specialText);
    logmessage msg2("Logger", "Time", "Context", "INFO", quotesText);
    logmessage msg3("Logger", "Time", "Context", "INFO", pathText);
    
    EXPECT_EQ(msg1.getText(), specialText);
    EXPECT_EQ(msg2.getText(), quotesText);
    EXPECT_EQ(msg3.getText(), pathText);
}

/**
 * TEST: WhitespaceHandling
 * 
 * PURPOSE: Verify logmessage preserves whitespace
 * IMPORTANCE: Whitespace is significant for formatting
 * EXPECTED: Leading/trailing whitespace should never be trimmed
 */
TEST_F(LogMessageTest, WhitespaceHandling) {
    std::string leadingSpace = "   Leading spaces";
    std::string trailingSpace = "Trailing spaces   ";
    std::string bothSpace = "   Both   ";
    
    logmessage msg1("Logger", "Time", "Context", "INFO", leadingSpace);
    logmessage msg2("Logger", "Time", "Context", "INFO", trailingSpace);
    logmessage msg3("Logger", "Time", "Context", "INFO", bothSpace);
    
    EXPECT_EQ(msg1.getText(), leadingSpace);
    EXPECT_EQ(msg2.getText(), trailingSpace);
    EXPECT_EQ(msg3.getText(), bothSpace);
}


// ============================================================================
// STREAM OPERATOR TESTS
// ============================================================================

/**
 * TEST: StreamOperatorBasicOutput
 * 
 * PURPOSE: Verify the << operator formats output correctly
 * IMPORTANCE: Stream operator is used for displaying messages
 * EXPECTED: Format should be [time] [severity] name (context): text
 */
TEST_F(LogMessageTest, StreamOperatorBasicOutput) {
    logmessage msg("MyApp", "2024-01-07 10:30:45", "Main", "INFO", "Application started");
    
    std::ostringstream oss;
    oss << msg;
    
    std::string expected = "[2024-01-07 10:30:45] [INFO] MyApp (Main): Application started";
    
    EXPECT_EQ(oss.str(), expected);
}

/**
 * TEST: StreamOperatorWithEmptyFields
 * 
 * PURPOSE: Verify stream operator handles empty fields
 * IMPORTANCE: Empty fields should not break formatting
 */
TEST_F(LogMessageTest, StreamOperatorWithEmptyFields) {
    logmessage msg("", "", "", "", "");
    
    std::ostringstream oss;
    oss << msg;
    
    std::string expected = "[] []  (): ";
    
    EXPECT_EQ(oss.str(), expected);
}

/**
 * TEST: StreamOperatorWithDifferentSeverities
 * 
 * PURPOSE: Verify stream operator works with different severities
 */
TEST_F(LogMessageTest, StreamOperatorWithDifferentSeverities) {
    logmessage errorMsg("App", "10:00:00", "DB", "ERROR", "Connection failed");
    logmessage debugMsg("App", "10:00:01", "UI", "DEBUG", "Button clicked");
    logmessage warningMsg("App", "10:00:02", "Net", "WARNING", "Slow response");
    
    std::ostringstream oss1, oss2, oss3;
    
    oss1 << errorMsg;
    oss2 << debugMsg;
    oss3 << warningMsg;
    
    EXPECT_EQ(oss1.str(), "[10:00:00] [ERROR] App (DB): Connection failed");
    EXPECT_EQ(oss2.str(), "[10:00:01] [DEBUG] App (UI): Button clicked");
    EXPECT_EQ(oss3.str(), "[10:00:02] [WARNING] App (Net): Slow response");
}

/**
 * TEST: StreamOperatorMultipleMessages
 * 
 * PURPOSE: Verify multiple messages can be streamed sequentially
 * IMPORTANCE: Multiple log messages are often written to same stream
 */
TEST_F(LogMessageTest, StreamOperatorMultipleMessages) {
    logmessage msg1("App", "10:00:00", "Init", "INFO", "Starting");
    logmessage msg2("App", "10:00:01", "Init", "INFO", "Ready");
    
    std::ostringstream oss;
    oss << msg1 << "\n" << msg2;
    
    std::string expected = "[10:00:00] [INFO] App (Init): Starting\n"
                          "[10:00:01] [INFO] App (Init): Ready";
    
    EXPECT_EQ(oss.str(), expected);
}

/**
 * TEST: StreamOperatorAfterSetters
 * 
 * PURPOSE: Verify stream operator reflects changes from setters
 * IMPORTANCE: Output should show updated values after modification
 */
TEST_F(LogMessageTest, StreamOperatorAfterSetters) {
    logmessage msg("OldApp", "10:00:00", "OldContext", "INFO", "Old message");
    
    msg.setName("NewApp");
    msg.setSeverity("ERROR");
    msg.setText("New message");
    
    std::ostringstream oss;
    oss << msg;
    
    std::string expected = "[10:00:00] [ERROR] NewApp (OldContext): New message";
    
    EXPECT_EQ(oss.str(), expected);
}


// ============================================================================
// INTEGRATION TESTS
// ============================================================================

/**
 * TEST: CompleteWorkflow
 * 
 * PURPOSE: Test a complete workflow of creating and using a logmessage
 * IMPORTANCE: Simulates how the class will be used in production
 */
TEST_F(LogMessageTest, CompleteWorkflow) {
    logmessage msg("MyApp", "2024-01-07 10:30:00", "Database::Connect", "INFO", "Connecting");
    
    EXPECT_EQ(msg.getSeverity(), "INFO");
    EXPECT_EQ(msg.getText(), "Connecting");
    
    msg.setSeverity("ERROR");
    msg.setText("Failed to connect to database: Connection timeout");
    
    EXPECT_EQ(msg.getSeverity(), "ERROR");
    EXPECT_EQ(msg.getText(), "Failed to connect to database: Connection timeout");
    
    std::ostringstream oss;
    oss << msg;
    
    std::string expected = "[2024-01-07 10:30:00] [ERROR] MyApp (Database::Connect): Failed to connect to database: Connection timeout";
    EXPECT_EQ(oss.str(), expected);
}

/**
 * TEST: MultipleIndependentMessages
 * 
 * PURPOSE: Verify multiple logmessage objects can coexist independently
 * IMPORTANCE: Many messages are created simultaneously in production
 */
TEST_F(LogMessageTest, MultipleIndependentMessages) {
    logmessage msg1("App1", "10:00:00", "Context1", "INFO", "Message 1");
    logmessage msg2("App2", "10:00:01", "Context2", "ERROR", "Message 2");
    logmessage msg3("App3", "10:00:02", "Context3", "DEBUG", "Message 3");
    
    EXPECT_EQ(msg1.getName(), "App1");
    EXPECT_EQ(msg2.getName(), "App2");
    EXPECT_EQ(msg3.getName(), "App3");
    
    EXPECT_EQ(msg1.getSeverity(), "INFO");
    EXPECT_EQ(msg2.getSeverity(), "ERROR");
    EXPECT_EQ(msg3.getSeverity(), "DEBUG");
    
    EXPECT_EQ(msg1.getText(), "Message 1");
    EXPECT_EQ(msg2.getText(), "Message 2");
    EXPECT_EQ(msg3.getText(), "Message 3");
}


// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}