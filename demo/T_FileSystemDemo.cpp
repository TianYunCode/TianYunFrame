
#include "DemoHead.h"

#if T_FileSystemDemo

#include <iostream>
#include "FileSystem.h"

int main()
{
    FileSystem pathDir("test");
    FileSystem pathFile("test/test.txt");
    std::cout << "test dir exits: " << pathDir.exits() << std::endl;
    std::cout << "test file exits: " << pathFile.exits() << std::endl;

    std::cout << "create test dir: " << FileSystem::createDir("test") << std::endl;
    std::cout << "create test file: " << FileSystem::createNullFile("test/test.txt") << std::endl;

    std::cout << "test dir exits: " << pathDir.exits() << std::endl;
    std::cout << "test file exits: " << pathFile.exits() << std::endl;

    std::cout << "test dir is dir: " << pathDir.isDir() << std::endl;
    std::cout << "test dir is file: " << pathDir.isFile() << std::endl;

    std::cout << "test file is dir: " << pathFile.isDir() << std::endl;
    std::cout << "test file is file: " << pathFile.isFile() << std::endl;

    FileSystem::copy("test", "test2");

    FileSystem::createNullFile("test/test1.txt");

    FileSystem::rename("test/test.txt", "test/test2.txt");
    FileSystem::rename("test", "test1");

    FileSystem::copy("test1", "test");

    std::cout << "remove test1/test2.txt: " << FileSystem::remove("test1/test2.txt") << std::endl;
    std::cout << "remove test: " << FileSystem::remove("test") << std::endl;
    std::cout << "remove all test1: " << FileSystem::removeAll("test1") << std::endl;
    std::cout << "remove test2: " << FileSystem::remove("test2") << std::endl;
    std::cout << "remove all test2: " << FileSystem::removeAll("test2") << std::endl;

    std::cout << "get path type test: " << FileSystem::toString(FileSystem::getPathType("test")) << std::endl;
    std::cout << "get path type test/test.txt: " << FileSystem::toString(FileSystem::getPathType("test/test2.txt")) << std::endl;

    std::cout << "get file size test1/test2: " << FileSystem::getFileSize("test1/test2.txt") << std::endl;
    std::cout << "get file size test/test2: " << FileSystem::getFileSize("test/test2.txt") << std::endl;

    {
        auto filenames = FileSystem::getFiles("test1");
        for (auto &filename : filenames)
        {
            std::cout << "test1 file: " << filename << std::endl;
        }
    }

    {
        auto filenames = FileSystem::getFiles("test");
        for (auto &filename : filenames)
        {
            std::cout << "test file: " << filename << std::endl;
        }
    }

    std::cout << "remove all test: " << FileSystem::removeAll("test") << std::endl;

    return 0;
}

#endif
