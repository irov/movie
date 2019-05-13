#include "movie/movie.h"

#include <stdlib.h>

AE_CALLBACK ae_voidptr_t stdlib_movie_alloc( ae_userdata_t _userdata, ae_size_t _size ) {
    AE_UNUSED( _userdata );
    return malloc( _size );
}

AE_CALLBACK ae_voidptr_t stdlib_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count ) {
    AE_UNUSED( _userdata );
    ae_size_t total = _size * _count;
    return malloc( total );
}

AE_CALLBACK ae_void_t stdlib_movie_free( ae_userdata_t _data, ae_constvoidptr_t _ptr ) {
    AE_UNUSED( _data );
    free( (ae_voidptr_t)_ptr );
}

AE_CALLBACK ae_void_t stdlib_movie_free_n( ae_userdata_t _data, ae_constvoidptr_t _ptr ) {
    AE_UNUSED( _data );
    free( (ae_voidptr_t)_ptr );
}

int main( int argc, char *argv[] )
{
    AE_UNUSED( argc );
    AE_UNUSED( argv );

    const aeMovieInstance * movieInstance = ae_create_movie_instance( AE_HASHKEY_EMPTY
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , (ae_movie_strncmp_t)AE_FUNCTION_NULL
        , (ae_movie_logger_t)AE_FUNCTION_NULL
        , AE_NULLPTR );

    if( movieInstance == AE_NULLPTR )
    {
        return EXIT_FAILURE;
    }

    ae_delete_movie_instance( movieInstance );

    return EXIT_SUCCESS;
}
