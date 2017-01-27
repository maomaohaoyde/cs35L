//
//  main.c
//  srt
//
//  Created by vector on 11/2/10.
//  Copyright (c) 2010 Brian F. Allen.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "raymath.h"
#include "shaders.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>


static double dirs[6][3] =
{ {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1} };
static const int opposites[] = { 1, 0, 3, 2, 5, 4 };


static void
add_sphereflake( scene_t* scene, int sphere_id, int parent_id, int dir,
                double ratio, int recursion_level )
{
    sphere_t* parent = &scene->spheres[parent_id];
    sphere_t* child = &scene->spheres[sphere_id];
    
    /* start at parents origin */
    mul( child->org, dirs[dir], (1.+ratio)*parent->rad );
    add( child->org, child->org, parent->org );
    child->rad = parent->rad * ratio;
    copy( child->color, parent->color );
    child->shader = parent->shader;
    scene->sphere_count++;
}

static int
recursive_add_sphereflake( scene_t* scene, int parent_id, int parent_dir,
                          int sphere_id, int dir,
                          int recursion_level, int recursion_limit )
{
    const double ratio = 0.35;
    
    add_sphereflake( scene, sphere_id, parent_id, dir, ratio, recursion_level );
    if( recursion_level > recursion_limit )
    {
        return sphere_id + 1;
    }
    
    /* six children, one at each cardinal point */
    parent_id = sphere_id;
    sphere_id = sphere_id + 1;
    int child_dir;
    for( child_dir=0; child_dir<6; ++child_dir )
    {
        /* skip making spheres inside parent */
        if( parent_dir == opposites[child_dir] ) continue;
        sphere_id = recursive_add_sphereflake( scene, parent_id, parent_dir,
                                              sphere_id, child_dir,
                                              recursion_level + 1,
                                              recursion_limit );
    }
    return sphere_id;
}

static scene_t
create_sphereflake_scene( int recursion_limit )
{
    scene_t scene;
    Vec3 color;
    sphere_t* sphere;
    
    init_scene( &scene );
    add_light( &scene, 2, 5, 0, 0.92, 0.76, 0.771 );
    add_light( &scene, -5, 3, -5, 0.96, 0.93, 0.88 );
    int max_sphere_count = 2 + powl( 6, recursion_limit + 2 );
    scene.spheres = realloc( scene.spheres,
                            max_sphere_count*sizeof( sphere_t ) );
    if( !scene.spheres )
    {
        fprintf( stderr, "Failed to get memory for sphereflake.  aborting.\n" );
        exit( -1 );
    }
    
    //    sphere = &(scene.spheres[0]);
    //    set( sphere->org, -0.5, -1.0, 0 );
    //    sphere->rad = 0.75;
    //    set( color, 0.85, 0.25, 0.25 );
    //    copy( sphere->color, color );
    //    sphere->shader = mirror_shader;
    
    
    /* center sphere is special, child inherent shader and color */
    sphere = &(scene.spheres[0]);
    scene.sphere_count++;
    set( sphere->org, 0, -1, 0 );
    sphere->rad = 0.75;
    set( color, 0.75, 0.75, 0.75 );
    copy( sphere->color, color );
    sphere->shader = mirror_shader;
    recursive_add_sphereflake( &scene,
                              0, /* parent is the first sphere */
                              -1, /* -1 means no dir, make all children */
                              1, /* next free sphere index */
                              2, /* starting dir */
                              0, /* starting recursion level */
                              recursion_limit );
    
    return scene;
}

static void
free_scene( scene_t* arg )
{
    free( arg->lights );
    arg->light_count = 0;
    free( arg->spheres );
    arg->sphere_count = 0;
}

/******
 * Constants that have a large effect on performance */

/* how many levels to generate spheres */
enum { sphereflake_recursion = 3 };

/* output image size */
enum { height = 131 };
enum { width = 131 };

/* antialiasing samples, more is higher quality, 0 for no AA */
enum { halfSamples = 4 };
/******/

/* color depth to output for ppm */
enum { max_color = 255 };

/* z value for ray */
enum { z = 1 };

scene_t scene;
struct arguments
{
    int start;
    int end;
    double pixel_dy;
    double pixel_dx;
    double subsample_dx;
    double subsample_dy;
    Vec3 *bg_color;
    Vec3 *camera_pos;
    Vec3 **pixel_color;
};

void threadFunction(void* b)
{
    struct arguments *a=(struct arguments*)b;
    int px;
    for (px=a->start; px<a->end; px++)
    {
        a->pixel_color[px]=(Vec3*)malloc(width*sizeof(Vec3));
        const double x = a->pixel_dx * ((double)( px-(width/2) ));
        int py;
        for( py=0; py<height; ++py )
        {
            const double y = a->pixel_dy * ((double)( py-(height/2) ));
            int xs;
                for( xs=-halfSamples; xs<=halfSamples; ++xs )
                {   int ys;
            for( ys=-halfSamples; ys<=halfSamples; ++ys )
            {
                double subx = x + ((double)xs)*a->subsample_dx;
                double suby = y + ((double)ys)*a->subsample_dy;
                
                /* construct the ray coming out of the camera, through
                 * the screen at (subx,suby)
                 */
                ray_t pixel_ray;
                copy( pixel_ray.org, *(a->camera_pos) );
                Vec3 pixel_target;
                set( pixel_target, subx, suby, z );
                sub( pixel_ray.dir, pixel_target, *(a->camera_pos) );
                norm( pixel_ray.dir, pixel_ray.dir );
                Vec3 sample_color;
                copy( sample_color, *(a->bg_color) );
                /* trace the ray from the camera that
                 * passes through this pixel */
                trace( &scene, sample_color, &pixel_ray, 0 );
                /* sum color for subpixel AA */
                add( a->pixel_color[px][py], a->pixel_color[px][py], sample_color );
            }
        }
            if( halfSamples )
            {
                mul( a->pixel_color[px][py], a->pixel_color[px][py],
                    (1.0/( 4.0 * halfSamples * halfSamples ) ) );
            }
        }
 }
}
int
main( int argc, char **argv )
{
    int nthreads = argc == 2 ? atoi( argv[1] ) : 0;
    
    if( nthreads < 1 )
    {
        fprintf( stderr, "%s: usage: %s NTHREADS\n", argv[0], argv[0] );
        return 1;
    }
    
    
    scene = create_sphereflake_scene( sphereflake_recursion );
    
    /* Write the image format header */
    /* P3 is an ASCII-formatted, color, PPM file */
    printf( "P3\n%d %d\n%d\n", width, height, max_color );
    printf( "# Rendering scene with %d spheres and %d lights\n",
           scene.sphere_count,
           scene.light_count );
    
    Vec3 camera_pos;
    set( camera_pos, 0., 0., -4. );
    Vec3 camera_dir;
    set( camera_dir, 0., 0., 1. );
    const double camera_fov = 75.0 * (PI/180.0);
    Vec3 bg_color;
    set( bg_color, 0.8, 0.8, 1 );
    
    const double pixel_dx = tan( 0.5*camera_fov ) / ((double)width*0.5);
    const double pixel_dy = tan( 0.5*camera_fov ) / ((double)height*0.5);
    const double subsample_dx
    = halfSamples  ? pixel_dx / ((double)halfSamples*2.0)
    : pixel_dx;
    const double subsample_dy
    = halfSamples ? pixel_dy / ((double)halfSamples*2.0)
    : pixel_dy;
    
    
    
    pthread_t threads[nthreads];
    int ret, t;
    struct arguments *argmt=malloc(sizeof(double)*4+sizeof(int)*2+sizeof(Vec3*)*2+sizeof(Vec3**));
    argmt->pixel_dx=pixel_dx;
    argmt->pixel_dy=pixel_dy;
    argmt->camera_pos=&camera_pos;
    argmt->bg_color=&bg_color;
    argmt->subsample_dx=subsample_dx;
    argmt->subsample_dy=subsample_dy;
    Vec3** pixel_color;
    pixel_color=(Vec3**)malloc(height*sizeof(Vec3*));
    argmt->pixel_color=pixel_color;
    int singleTime=width/nthreads;
    
    for(t = 0; t < nthreads-1; t++)
    {
        argmt->start=t*singleTime;
        argmt->end=(t+1)*singleTime;
        ret = pthread_create(&threads[t], NULL, (void *)threadFunction, (void *) argmt);
        if (ret!=0) {
            fprintf(stderr, "Fail to create new thread!");
        }
    }
    
    
    argmt->start=t*singleTime;
    argmt->end=width;
    ret = pthread_create(&threads[t], NULL, (void *)threadFunction, (void *) argmt);
    if (ret!=0) {
        fprintf(stderr, "Fail to create new thread!");
    }
    
    
        for(t = 0; t < nthreads; t++)
        {
            ret =  pthread_join(threads[t], NULL);
            if (ret!=0) {
                fprintf(stderr, "Fail to join the thread!");
            }
    }
    
    
    /* for every pixel */
    int px;
    for( px=0; px<width; ++px )
    {
        int py;
        for( py=0; py<height; ++py )
        {
            
           
            
            
            /* at this point, have accumulated (2*halfSamples)^2 samples,
             * so need to average out the final pixel color
             */
            
            
            /* done, final floating point color values are in pixel_color */
            float scaled_color[3];
            scaled_color[0] = gamma( pixel_color[px][py][0] ) * max_color;
            scaled_color[1] = gamma( pixel_color[px][py][1] ) * max_color;
            scaled_color[2] = gamma( pixel_color[px][py][2] ) * max_color;
            
            /* enforce caps, replace with real gamma */
            int i;
            for( i=0; i<3; i++)
                scaled_color[i] = max( min(scaled_color[i], 255), 0);
            
            /* write this pixel out to disk. ppm is forgiving about whitespace,
             * but has a maximum of 70 chars/line, so use one line per pixel
             */
            printf( "%.0f %.0f %.0f\n",
                   scaled_color[0], scaled_color[1], scaled_color[2] );
        }
        printf( "\n" );
    }
    
    free_scene( &scene );
    
    if( ferror( stdout ) || fclose( stdout ) != 0 )
    {
        fprintf( stderr, "Output error\n" );
        return 1;
    }
    free(argmt);
    free(pixel_color);
    return 0;

    }
