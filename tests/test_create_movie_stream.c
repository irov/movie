#include "movie/movie.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const ae_char_t * test_example_license_hash = "52ad6f051099762d0a0787b4eb2d07c8a0ee4491";
static const ae_char_t * test_example_file_path = "examples/resources/Knight/Knight.aem";

AE_CALLBACK ae_voidptr_t stdlib_movie_alloc( ae_userdata_t _userdata, ae_size_t _size ) {
    AE_UNUSED( _userdata );
    return malloc( _size );
}

AE_CALLBACK ae_voidptr_t stdlib_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count ) {
    AE_UNUSED( _userdata );
    ae_size_t total = _size * _count;
    return malloc( total );
}

AE_CALLBACK ae_void_t stdlib_movie_free( ae_voidptr_t _data, ae_constvoidptr_t _ptr ) {
    AE_UNUSED( _data );
    free( (ae_voidptr_t)_ptr );
}

AE_CALLBACK ae_void_t stdlib_movie_free_n( ae_voidptr_t _data, ae_constvoidptr_t _ptr ) {
    AE_UNUSED( _data );
    free( (ae_voidptr_t)_ptr );
}

//////////////////////////////////////////////////////////////////////////
AE_CALLBACK ae_size_t __read_file( ae_voidptr_t _data, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size )
{
    AE_UNUSED( _carriage );

    FILE * f = (FILE *)_data;

    ae_size_t s = fread( _buff, 1, _size, f );

    return s;
}

AE_CALLBACK ae_void_t __memory_copy( ae_voidptr_t _data, ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size )
{
    AE_UNUSED( _data );

    memcpy( _dst, _src, _size );
}

int main( int argc, char *argv[] )
{
    AE_UNUSED( argc );
    AE_UNUSED( argv );
        
    const aeMovieInstance * movieInstance = ae_create_movie_instance( test_example_license_hash
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , (ae_movie_strncmp_t)AE_FUNCTION_NULL
        , (ae_movie_logger_t)AE_FUNCTION_NULL
        , AE_NULL );

    if( movieInstance == AE_NULL )
    {
        return EXIT_FAILURE;
    }

    aeMovieDataProviders data_providers;
    ae_clear_movie_data_providers( &data_providers );

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

    aeMovieStream * movie_stream = ae_create_movie_stream( movieInstance, &__read_file, &__memory_copy, f );

    ae_delete_movie_stream( movie_stream );

    fclose( f );

    ae_delete_movie_data( movieData );

    ae_delete_movie_instance( movieInstance );

    return EXIT_SUCCESS;
}
