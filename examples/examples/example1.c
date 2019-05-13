/*
==================================================
    EXAMPLE #1

Load movie file & give basic info about it.
No resource or node providers.
==================================================
*/

#include "common.h"

static const ae_char_t * ex_example_movie_path = "../../examples/resources/Knight/Knight.aem";
static const ae_char_t * ex_example_composition_name = "Knight";

int main( int argc, char *argv[] ) {
    AE_UNUSED( argc );
    AE_UNUSED( argv );
    //
    // Initialize library instance, movie & composition names.
    //

    ex_init( AE_HASHKEY_EMPTY, ex_example_movie_path, ex_example_composition_name );

    //
    // Set pointer to resource loader callback, in this example the empty one.
    //

    ex.resource_provider = &ex_callback_resource_provider_empty;
    ex.resource_deleter = &ex_callback_resource_deleter_empty;

    //
    // Load movie data structure, which in particular contains info about all compositions in the movie.
    //

    ex_load_movie_data();

    //
    // Usually we should set composition provider callbacks here,
    // but for the current example we are using AE_NULL ones.
    //

    //
    // Select the composition & load its data (nodes, cameras etc.).
    //

    ex_set_composition();

    //
    // Usually it can be played at this moment.
    //

    EX_LOG( "The movie composition has loaded successfully. See further examples.\n" );

    //
    // Shutdown.
    //

    ex_shutdown();

    return EXIT_SUCCESS;
}
