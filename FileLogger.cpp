#include "FileLogger.h"
#include <utility> // For std::move

FileLogger::FileLogger(uint16_t record_frequency, uint32_t record_duration)
{
    if(record_frequency == 0)
        record_frequency = 1;

    recFrq = record_frequency;
    recDur = record_duration;
}

bool FileLogger::setDir(const std::filesystem::path& directory_address) 
{
    // Check if the directory exists, if not, create it
    if (!std::filesystem::exists(directory_address)) 
    {
        if (!mkdir(directory_address)) 
        {
            errorMessage = "Failed to create directory: " + directory_address.string();
            return false; // Exit with an error code
        }
    }
    _root = directory_address;
    return true;
}

bool FileLogger::open(const std::string& name_add, std::ios_base::openmode mode) 
{
    // Check if file is opend, then close it and do reopen.
    if (_fileStream.is_open()) 
    {
        _fileStream.close();
    }
    _fileStream.open(_root / name_add, mode);
    _currentFileName = name_add;

    if (!_fileStream.is_open()) 
    {
        errorMessage = "Can not open the file: " + name_add + ".";
        return false;
    }

    return true;
}

size_t FileLogger::write(uint8_t byte) 
{
    if (!_fileStream.is_open()) 
    {
        std::cerr << "Error: File is not open." << std::endl;
        return 0;
    }

    _fileStream.put(static_cast<char>(byte));
    
    if (!_fileStream.good()) 
    {
        std::cerr << "Error: Failed to write byte to file." << std::endl;
        return 0;
    }
    return 1;
}

size_t FileLogger::write(const uint8_t* buf, size_t size) 
{
    if (!_fileStream.is_open()) 
    {
        std::cerr << "Error: File is not open." << std::endl;
        return 0;
    }

    if (buf == nullptr || size == 0) 
    {
        std::cerr << "Error: Null buffer or zero size." << std::endl;
        return 0;
    }

    _fileStream.write(reinterpret_cast<const char*>(buf), size);

    if (!_fileStream.good()) 
    {
        std::cerr << "Error: Failed to write buffer to file." << std::endl;
        return 0;
    }
    return size;
}

void FileLogger::print(const std::string& msg) 
{
    if (_fileStream.is_open()) 
    {
        _fileStream << msg;
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }
}

void FileLogger::println(const std::string& msg) 
{
    print(msg); // Print the message
    if (_fileStream.is_open()) 
    {
        _fileStream << std::endl; // Append newline
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }

    // Increase log cursor 1 line after record custom log.
    ++_lineNum;
}

// Overloaded print methods for different types
void FileLogger::print(int value) 
{
    if (_fileStream.is_open()) 
    {
        _fileStream << value;
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }
}

void FileLogger::println(int value) 
{
    print(value); // Print the value
    if (_fileStream.is_open()) 
    {
        _fileStream << std::endl; // Append newline
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }

    // Increase log cursor 1 line after record custom log.
    ++_lineNum;
}

void FileLogger::print(double value) 
{
    if (_fileStream.is_open()) 
    {
        _fileStream << std::fixed << std::setprecision(2) << value;
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }
}

void FileLogger::println(double value) 
{
    print(value); // Print the value
    if (_fileStream.is_open()) 
    {
        _fileStream << std::endl; // Append newline
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }

    // Increase log cursor 1 line after record custom log.
    ++_lineNum;
}

void FileLogger::print(const char* value) 
{
    if (_fileStream.is_open()) 
    {
        _fileStream << value;
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }
}

void FileLogger::println(const char* value) 
{
    print(value); // Print the string
    if (_fileStream.is_open()) 
    {
        _fileStream << std::endl; // Append newline
    } 
    else 
    {
        std::cerr << "Error: File is not open." << std::endl;
    }

    // Increase log cursor 1 line after record custom log.
    ++_lineNum;
}


int FileLogger::availableForWrite() 
{
    return _fileStream.is_open() ? 1 : 0; // Simple check for file open status
}

int FileLogger::read() 
{
    if (!_fileStream.is_open()) return -1;
    return _fileStream.get();
}

int FileLogger::read(void* buf, uint16_t nbyte) 
{
    if (!_fileStream.is_open()) return 0;
    _fileStream.read(reinterpret_cast<char*>(buf), nbyte);
    return _fileStream.gcount();
}

std::string FileLogger::readAll() {
    if (!_fileStream.is_open()) {
        std::cerr << "Error: File is not open." << std::endl;
        return "";
    }

    std::ostringstream oss;
    oss << _fileStream.rdbuf();
    return oss.str();
}

int FileLogger::peek() 
{
    if (!_fileStream.is_open()) return -1;
    return _fileStream.peek();
}

int FileLogger::available() 
{
    if (!_fileStream.is_open()) return 0;
    return _fileStream.rdbuf()->in_avail();
}

void FileLogger::flush() 
{
    if (_fileStream.is_open()) 
    {
        _fileStream.flush();
    }
}

bool FileLogger::seek(uint32_t pos) 
{
    if (!_fileStream.is_open()) return false;
    _fileStream.seekg(pos);
    return _fileStream.good();
}

uint32_t FileLogger::position() 
{
    if (!_fileStream.is_open()) return 0;
    return static_cast<uint32_t>(_fileStream.tellg());
}

uint32_t FileLogger::size() 
{
    if (!_fileStream.is_open()) return 0;
    std::streampos currentPos = _fileStream.tellg();
    _fileStream.seekg(0, std::ios::end);
    std::streampos fileSize = _fileStream.tellg();
    _fileStream.seekg(currentPos); // Restore original position
    return static_cast<uint32_t>(fileSize);
}

void FileLogger::close() 
{
    if (_fileStream.is_open()) 
    {
        _fileStream.close();
    }
}

FileLogger::operator bool() const {
    return _fileStream.is_open();
}

const std::string& FileLogger::name() const {
    return _currentFileName;
}

bool FileLogger::isDirectory() const {
    return std::filesystem::is_directory(_root / _currentFileName);
}

void FileLogger::rewindDirectory() {
    // Placeholder; implement if necessary
}

void FileLogger::end() {
    _root.clear();
}

bool FileLogger::exists(const std::string& filepath) {
    return std::filesystem::exists(_root / filepath);
}

bool FileLogger::mkdir(const std::string& filepath) {
    return std::filesystem::create_directories(_root / filepath);
}

bool FileLogger::remove(const std::string& filepath) {
    return std::filesystem::remove(_root / filepath);
}

bool FileLogger::rmdir(const std::string& filepath) {
    return std::filesystem::remove_all(_root / filepath) > 0;
}

std::fstream FileLogger::getParentDir(const std::string& /*filepath*/, int* /*indx*/) {
    // Placeholder; implement if needed
    return std::fstream();
}

void FileLogger::resetLineNum(void)
{
    _lineNum = 0;
    seek(0);
}
