/* This file is part of the "ST" project.
 * You may use, distribute or modify this code under the terms
 * of the GNU General Public License version 2.
 * See LICENCE.txt in the root directory of the project.
 *
 * Author: Maxim Atanasov
 * E-mail: maxim.atanasov@protonmail.com
 */

#include <gtest/gtest.h>
#include "../../../ST_message_bus/include/message_bus.hpp"
#include <console.hpp>

/// Tests fixture for the console
class console_test : public ::testing::Test {

protected:
    console* test_cnsl{};

    uint8_t get_log_level(){
        return test_cnsl->log_level;
    }

    void write(ST::log_type type, const std::string &text){
        test_cnsl->write(text, type);
    }

    message_bus* msg_bus{};

    void SetUp() override{
        msg_bus = new message_bus();
        test_cnsl = new console(msg_bus);
    }

    void TearDown() override{
        delete test_cnsl;
        delete msg_bus;
    }
};

TEST_F(console_test, set_log_level) {
    test_cnsl->set_log_level(ST::log_type::ERROR);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::ERROR), get_log_level());

    test_cnsl->set_log_level(ST::log_type::INFO);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::INFO), get_log_level());

    test_cnsl->set_log_level(ST::log_type::SUCCESS);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::SUCCESS), get_log_level());

    test_cnsl->set_log_level(ST::log_type::SUCCESS | ST::log_type::ERROR);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::SUCCESS | ST::log_type::ERROR), get_log_level());

    test_cnsl->set_log_level(ST::log_type::INFO | ST::log_type::ERROR);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::INFO | ST::log_type::ERROR), get_log_level());

    test_cnsl->set_log_level(ST::log_type::INFO | ST::log_type::SUCCESS);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::INFO | ST::log_type::SUCCESS), get_log_level());

    test_cnsl->set_log_level(ST::log_type::INFO | ST::log_type::SUCCESS | ST::log_type::ERROR);
    EXPECT_EQ(static_cast<uint8_t>(ST::log_type::INFO | ST::log_type::SUCCESS | ST::log_type::ERROR), get_log_level());
}

TEST_F(console_test, console_write_error) {
    ::testing::internal::CaptureStdout();
    test_cnsl->set_log_level(ST::log_type::ERROR);
    write(ST::log_type::ERROR, "TEST_STRING");
    ASSERT_EQ("TEST_STRING\n", testing::internal::GetCapturedStdout());
}

TEST_F(console_test, console_write_info) {
    ::testing::internal::CaptureStdout();
    test_cnsl->set_log_level(ST::log_type::INFO);
    write(ST::log_type::INFO, "TEST_STRING");
    ASSERT_EQ("TEST_STRING\n", testing::internal::GetCapturedStdout());
}

TEST_F(console_test, console_write_success) {
    ::testing::internal::CaptureStdout();
    test_cnsl->set_log_level(ST::log_type::SUCCESS);
    write(ST::log_type::SUCCESS, "TEST_STRING");
    ASSERT_EQ("TEST_STRING\n", testing::internal::GetCapturedStdout());
}


TEST_F(console_test, console_write_all) {
    ::testing::internal::CaptureStdout();
    test_cnsl->set_log_level(ST::log_type::SUCCESS | ST::log_type::INFO | ST::log_type::ERROR);

    write(ST::log_type::INFO, "TEST_STRING");
    write(ST::log_type::ERROR, "TEST_STRING2");
    write(ST::log_type::SUCCESS, "TEST_STRING3");

    ASSERT_EQ("TEST_STRING\nTEST_STRING2\nTEST_STRING3\n", testing::internal::GetCapturedStdout());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}