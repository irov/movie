/*
==================================================
    EXAMPLE #3

Updates & rendering.
==================================================
*/

#include "common.h"

static const ae_char_t * ex_example_license_hash = "52ad6f051099762d0a0787b4eb2d07c8a0ee4491";
static const ae_char_t * ex_example_movie_path = "../../examples/resources/Knight/Knight.aem";
static const ae_char_t * ex_example_composition_name = "Knight";

int main( int argc, char *argv[] ) {
    AE_UNUSED( argc );
    AE_UNUSED( argv );

    //
    // Initialize library instance, movie & composition names.
    //

    ex_init( ex_example_license_hash, ex_example_movie_path, ex_example_composition_name );

    //
    // Set pointer to resource loader callback.
    //

    ex.resource_provider = &ex_callback_resource_provider;
    ex.resource_deleter = &ex_callback_resource_deleter_empty;

    //
    // Load movie data structure, which in particular contains info about all compositions in the movie.
    //

    ex_load_movie_data();

    //
    // Set composition data provider callbacks here.
    //

    ae_initialize_movie_composition_providers( &ex.comp_providers );    
    ex.comp_providers.node_provider = &ex_callback_node_provider;
    ex.comp_providers.node_deleter = &ex_callback_node_destroyer;
    ex.comp_providers.node_update = &ex_callback_node_update;
    ex.comp_providers.camera_provider = &ex_callback_camera_provider;
    ex.comp_providers.track_matte_update = &ex_callback_track_matte_update;
    ex.comp_providers.composition_event = &ex_callback_composition_event;
    ex.comp_providers.composition_state = &ex_callback_composition_state;

    //
    // Select the composition & load its data (nodes, cameras etc.).
    //

    ex_set_composition();

    //
    // Play the movie.
    //

    EX_LOG( "The movie is playing in loop now. Press Ctrl-C to break...\n" );

    ae_play_movie_composition( ex.composition, 0.f );

    ae_float_t last = ex_get_time();

    while( ae_is_play_movie_composition( ex.composition ) == AE_TRUE ) {
        ae_float_t t = ex_get_time();
        ae_float_t dt = t - last;
        last = t;

        ex_update( dt );
        ex_render();
    }

    //
    // Shutdown.
    //

    ex_shutdown();

    return EXIT_SUCCESS;
}
