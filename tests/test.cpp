extern "C" {
    #include "arg_parsing.h"
    #include "file_manipulations.h"
    #include "decompress.h"
    #include "compress.h"
}

#include <gtest/gtest.h>
#include <random>
TEST(TestArgs, TestArgs1) {
    struct arg_values arguments = {0};
    char* argv[] = {"zip", "-v", "file.txt", "output.zip"};
    arg_values_parse(4, argv, &arguments);
    ASSERT_STREQ(arguments.input_filename, "file.txt");
    ASSERT_STREQ(arguments.output_filename, "output.zip");
    ASSERT_EQ(arguments.blocksize, 16384);
    ASSERT_EQ(arguments.verbose, 1);
}

TEST(TestArgs, TestArgs2) {
    struct arg_values arguments = {0};
    char* argv[] = {"zip", "-b16384", "file.txt"};
    arg_values_parse(3, argv, &arguments);
    ASSERT_STREQ(arguments.input_filename, "file.txt");
    ASSERT_STREQ(arguments.output_filename, "archive.zip");
    ASSERT_EQ(arguments.blocksize, 16384);
    ASSERT_EQ(arguments.verbose, 0);
}

TEST(TestArgs, TestError1) {
    struct arg_values arguments = {0};
    char* argv[] = {"zip", "-b16383", "file.txt"};
    ASSERT_DEATH(arg_values_parse(3, argv, &arguments), "Block size must be at least 16K");
}
TEST(TestArgs, TestError2) {
    struct arg_values arguments;
    char* argv[] = {"zip", "-b", "file"};
    ASSERT_DEATH(arg_values_parse(3, argv, &arguments), "Must be block size");
}
TEST(TestArgs, TestError3) {
    struct arg_values arguments;
    char* argv[] = {"zip", "-bK20", "file"};
    ASSERT_DEATH(arg_values_parse(3, argv, &arguments), "Error in specifying block size");
}
TEST(TestArgs, TestError4) {
    struct arg_values arguments;
    char* argv[] = {"zip", "-c70", "file"};
    ASSERT_DEATH(arg_values_parse(3, argv, &arguments), "invalid option");
}
TEST(TestArgs, TestError5) {
    struct arg_values arguments;
    char* argv[] = {"zip"};
    ASSERT_DEATH(arg_values_parse(1, argv, &arguments), "Must be at least one argument");
}
TEST(TestArgs, TestError6) {
    struct arg_values arguments;
    char* argv[] = {"zip", "file", "file2", "file3"};
    ASSERT_DEATH(arg_values_parse(4, argv, &arguments), "Too much args");
}

TEST(TestFileManipulations, TestGetSize){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> size_distribution(0,5*1024*1024);
    off_t real_size = size_distribution(gen);
    char* test_filename = "test_file.txt";
    FILE* file = fopen(test_filename, "w");
    ftruncate(fileno(file), real_size);
    ASSERT_EQ(real_size, get_file_size(fileno(file)));
    fclose(file);
    unlink(test_filename);
}
TEST(TestFileManipulations, InitAndDeinitEmptyFile){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> size_distribution(0,5*1024*1024);
    off_t real_size = size_distribution(gen);
    std::uniform_int_distribution<int> blocksize_distribution(sysconf(_SC_PAGE_SIZE),5*1024*1024);
    off_t blocksize = blocksize_distribution(gen);
    char* test_filename = "test_file.txt";
    FILE* file = fopen(test_filename, "w+");
    ftruncate(fileno(file), real_size);
    struct file_map test_file_map;
    ASSERT_EQ(0, init_file_map(&test_file_map, fileno(file), blocksize, nullptr, PROT_READ, MAP_SHARED, 0));
    ASSERT_EQ(std::min(blocksize - blocksize % sysconf(_SC_PAGE_SIZE), real_size), test_file_map.block_end);
    ASSERT_EQ(0, deinit_file_map(&test_file_map));
    fclose(file);
    unlink(test_filename);
}
TEST(TestFileManipulations, InitAndDeinitFieForReading){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> size_distribution(0,5*1024*1024);
    off_t real_size = size_distribution(gen);
    std::uniform_int_distribution<int> blocksize_distribution(sysconf(_SC_PAGE_SIZE),5*1024*1024);
    off_t blocksize = blocksize_distribution(gen);
    char* test_filename = "test_file.txt";
    FILE* file = fopen(test_filename, "w+");
    struct file_map test_file_map;
    ASSERT_EQ(0, init_file_map(&test_file_map, fileno(file), blocksize, nullptr, PROT_WRITE, MAP_SHARED, 0));
    ASSERT_EQ(blocksize - blocksize % sysconf(_SC_PAGE_SIZE), test_file_map.block_end);
    ASSERT_EQ(0, deinit_file_map(&test_file_map));
    fclose(file);
    unlink(test_filename);
}
TEST(TestFileManipulations, InitPermissionsDenied){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> size_distribution(0,5*1024*1024);
    off_t real_size = size_distribution(gen);
    std::uniform_int_distribution<int> blocksize_distribution(sysconf(_SC_PAGE_SIZE),5*1024*1024);
    off_t blocksize = blocksize_distribution(gen);
    char* test_filename = "test_file.txt";
    FILE* file = fopen(test_filename, "w");
    struct file_map test_file_map;
    ASSERT_EQ(EACCES, init_file_map(&test_file_map, fileno(file), blocksize, nullptr, PROT_WRITE, MAP_SHARED, 0));
    fclose(file);
    unlink(test_filename);
}
TEST(TestFileManipulations, UpdateFile){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> size_distribution(0,5*1024*1024);
    off_t real_size = size_distribution(gen);
    std::uniform_int_distribution<int> blocksize_distribution(sysconf(_SC_PAGE_SIZE),5*1024*1024);
    off_t blocksize = blocksize_distribution(gen);
    char* test_filename = "test_file.txt";
    FILE* file = fopen(test_filename, "w+");
    struct file_map test_file_map;
    ASSERT_EQ(0, init_file_map(&test_file_map, fileno(file), blocksize, nullptr, PROT_WRITE, MAP_SHARED, blocksize));
    ASSERT_EQ(0, update_check_file_map(&test_file_map));
    ASSERT_EQ(blocksize - blocksize % sysconf(_SC_PAGE_SIZE), test_file_map.block_start);
    ASSERT_EQ(2 * (blocksize - blocksize % sysconf(_SC_PAGE_SIZE)), test_file_map.block_end);
    ASSERT_EQ(0, deinit_file_map(&test_file_map));
    fclose(file);
    unlink(test_filename);
}
TEST(TestCompressing, CompressingAndDecompressing){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> size_distribution(0,5*1024*1024);
    off_t real_size = size_distribution(gen);
    std::uniform_int_distribution<int> blocksize_distribution(sysconf(_SC_PAGE_SIZE),5*1024*1024);
    off_t blocksize = blocksize_distribution(gen);
    std::uniform_int_distribution<int> char_distribution('a', 'z');
    char* name_input = "test_input.txt";
    FILE* input = fopen(name_input, "w+");
    char* name_archive = "test_archive.zip";
    FILE* archive = fopen(name_archive, "w+");

    for(int i = 0; i < real_size; i+= blocksize){
        char to_write[std::min(blocksize, real_size - i)];
        for(int j = 0; j < std::min(blocksize, real_size - i); j++){
            to_write[j] = char_distribution(gen);
        }
        fwrite(to_write, 1, std::min(blocksize, real_size - i), input);
    }
    fclose(input);
    input = fopen(name_input, "r");
    ASSERT_EQ(0, fd_compress(fileno(archive), fileno(input), blocksize));
    fclose(archive);
    archive = fopen(name_archive, "r");
    char* name_output = "test_output.txt";
    FILE* output = fopen(name_output, "w+");
    ASSERT_EQ(0, fd_decompress(fileno(output), fileno(archive), blocksize));
    fclose(output);
    output = fopen(name_output, "r");
    ASSERT_EQ(get_file_size(fileno(input)), get_file_size(fileno(output)));
    char check_equal = 1;
    for(int i = 0; i < real_size; i+= blocksize){
        char input_buffer[std::min(blocksize, real_size - i)];
        char output_buffer[std::min(blocksize, real_size - i)];
        fread(input_buffer, 1, std::min(blocksize, real_size - i), input);
        fread(output_buffer, 1, std::min(blocksize, real_size - i), output);
        for(int j = 0; j < std::min(blocksize, real_size - i); j++){
            if(input_buffer[j] != output_buffer[j]){
                check_equal = 0;
            }
        }
    }
    ASSERT_EQ(1, check_equal);
    fclose(input);
    unlink(name_input);
    fclose(archive);
    unlink(name_archive);
    fclose(output);
    unlink(name_output);
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}