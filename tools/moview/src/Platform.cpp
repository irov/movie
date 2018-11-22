#include "Platform.h"

#ifdef PLATFORM_WINDOWS

#include <Windows.h>
#include <Shlobj.h>
#include <shlwapi.h>

#include <locale>
#include <codecvt>

// Useful helpers for Win platform
static std::wstring Utf8ToUnicode( const std::string & _utf )
{
    std::wstring_convert<std::codecvt_utf8<std::wstring::value_type>, std::wstring::value_type> convert;
    return std::move( convert.from_bytes( _utf ) );
}

inline std::string UnicodeToUtf8( const std::wstring & _unicode )
{
    std::wstring_convert<std::codecvt_utf8<std::wstring::value_type>, std::wstring::value_type> convert;
    return std::move( convert.to_bytes( _unicode ) );
}

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>

static int do_mkdir( const char * path, mode_t mode )
{
    struct stat st;
    int         status = 0;

    if( stat( path, &st ) != 0 )
    {
        /* Directory does not exist. EEXIST for race condition */
        if( mkdir( path, mode ) != 0 && errno != EEXIST )
        {
            status = -1;
        }
    }
    else if( !S_ISDIR( st.st_mode ) )
    {
        errno = ENOTDIR;
        status = -1;
    }

    return status;
}

/**
** do_mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int do_mkpath( const char * path, mode_t mode )
{
    char * pp;
    char * sp;
    int    status;
    char * copypath = strdup( path );

    status = 0;
    pp = copypath;
    while( status == 0 && ( sp = strchr( pp, '/' ) ) != 0 )
    {
        if( sp != pp )
        {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir( copypath, mode );
            *sp = '/';
        }
        pp = sp + 1;
    }
    if( status == 0 )
    {
        status = do_mkdir(path, mode);
    }
    free( copypath );
    return status;
}

static std::string get_home()
{
    std::string result;

    int uid = getuid();
    const char* homeEnv = std::getenv("HOME");
    if( uid != 0 && homeEnv )
    {
        //We only acknowlegde HOME if not root.
        result = homeEnv;
    }
    else
    {
        struct passwd * pw = getpwuid(uid);
        if( !pw )
        {
            const char* tempRes = pw->pw_dir;
            if( tempRes )
            {
                result = tempRes;
            }
        }
    }

    return result;
}

#endif // PLATFORM_WINDOWS

namespace Platform
{

    Handle FOpen( const std::string & _path, const FOMode _mode )
    {
#ifdef PLATFORM_WINDOWS
        std::wstring unicode = Utf8ToUnicode( _path );

        DWORD dwDesiredAccess = 0;
        DWORD dwShareMode = 0;
        DWORD dwCreationDisposition = 0;
        if( _mode == FOMode::Read )
        {
            dwDesiredAccess = GENERIC_READ;
            dwShareMode = FILE_SHARE_READ;
            dwCreationDisposition = OPEN_EXISTING;
        }
        else if( _mode == FOMode::Write )
        {
            dwDesiredAccess = GENERIC_WRITE;
            dwShareMode = FILE_SHARE_WRITE;
            dwCreationDisposition = CREATE_ALWAYS;
        }
        else
        {
            dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
            dwCreationDisposition = CREATE_ALWAYS; //#TODO: ???
        }

        HANDLE hFile = ::CreateFileW( unicode.c_str(), dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr );
        if( hFile == INVALID_HANDLE_VALUE )
        {
            // Winapi is weird
            hFile = nullptr;
        }
        return static_cast<Handle>( hFile );
#else
        const char* m = ((_mode == FOMode::Read) ? "rb" : ((_mode == FOMode::Write) ? "wb" : "w+"));
        FILE* file = fopen( _path.c_str(), m );
        return reinterpret_cast<Handle>( file );
#endif
    }

    size_t FRead( void * _buffer, const size_t _numBytes, Handle _handle )
    {
        size_t result = 0;

        if( _handle && _buffer && _numBytes )
        {
#ifdef PLATFORM_WINDOWS
            const DWORD dwToRead = static_cast<DWORD>( _numBytes & 0xFFFFFFFF );
            DWORD bytesRead = 0;
            if( ::ReadFile( static_cast<HANDLE>( _handle ), _buffer, dwToRead, &bytesRead, nullptr ) )
            {
                result = static_cast<size_t>( bytesRead );
            }
#else
            result = fread( _buffer, 1, _numBytes, reinterpret_cast<FILE*>( _handle ) );
#endif
        }

        return result;

    }

    size_t FWrite( const void * _buffer, const size_t _numBytes, Handle _handle )
    {
        size_t result = 0;

        if( _handle && _buffer && _numBytes )
        {
#ifdef PLATFORM_WINDOWS
            const DWORD dwToWrite = static_cast<DWORD>( _numBytes & 0xFFFFFFFF );
            DWORD bytesWritten = 0;
            if( ::WriteFile( static_cast<HANDLE>( _handle ), _buffer, dwToWrite, &bytesWritten, nullptr ) )
            {
                result = static_cast<size_t>( bytesWritten );
            }
#else
            result = fwrite( _buffer, 1, _numBytes, reinterpret_cast<FILE*>( _handle ) );
#endif
        }

        return result;
    }

    size_t FSeek( const size_t _offset, const SeekOrigin _origin, Handle _handle )
    {
        size_t result = 0;

        if( _handle )
        {
#ifdef PLATFORM_WINDOWS
            DWORD dwMoveMethod = 0;
            if( _origin == SeekOrigin::Begin )
            {
                dwMoveMethod = FILE_BEGIN;
            }
            else if( _origin == SeekOrigin::Current )
            {
                dwMoveMethod = FILE_CURRENT;
            }
            else
            {
                dwMoveMethod = FILE_END;
            }

            LARGE_INTEGER liDistanceToMove, liNewFilePointer;
            liDistanceToMove.QuadPart = static_cast<LONGLONG>( _offset );
            if( ::SetFilePointerEx( static_cast<HANDLE>( _handle ), liDistanceToMove, &liNewFilePointer, dwMoveMethod ) )
            {
                result = static_cast<size_t>( liNewFilePointer.QuadPart );
            }
#else
            const int org = ((_origin == SeekOrigin::Begin) ? SEEK_SET : ((_origin == SeekOrigin::Current) ? SEEK_CUR : SEEK_END));
            result = static_cast<size_t>( fseek( reinterpret_cast<FILE*>( _handle ), static_cast<long>( _offset ), org ) );
#endif
        }

        return result;
    }

    size_t FTell( Handle _handle )
    {
#ifdef PLATFORM_WINDOWS
        // https://docs.microsoft.com/en-us/windows/desktop/api/fileapi/nf-fileapi-setfilepointerex
        //  You can also use SetFilePointerEx to query the current file pointer position.
        //  To do this, specify a move method of FILE_CURRENT and a distance of zero.
        return FSeek( 0, SeekOrigin::Current, _handle );
#else
        return static_cast<size_t>( ftell( reinterpret_cast<FILE*>( _handle ) ) );
#endif
    }

    void FClose( Handle _handle )
    {
        if( _handle )
        {
#ifdef PLATFORM_WINDOWS
            ::CloseHandle( static_cast<HANDLE>( _handle ) );
#else
            fclose( reinterpret_cast<FILE*>( _handle ) );
#endif
        }
    }


    bool CheckIfExists( const std::string & _path )
    {
#ifdef PLATFORM_WINDOWS
        std::wstring unicode = Utf8ToUnicode( _path );
        return ( INVALID_FILE_ATTRIBUTES != ::GetFileAttributesW( unicode.c_str() ) );
#else
        struct stat buffer;
        return ( 0 == stat( _path.c_str(), &buffer ) );
#endif
    }

    bool CreateDirs( const std::string & _path )
    {
#ifdef PLATFORM_WINDOWS
        std::wstring unicode = Utf8ToUnicode(_path);
        const int result = ::SHCreateDirectory( nullptr, unicode.c_str() );

        // "The directory exists" code is not an error in our case
        return ( ERROR_SUCCESS        == result ||
                 ERROR_FILE_EXISTS    == result ||
                 ERROR_ALREADY_EXISTS == result );
#else
        const mode_t mode = 0755;
        int ret = do_mkpath( _path.c_str(), mode );
        return ( ret == 0 );
#endif
    }


    std::string GetAppDataFolder()
    {
        std::string result;

#if defined( PLATFORM_WINDOWS )
        TCHAR wszPath[MAX_PATH];
        HRESULT hr = ::SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, nullptr, 0, wszPath );
        if( SUCCEEDED( hr ) )
        {
#ifdef _UNICODE
            result = UnicodeToUtf8( wszPath );
#else
            result = wszPath;
#endif
        }
#elif defined( PLATFORM_MACOS )
        result = get_home() + "/Library/Application Support";
#elif defined( PLATFORM_LINUX )
        // FIrst we try to get the folder from environment
        const char * tmp = std::getenv( "XDG_DATA_HOME" );
        if( tmp )
        {
            result = tmp;
        }
        else
        {   // if failed = then fallback to default one
            result = get_home() + "/.local/share";
        }
#endif

        return result;
    }

    std::string PathConcat( const std::string & _pathA, const std::string & _pathB )
    {
        std::string result;

#ifdef PLATFORM_WINDOWS
        std::wstring unicodeA = Utf8ToUnicode( _pathA );
        std::wstring unicodeB = Utf8ToUnicode( _pathB );

        std::wstring::value_type finalPath[MAX_PATH + 1];

        if( nullptr != ::PathCombineW( finalPath, unicodeA.c_str(), unicodeB.c_str() ) )
        {
            result = UnicodeToUtf8( finalPath );
        }
#else
        // Super-dumb version for *nix systems
        result = _pathA;
        if( _pathA.back() != '/' )
        {
            result += '/';
        }
        result += _pathB;
#endif

        return result;
    }

}
