#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#pragma comment (lib, "sdl.lib")
#pragma comment (lib, "sdlmain.lib")

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 630;
const int SCREEN_BPP = 32;

const int AXES_ARROW = 5;
const int AXES_MARGE = 20;
const int BLOCK = 0.1;

float ZOOM = 1;

float AXES_CENTER[2] = {SCREEN_WIDTH/2,SCREEN_HEIGHT/2};
const int AXES_COLOR[3] = {255,255,0};
float AXES_VALUES_X[3] = {-10,10,1}; // min, max, step
float AXES_VALUES_Y[3] = {-10,10,1};
float x_unity;
float y_unity;

//Les surfaces que nous allons utiliser
SDL_Surface *message = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;

SDL_Event event;

SDL_Surface *load_image( std::string filename ) {
	//Surface tampon qui nous servira pour charger l'image
	SDL_Surface* loadedImage = NULL;

	//L'image optimisée qu'on va utiliser
	SDL_Surface* optimizedImage = NULL;
	//Chargement de l'image
	loadedImage = SDL_LoadBMP( filename.c_str() );

	//Si le chargement se passe bien
	if( loadedImage != NULL ) {
		//Création de l'image optimisée
		optimizedImage = SDL_DisplayFormat( loadedImage );

		//Libération de l'ancienne image
		SDL_FreeSurface( loadedImage );
	}

	//On retourne l'image optimisée
	return optimizedImage;
}
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel){
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination ) {
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;
	//On blitte la surface
	SDL_BlitSurface( source, NULL, destination, &offset );
}

void set_point(SDL_Surface* screen, int** coord, const int* color){
    int *point = (int *)coord;
    for(int z = 0; z < 4; ++z)
    {
        int x = point[0];
        int y = point[1];
        int r = point[2];
        for(int i=-AXES_ARROW; i < AXES_ARROW ; i++){
            if(AXES_CENTER[0]+x*x_unity<=SCREEN_WIDTH-AXES_MARGE && AXES_CENTER[0]+x*x_unity>=AXES_MARGE && AXES_CENTER[1]-y*y_unity>=AXES_MARGE && AXES_CENTER[1]-y*y_unity<=SCREEN_HEIGHT-AXES_MARGE){
                putpixel(screen,AXES_CENTER[0]+x*x_unity+i, AXES_CENTER[1]-y*y_unity+i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                putpixel(screen,AXES_CENTER[0]+x*x_unity-i, AXES_CENTER[1]-y*y_unity+i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
        point += 3;
    }
}

void set_axes(SDL_Surface* screen, int x, int y, const int* color, bool start){
    x_unity = (SCREEN_WIDTH-2*AXES_MARGE)/(AXES_VALUES_X[1]-AXES_VALUES_X[0]);
    y_unity = (SCREEN_HEIGHT-2*AXES_MARGE)/(AXES_VALUES_Y[1]-AXES_VALUES_Y[0]);
    float   marge_min = AXES_VALUES_X[0];
    float   marge_max = AXES_VALUES_X[1];
    float position_x0 = (int)AXES_CENTER[0];
    float position_y0 = (int)AXES_CENTER[1];

    if(start)position_x0 = -1.00*(AXES_VALUES_X[0])*(x_unity);
    if(start && AXES_VALUES_Y[0]<0 && AXES_VALUES_Y[1]>0) position_y0 = -1.00*(AXES_VALUES_Y[0])*(y_unity);
    AXES_CENTER[0] = position_x0;
    AXES_CENTER[1] = position_y0;
    /* MES X TAVU */
    {
    // PARTIE AVANT 0
    for(int i=position_x0; i>=AXES_MARGE; i-=x_unity){ // baton baton !
        if(i<=SCREEN_WIDTH-AXES_MARGE && i>=AXES_MARGE){
            for(int taille=0; taille<=AXES_ARROW; taille++){
                putpixel(screen,i, taille+AXES_MARGE, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    if(AXES_CENTER[1]>AXES_MARGE && AXES_CENTER[1]<SCREEN_HEIGHT-AXES_MARGE){
                    putpixel(screen,i, (int)position_y0+taille, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    putpixel(screen,i, (int)position_y0-taille, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
                putpixel(screen,i, (SCREEN_HEIGHT-AXES_MARGE)-taille, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    //PARTIE APRES 0
    for(int i=position_x0; i<=SCREEN_WIDTH-AXES_MARGE; i+=x_unity){ // baton baton !
        if(i<=SCREEN_WIDTH-AXES_MARGE && i>=AXES_MARGE){
            for(int taille=0; taille<=AXES_ARROW; taille++){
                putpixel(screen,i, taille+AXES_MARGE, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                if(AXES_CENTER[1]>AXES_MARGE && AXES_CENTER[1]<SCREEN_HEIGHT-AXES_MARGE){
                    putpixel(screen,i, (int)position_y0+taille, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    putpixel(screen,i, (int)position_y0-taille, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
                putpixel(screen,i, (SCREEN_HEIGHT-AXES_MARGE)-taille, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    //AXE X + ARROW
    if(AXES_CENTER[1]>AXES_MARGE && AXES_CENTER[1]<SCREEN_HEIGHT-AXES_MARGE){
        for(int i=AXES_MARGE; i<=SCREEN_WIDTH-AXES_MARGE; i++ ){
            putpixel(screen,i, position_y0, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
        }
        for(int i=SCREEN_WIDTH-AXES_MARGE; i<=SCREEN_WIDTH+AXES_ARROW; i++ ){
            for( int j=SCREEN_WIDTH-AXES_MARGE-i+AXES_ARROW ; j>=0 ; j--){
                putpixel(screen,i, position_y0+j, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                putpixel(screen,i, position_y0-j, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
        /* FIN DE MES X TAVU */

    }
    /* MES Y TAVU */            //MISTAKE Y trop grand
    {
    for(int i=position_y0; i>=AXES_MARGE; i-=y_unity){ // baton baton !
        if(i<=SCREEN_HEIGHT-AXES_MARGE && i>=AXES_MARGE){
            for(int taille=0; taille<=AXES_ARROW; taille++){
                putpixel(screen,(taille+AXES_MARGE), i,SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                if(AXES_CENTER[0]>AXES_MARGE && AXES_CENTER[0]<SCREEN_WIDTH-AXES_MARGE){
                    putpixel(screen,((int)position_x0+taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    putpixel(screen,((int)position_x0-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
                putpixel(screen,((SCREEN_WIDTH-AXES_MARGE)-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    for(int i=position_y0; i<=SCREEN_WIDTH-AXES_MARGE; i+=y_unity){ // baton baton !
        if(i<=SCREEN_HEIGHT-AXES_MARGE && i>=AXES_MARGE){
            for(int taille=0; taille<=AXES_ARROW; taille++){
                putpixel(screen,(taille+AXES_MARGE), i,SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                if(AXES_CENTER[0]>AXES_MARGE && AXES_CENTER[0]<SCREEN_WIDTH-AXES_MARGE){
                    putpixel(screen,((int)position_x0+taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    putpixel(screen,((int)position_x0-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
                putpixel(screen,((SCREEN_WIDTH-AXES_MARGE)-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    if(AXES_CENTER[0]>AXES_MARGE && AXES_CENTER[0]<SCREEN_WIDTH-AXES_MARGE){
        for(int i=AXES_MARGE; i<=SCREEN_HEIGHT-AXES_MARGE; i++ ){
            putpixel(screen,position_x0, i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
        }
        for(int i=AXES_ARROW; i>=0; i-- ){
            for(int j=0; j<=i; j++ ){
                putpixel(screen,position_x0-j, i+AXES_MARGE-AXES_ARROW, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                putpixel(screen,position_x0+i-j, i+AXES_MARGE-AXES_ARROW, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    /* FIN DE MES Y TAVU */
    }

}
int main( int argc, char *argv[ ] ){
    bool continuer = true;
    SDL_Surface *screen;
    SDL_Rect pointer;
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 ){
        printf( "Can't init SDL:  %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }
    atexit( SDL_Quit );
//Mise en place de l'écran
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT+1, SCREEN_BPP, SDL_SWSURFACE );
    if( screen == NULL ){
        printf( "Can't set video mode: %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }
    SDL_FillRect(screen, NULL, 0x000000); // 0xFFFFFF = white in RGB, NULL = full window
    SDL_Flip(screen);
	SDL_WM_SetCaption( "LA SDL C RIGOLO", NULL ); //titre fenetre
    set_axes(screen, AXES_CENTER[0], AXES_CENTER[1], AXES_COLOR, true);
    int points[][3] = {
        {0,0,3},
        {1,1,3},
        {2,2,3},
        {-1,-1,3}
    };
    set_point(screen, (int **)points, AXES_COLOR);
    //set_point(screen, points[1], AXES_COLOR);

    int already_down=0, my_x=0, my_y=0, new_x=0, new_y=0, test=0;
    while (continuer){
        if( SDL_Flip( screen ) == -1 ) {
            return EXIT_FAILURE;
        }
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;

        }
        if(event.type ==SDL_MOUSEBUTTONDOWN){
            if( event.button.button == SDL_BUTTON_LEFT ){
                already_down = 1;
            }
            if( event.button.button == SDL_BUTTON_RIGHT ){
                if(ZOOM == 1){
                    ZOOM = 1.5;
                    AXES_VALUES_X[0] *= ZOOM;
                    AXES_VALUES_X[1] *= ZOOM;
                    AXES_VALUES_Y[0] *= ZOOM;
                    AXES_VALUES_Y[1] *= ZOOM;
                }else{
                    AXES_VALUES_X[0] /= ZOOM;
                    AXES_VALUES_X[1] /= ZOOM;
                    AXES_VALUES_Y[0] /= ZOOM;
                    AXES_VALUES_Y[1] /= ZOOM;
                    ZOOM = 1;
                }
            }
        }
        if(event.type ==SDL_MOUSEBUTTONUP){
           already_down = 0;
        }
        if(event.type == SDL_MOUSEMOTION){
            new_x = event.motion.x;
            new_y = event.motion.y;
            int pointer_color[3] = {50,50,50};
            if(already_down){
                /* If the mouse is moving to the left */
                if (event.motion.xrel < 0 && AXES_CENTER[0]>1*x_unity*ZOOM)
                    AXES_CENTER[0]-=5;
                /* If the mouse is moving to the right */
                else if (event.motion.xrel > 0 && AXES_CENTER[0]<20*x_unity*ZOOM)
                    AXES_CENTER[0]+=5;
                /* If the mouse is moving up */
                else if (event.motion.yrel < 0 && AXES_CENTER[1]>1*y_unity*ZOOM)
                    AXES_CENTER[1]-=5;
                /* If the mouse is moving down */
                else if (event.motion.yrel > 0 && AXES_CENTER[1]<20*y_unity*ZOOM)
                    AXES_CENTER[1]+=5;
            }
        }
        SDL_FillRect(screen, NULL, 0xf000f0); // 0xFFFFFF = white in RGB, NULL = full window
        set_axes(screen, AXES_CENTER[0], AXES_CENTER[1], AXES_COLOR, false);
        set_point(screen,(int**) points, AXES_COLOR);
        apply_surface( 0, 200, message, screen );
    }
	//Libération des surfaces
	SDL_FreeSurface( message );
	SDL_FreeSurface( background );

	//On quitte SDL
	SDL_Quit();
    return EXIT_SUCCESS;
}
