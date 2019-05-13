#include "movie/movie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////
static const ae_char_t * test_example_file_paths[] = {
    "examples/resources/Bridge/Bridge.aem",
    "examples/resources/Knight/Knight.aem",
    "examples/resources/Peacock/Peacock.aem",
    "examples/resources/Unicorn/Unicorn.aem" };
//////////////////////////////////////////////////////////////////////////
typedef struct memory_header_t
{
    ae_uint32_t size;
    ae_char_t file[260];
    ae_uint32_t line;
} memory_header_t;
//////////////////////////////////////////////////////////////////////////
#define TEST_MEMORY_MAX_RECORDS 2048
//////////////////////////////////////////////////////////////////////////
typedef struct memory_info_t
{
    ae_size_t memory_allocate;

    memory_header_t memory_records[TEST_MEMORY_MAX_RECORDS];
    ae_uint32_t memory_records_count;
} memory_info_t;
//////////////////////////////////////////////////////////////////////////
static ae_voidptr_t memory_info_alloc( memory_info_t * _info, ae_size_t _size, const ae_char_t * _file, ae_uint32_t _line )
{
    ae_voidptr_t ptr = malloc( _size + sizeof( memory_header_t ) );

    memory_header_t * ptr_header = (memory_header_t *)ptr;
    ptr_header->size = _size;
    strcpy( ptr_header->file, _file );
    ptr_header->line = _line;

    _info->memory_records[_info->memory_records_count++] = *ptr_header;

    if( _info->memory_records_count == TEST_MEMORY_MAX_RECORDS )
    {
        return AE_NULLPTR;
    }

	ae_voidptr_t ptr_data = ptr_header + 1;

    return ptr_data;
}
//////////////////////////////////////////////////////////////////////////
static ae_void_t memory_info_free( memory_info_t * _info, ae_constvoidptr_t _ptr )
{
    memory_header_t * ptr_data = (memory_header_t *)_ptr;
    memory_header_t * ptr_header = ptr_data - 1;

    _info->memory_allocate -= ptr_header->size;

    ae_uint32_t index = 0;
    for( ; index != _info->memory_records_count; ++index )
    {
        memory_header_t * header = _info->memory_records + index;

        if( header->size != ptr_header->size )
        {
            continue;
        }

        if( header->line != ptr_header->line )
        {
            continue;
        }

        if( strcmp( header->file, ptr_header->file ) != 0 )
        {
            continue;
        }

        _info->memory_records[index] = _info->memory_records[--_info->memory_records_count];

        break;
    }

    free( (ae_voidptr_t)ptr_header );
}
//////////////////////////////////////////////////////////////////////////
#ifdef AE_MOVIE_MEMORY_DEBUG
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_voidptr_t stdlib_movie_alloc( ae_userdata_t _userdata, ae_size_t _size, const ae_char_t * _file, ae_uint32_t _line ) {
    memory_info_t * info = (memory_info_t *)_userdata;

    info->memory_allocate += _size;

	ae_voidptr_t ptr_data = memory_info_alloc( info, _size, _file, _line);

    return ptr_data;
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_voidptr_t stdlib_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count, const ae_char_t * _file, ae_uint32_t _line ) {
    memory_info_t * info = (memory_info_t *)_userdata;

    ae_size_t total = _size * _count;

    info->memory_allocate += total;

	ae_voidptr_t ptr_data = memory_info_alloc( info, total, _file, _line );

    return ptr_data;
}
#else
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_voidptr_t stdlib_movie_alloc( ae_userdata_t _userdata, ae_size_t _size ) {
    memory_info_t * info = (memory_info_t *)_userdata;

    info->memory_allocate += _size;

	ae_voidptr_t ptr_data = memory_info_alloc( info, _size, "", 0 );

    return ptr_data;
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_voidptr_t stdlib_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count ) {
    memory_info_t * info = (memory_info_t *)_userdata;

    ae_size_t total = _size * _count;

    info->memory_allocate += total;

	ae_voidptr_t ptr_data = memory_info_alloc( info, total, "", 0 );

    return ptr_data;
}
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t stdlib_movie_free( ae_userdata_t _userdata, ae_constvoidptr_t _ptr ) {
    memory_info_t * info = (memory_info_t *)_userdata;

    if( _ptr == AE_NULLPTR )
    {
        return;
    }

    memory_info_free( info, _ptr );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t stdlib_movie_free_n( ae_userdata_t _userdata, ae_constvoidptr_t _ptr ) {
    memory_info_t * info = (memory_info_t *)_userdata;

    if( _ptr == AE_NULLPTR )
    {
        return;
    }

    memory_info_free( info, _ptr );
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_size_t __read_file( ae_userdata_t _data, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size )
{
    AE_UNUSED( _carriage );

    FILE * f = (FILE *)_data;

    ae_size_t s = fread( _buff, 1, _size, f );

    return s;
}
//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_void_t __memory_copy( ae_userdata_t _data, ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size )
{
    AE_UNUSED( _data );

    memcpy( _dst, _src, _size );
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    AE_UNUSED( argc );
    AE_UNUSED( argv );

    memory_info_t * mi = (memory_info_t *)malloc( sizeof( memory_info_t ) );
    mi->memory_allocate = 0;
    mi->memory_records_count = 0;

    const aeMovieInstance * movieInstance = ae_create_movie_instance( AE_HASHKEY_EMPTY
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , (ae_movie_strncmp_t)AE_FUNCTION_NULL
        , (ae_movie_logger_t)AE_FUNCTION_NULL
        , mi );

    if( movieInstance == AE_NULLPTR )
    {
        return EXIT_FAILURE;
    }

    aeMovieDataProviders data_providers;
    ae_clear_movie_data_providers( &data_providers );

    ae_uint32_t index = 0;
    for( ; index != sizeof( test_example_file_paths ) / sizeof( test_example_file_paths[0] ); ++index )
    {
        const char * test_example_file_path = test_example_file_paths[index];

        aeMovieData * movieData = ae_create_movie_data( movieInstance, &data_providers, AE_USERDATA_NULL );

        char full_example_file_path[256];
        sprintf( full_example_file_path, "%s/../%s"
            , argv[1]
            , test_example_file_path
        );

        FILE * f = fopen( full_example_file_path, "rb" );

        if( f == NULL )
        {
            return EXIT_FAILURE;
        }

        aeMovieStream * movieStream = ae_create_movie_stream( movieInstance, &__read_file, &__memory_copy, f );

        ae_uint32_t load_major_version;
        ae_uint32_t load_minor_version;
        ae_result_t load_movie_data_result = ae_load_movie_data( movieData, movieStream, &load_major_version, &load_minor_version );

        ae_delete_movie_stream( movieStream );

        fclose( f );

        if( load_movie_data_result != AE_RESULT_SUCCESSFUL )
        {
            return EXIT_FAILURE;
        }

        ae_uint32_t composition_data_count = ae_get_movie_composition_data_count( movieData );

        ae_uint32_t index_composition_data = 0;
        for( ; index_composition_data != composition_data_count; ++index_composition_data )
        {
            const aeMovieCompositionData * composition_data = ae_get_movie_composition_data_by_index( movieData, index_composition_data );

            aeMovieCompositionProviders providers;
            ae_initialize_movie_composition_providers( &providers );

            const aeMovieComposition * composition = ae_create_movie_composition( movieData, composition_data, AE_TRUE, &providers, AE_USERDATA_NULL );

            if( composition == AE_NULLPTR )
            {
                return EXIT_FAILURE;
            }

            ae_delete_movie_composition( composition );
        }        

        ae_delete_movie_data( movieData );
    }

    ae_delete_movie_instance( movieInstance );

    ae_uint32_t index_memory_record = 0;
    for( ; index_memory_record != mi->memory_records_count; ++index_memory_record )
    {
        memory_header_t * header = mi->memory_records + index_memory_record;
        printf( "file '%s[%d]' leak '%d' bytes\n"
            , header->file
            , header->line
            , header->size );
    }

    if( mi->memory_allocate != 0 )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
