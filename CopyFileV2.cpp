#include <windows.h>
#include <string>

void main()
{
    HANDLE secretFile;
    HANDLE protectedFile;
    DWORD  dwBytesWritten, dwPos, dwBytesRead = 0;
    BOOL fReadFile;

    // Open the existing file.

    secretFile = CreateFileA(
        "C:/Users/sotni/source/repos/CopyFileV2/secret.txt",
        GENERIC_READ,             // open for reading
        0,                        // do not share
        NULL,                     // no security
        OPEN_EXISTING,            // existing file only
        FILE_ATTRIBUTE_NORMAL,    // normal file
        NULL);                    // no attr. template

    if (secretFile == INVALID_HANDLE_VALUE)
    {
        printf("Could not open secret.txt.");
        return;
    }

    // Open the existing file, or if the file does not exist,
    // create a new file.

    protectedFile = CreateFileA(
        "C:/Users/sotni/source/repos/CopyFileV2/protected.txt", // open Two.txt
        GENERIC_WRITE,            // open for writing
        0,                        // allow multiple readers
        NULL,                     // no security
        CREATE_NEW,               // open or create
        FILE_ATTRIBUTE_NORMAL,    // normal file
        NULL);                    // no attr. template

    if (protectedFile == INVALID_HANDLE_VALUE)
    {
        printf("Could not create protected.txt.");
        return;
    }
    /*if (!fReadFile)
    {
        printf("Could not read the file");
    }*/

    const size_t bufSize = 4096;
    std::string buf(bufSize, '\0');
    const std::string searchString = "password: "; // 10
    while (ReadFile(secretFile, &buf[0], buf.size(), &dwBytesRead, NULL) && dwBytesRead > 0)
    {
        buf.resize(dwBytesRead);
        dwPos = SetFilePointer(protectedFile, 0, NULL, FILE_END);
        LockFile(protectedFile, dwPos, 0, dwBytesRead, 0);
        size_t startOfPassword = 0;
        size_t curPos = 0;
        // _password: ***** Some text. And other password: 77777 other Text
        while (startOfPassword != std::string::npos)
        {
            startOfPassword = buf.find(searchString, curPos); // 1
            if (startOfPassword != std::string::npos)
            {
                startOfPassword += searchString.size(); // startOfPassword == 11
                curPos += startOfPassword; // 11
                size_t endOfPassword = buf.find(" ", startOfPassword);//16
                if (endOfPassword != std::string::npos)
                {
                    const size_t passwordLength = endOfPassword - startOfPassword; // 5
                    buf.replace(startOfPassword, passwordLength, passwordLength, '*');
                }
            }
        }
        WriteFile(protectedFile, buf.data(), dwBytesRead, &dwBytesWritten, NULL);
        UnlockFile(protectedFile, dwPos, 0, dwBytesRead, 0);
    }

    /*size_t startOfPassword = 0;
    size_t curPos = 0;*/
    // _password: ***** Some text. And other password: 77777 other Text
    //while (startOfPassword != std::string::npos)
    //{
    //    startOfPassword = buf.find(searchString, curPos); // 1
    //    if (startOfPassword != std::string::npos)
    //    {
    //        startOfPassword += searchString.size(); // startOfPassword == 11
    //        curPos += startOfPassword; // 11
    //        size_t endOfPassword = buf.find(" ", startOfPassword);//16
    //        if (endOfPassword != std::string::npos)
    //        {
    //            const size_t passwordLength = endOfPassword - startOfPassword; // 5
    //            buf.replace(startOfPassword, passwordLength, passwordLength, '*');
    //        }
    //    }
    //}
    //WriteFile(protectedFile, buf.data(), dwBytesRead, &dwBytesWritten, NULL);

    // Close both files.

    CloseHandle(secretFile);
    CloseHandle(protectedFile);
}