#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
using namespace std;
const float SCREEN_WIDTH = 600;
const float SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

const int AXES_ARROW = 5;
const float AXES_MARGE = 20;

const int BLOCK = 0.1;
float ZOOM = 1;

int sizeText = 10;
float AXES_CENTER[2] = {(SCREEN_WIDTH/2),SCREEN_HEIGHT/2};
const int AXES_COLOR[3] = {0,0,0};
float AXES_VALUES_X[3] = {-10,10,1}; // min, max, step
float AXES_VALUES_Y[3] = {-10,10,1};
float x_unity;
float y_unity;
//Les surfaces que nous allons utiliser
SDL_Surface *message = NULL;
SDL_Surface *texte;
SDL_Surface *texte2 = NULL;
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
TTF_Font *police = NULL;
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
void debugg(SDL_Surface* screen, float calcul){
    char xy[100];
    sprintf(xy, "%.5f",calcul);
    texte = TTF_RenderText_Solid(police, xy, {200,0,0});
    apply_surface(50, SCREEN_HEIGHT-texte->h-5, texte, screen );
}
void setPixel(int x, int y, Uint32 coul){
 SDL_Surface*ecran;
ecran = SDL_SetVideoMode(640,480,32,SDL_HWSURFACE);
  *((Uint32*)(ecran->pixels) + x + y * ecran->w) = coul;
}
void setPixelVerif(int x, int y, Uint32 coul){
 SDL_Surface*ecran;
ecran = SDL_SetVideoMode(640,480,32,SDL_HWSURFACE);
  if (x >= 0 && x < ecran->w &&
      y >= 0 && y < ecran->h)
    setPixel(x, y, coul);
}
void echangerEntiers(int* x, int* y){
  int t = *x;
  *x = *y;
  *y = t;
}
void ligne(int x1, int y1, int x2, int y2, Uint32 coul){
  int d, dx, dy, aincr, bincr, xincr, yincr, x, y;

  if (abs(x2 - x1) < abs(y2 - y1)) {
    /* parcours par l'axe vertical */

    if (y1 > y2) {
      echangerEntiers(&x1, &x2);
      echangerEntiers(&y1, &y2);
    }

    xincr = x2 > x1 ? 1 : -1;
    dy = y2 - y1;
    dx = abs(x2 - x1);
    d = 2 * dx - dy;
    aincr = 2 * (dx - dy);
    bincr = 2 * dx;
    x = x1;
    y = y1;

    setPixelVerif(x, y, coul);

    for (y = y1+1; y <= y2; ++y) {
      if (d >= 0) {
    x += xincr;
    d += aincr;
      } else
    d += bincr;

      setPixelVerif(x, y, coul);
    }

  } else {
    /* parcours par l'axe horizontal */

    if (x1 > x2) {
      echangerEntiers(&x1, &x2);
      echangerEntiers(&y1, &y2);
    }

    yincr = y2 > y1 ? 1 : -1;
    dx = x2 - x1;
    dy = abs(y2 - y1);
    d = 2 * dy - dx;
    aincr = 2 * (dy - dx);
    bincr = 2 * dy;
    x = x1;
    y = y1;

    setPixelVerif(x, y, coul);

    for (x = x1+1; x <= x2; ++x) {
      if (d >= 0) {
    y += yincr;
    d += aincr;
      } else
    d += bincr;

      setPixelVerif(x, y, coul);
    }
  }
}
void pointer_precision(SDL_Surface* screen, const int* color){
    int x = event.motion.x;
    if(x>=AXES_MARGE && x<=SCREEN_WIDTH-AXES_MARGE){
        for(int i=AXES_MARGE;i<SCREEN_HEIGHT-AXES_MARGE;i++){
            putpixel(screen,x, i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
        }
    }
    int y = event.motion.y;
    if(y>=AXES_MARGE && y<=SCREEN_HEIGHT-AXES_MARGE){
        for(int i=AXES_MARGE;i<SCREEN_WIDTH-AXES_MARGE;i++){
            putpixel(screen,i, y, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
        }
    }
    char xy[100];
    sprintf(xy, "%.5f ; %.5f",(x-AXES_CENTER[0])/x_unity, -1*(y-AXES_CENTER[1])/y_unity);
    texte = TTF_RenderText_Solid(police, xy, {200,0,0});
    apply_surface(SCREEN_WIDTH-texte->w-10, SCREEN_HEIGHT-texte->h-5, texte, screen );
}
void SDL_PutPixel32(SDL_Surface *surface, int x, int y, Uint32 pixel){
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32*)p = pixel;
}
Uint32 SDL_GetPixel32(SDL_Surface *surface, int x, int y){
    Uint8 *p = (Uint8*)surface->pixels + y * surface->pitch + x * 4;
    return *(Uint32*)p;
}
void Line(SDL_Surface* surf,int x1,int y1, int x2,int y2,Uint32 couleur){
  int x,y;
  int Dx,Dy;
  int xincr,yincr;
  int erreur;
  int i;

  Dx = abs(x2-x1);
  Dy = abs(y2-y1);
  if(x1<x2)
    xincr = 1;
  else
    xincr = -1;
  if(y1<y2)
    yincr = 1;
  else
    yincr = -1;

  x = x1;
  y = y1;
  if(Dx>Dy)
    {
      erreur = Dx/2;
      for(i=0;i<Dx;i++)
        {
          x += xincr;
          erreur += Dy;
          if(erreur>Dx)
            {
              erreur -= Dx;
              y += yincr;
            }
        SDL_PutPixel32(surf,x, y,couleur);
        }
    }
  else
    {
      erreur = Dy/2;
      for(i=0;i<Dy;i++)
        {
          y += yincr;
          erreur += Dx;
          if(erreur>Dy)
            {
              erreur -= Dy;
              x += xincr;
            }
        SDL_PutPixel32(surf,x, y,couleur);
        }
    }
    SDL_PutPixel32(surf,x1,y1,couleur);
    SDL_PutPixel32(surf,x2,y2,couleur);
}
void set_point(SDL_Surface* screen,vector< vector<double> >points, const int* color, int ligne){
    if(points[0].size() == 0)return;
    for(int z = 0; z < ligne; ++z){
        double x = points[z][0];
        double y = points[z][1];
        int r = 0;
        for(int i=-r; i <= r ; i++){
            for(int j=-r; j <= r ; j++){
                if(AXES_CENTER[0]+x*x_unity<=SCREEN_WIDTH-AXES_MARGE && AXES_CENTER[0]+x*x_unity>=AXES_MARGE && AXES_CENTER[1]-y*y_unity>=AXES_MARGE && AXES_CENTER[1]-y*y_unity<=SCREEN_HEIGHT-AXES_MARGE){
                    putpixel(screen,AXES_CENTER[0]+x*x_unity+i, AXES_CENTER[1]-y*y_unity+j, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
            }
        }
    }
}
void relierP(SDL_Surface* screen,vector< vector<double> >points, Uint32 coul, int ligne){
    //float x_min = -1*(AXES_CENTER[0]-AXES_MARGE)/x_unity;
    //float x_max = (SCREEN_WIDTH-AXES_CENTER[0]-AXES_MARGE)/x_unity;
    //float y_max = (AXES_CENTER[1]-AXES_MARGE)/y_unity;
    //float y_min = -1*(SCREEN_HEIGHT-AXES_CENTER[1]-AXES_MARGE)/y_unity;
    for(int z = 0; z < ligne-1; ++z){
        double x1 = AXES_CENTER[0]+points[z][0]*x_unity;
        double x2 = AXES_CENTER[0]+points[z+1][0]*x_unity;
        double y1 = AXES_CENTER[1]-points[z][1]*y_unity;
        double y2 = AXES_CENTER[1]-points[z+1][1]*y_unity;
        if(x1>=AXES_MARGE && x2<=SCREEN_WIDTH-AXES_MARGE){
            if(y1<=SCREEN_HEIGHT-AXES_MARGE && y1 >= AXES_MARGE  && y2<=SCREEN_HEIGHT-AXES_MARGE && y2 >= AXES_MARGE)
                Line(screen,x1,y1,x2,y2,coul);
        }
    }
}
void set_axes(SDL_Surface* screen, int x, int y, const int* color, bool start){
    x_unity = (int)((SCREEN_WIDTH-2*AXES_MARGE)/(AXES_VALUES_X[1]-AXES_VALUES_X[0])*ZOOM);
    y_unity = (int)((SCREEN_HEIGHT-2*AXES_MARGE)/(AXES_VALUES_Y[1]-AXES_VALUES_Y[0])*ZOOM);
    //float   marge_min = AXES_VALUES_X[0];
    //float   marge_max = AXES_VALUES_X[1];
    float position_x0 = (int)AXES_CENTER[0];
    float position_y0 = (int)AXES_CENTER[1];
    { /* MES X TAVU */
    // PARTIE AVANT 0
    for(float i=position_x0; i>=AXES_MARGE; i-=x_unity*AXES_VALUES_X[2]){ // baton baton !
        if(i<=SCREEN_WIDTH-AXES_MARGE && i>=AXES_MARGE){
            for(float taille=0; taille<=AXES_ARROW; taille++){
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
    for(float i=position_x0; i<=SCREEN_WIDTH-AXES_MARGE; i+=x_unity*AXES_VALUES_X[2]){ // baton baton !
        if(i<=SCREEN_WIDTH-AXES_MARGE && i>=AXES_MARGE){
            for(float taille=0; taille<=AXES_ARROW; taille++){
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
    if(AXES_CENTER[1]>=AXES_MARGE && AXES_CENTER[1]<=SCREEN_HEIGHT-AXES_MARGE){
        for(float i=AXES_MARGE; i<=SCREEN_WIDTH-AXES_MARGE; i++ ){
            putpixel(screen,i, position_y0, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
        }
        for(float i=SCREEN_WIDTH-AXES_MARGE; i<=SCREEN_WIDTH+AXES_ARROW; i++ ){
            for( float j=SCREEN_WIDTH-AXES_MARGE-i+AXES_ARROW ; j>=0 ; j--){
                putpixel(screen,i, position_y0+j, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                putpixel(screen,i, position_y0-j, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
        /* FIN DE MES X TAVU */

    }
    { /* MES Y TAVU */
    for(int i=position_y0; i>=AXES_MARGE; i-=y_unity*AXES_VALUES_Y[2]){ // baton baton !
        if(i<=SCREEN_HEIGHT-AXES_MARGE && i>=AXES_MARGE){
            for(float taille=0; taille<=AXES_ARROW; taille++){
                putpixel(screen,(taille+AXES_MARGE), i,SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                if(AXES_CENTER[0]>AXES_MARGE && AXES_CENTER[0]<SCREEN_WIDTH-AXES_MARGE){
                    putpixel(screen,((int)position_x0+taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    putpixel(screen,((int)position_x0-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
                putpixel(screen,((SCREEN_WIDTH-AXES_MARGE)-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    for(float i=position_y0; i<=SCREEN_WIDTH-AXES_MARGE; i+=y_unity*AXES_VALUES_Y[2]){ // baton baton !
        if(i<=SCREEN_HEIGHT-AXES_MARGE && i>=AXES_MARGE){
            for(float taille=0; taille<=AXES_ARROW; taille++){
                putpixel(screen,(taille+AXES_MARGE), i,SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                if(AXES_CENTER[0]>AXES_MARGE && AXES_CENTER[0]<SCREEN_WIDTH-AXES_MARGE){
                    putpixel(screen,((int)position_x0+taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                    putpixel(screen,((int)position_x0-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
                }
                putpixel(screen,((SCREEN_WIDTH-AXES_MARGE)-taille), i, SDL_MapRGB(screen->format,color[0], color[1], color[2]));
            }
        }
    }
    if(AXES_CENTER[0]>=AXES_MARGE && AXES_CENTER[0]<=SCREEN_WIDTH-AXES_MARGE){
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
    { //TEXTE TOP
        for(int i=position_x0; i>=AXES_MARGE; i-=x_unity){
            if(i<=SCREEN_WIDTH-AXES_MARGE && i>=AXES_MARGE){
                char yolo[100];
                    sprintf(yolo, "%d", (int)(((float)i-position_x0)/x_unity));
                    texte = TTF_RenderText_Blended(police, yolo, {200,100,0});
                    apply_surface( i-texte->w/2+1, AXES_MARGE-15, texte, screen );
                }
            }
        for(float i=position_x0; i<=SCREEN_WIDTH-AXES_MARGE; i+=x_unity){
            if(i<=SCREEN_WIDTH-AXES_MARGE && i>=AXES_MARGE){
                char yolo[100];
                sprintf(yolo, "%d", (int)(((float)i-position_x0)/x_unity));
                texte = TTF_RenderText_Blended(police, yolo, {200,100,0});
                apply_surface( i-texte->w/2+1, AXES_MARGE-15, texte, screen );
            }
        }
    }
    { //TEXTE GAUCHE
        for(float i=position_y0; i>=AXES_MARGE; i-=y_unity){
            if(i<=SCREEN_HEIGHT-AXES_MARGE && i>=AXES_MARGE){
                char yolo[100];
                sprintf(yolo, "%d", (int)(-1*((float)i-position_y0)/y_unity));
                texte = TTF_RenderText_Blended(police, yolo, {200,100,0});
                apply_surface(AXES_MARGE-15, i-texte->h/2+1, texte, screen );
            }
        }
        for(float i=position_y0; i<=SCREEN_WIDTH-AXES_MARGE; i+=y_unity){
            if(i<=SCREEN_HEIGHT-AXES_MARGE && i>=AXES_MARGE){
                char yolo[100];
                sprintf(yolo, "%d", (int)(-1*((float)i-position_y0)/y_unity));
                texte = TTF_RenderText_Blended(police, yolo, {200,100,0});
                apply_surface(AXES_MARGE-15, i-texte->h/2, texte, screen );
            }
        }
    }
}

int main( int argc, char *argv[ ] ){
    fstream fichier("fichierResultat.txt");
    fstream option("option.txt");

    int  ligne=0;
    string values;
    //float min=0,max=0,
    float nbFonction=0;
    vector< vector<double> >points;
    vector<vector< vector<double> > >fonctions;

    bool continuer = true;
    SDL_Surface *screen;
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 ){
        printf( "Can't init SDL:  %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }
    if(TTF_Init() == -1){
        fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    atexit( SDL_Quit );
    police = TTF_OpenFont("arial.ttf", 10);
    //Mise en place de l'écran
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT+1, SCREEN_BPP, SDL_SWSURFACE | SDL_DOUBLEBUF );
    if( screen == NULL ){
        printf( "Can't set video mode: %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }
    SDL_FillRect(screen, NULL, 0x000000); // 0xFFFFFF = white in RGB, NULL = full window
    SDL_Flip(screen);
	SDL_WM_SetCaption( "LA SDL C RIGOLO", NULL ); //titre fenetre
    set_axes(screen, AXES_CENTER[0], AXES_CENTER[1], AXES_COLOR, true);
    if(option){
        getline(option,values);
        AXES_VALUES_X[0] = atof(values.c_str());
        getline(option,values);
        AXES_VALUES_X[1] = atof(values.c_str());
        getline(option,values);
        AXES_VALUES_X[2] = atof(values.c_str());
        getline(option,values);
        nbFonction = atof(values.c_str());
        option.close();
    }
    else{
        cout << "Erreur" <<endl;
    }
    if(fichier){
        while(getline(fichier,values)){
            ligne++;
            if(values[0] == '#'){
                fonctions.push_back(points);
                points.clear();
            }else{
                double x = atof(values.c_str());
                getline(fichier,values);
                double y = atof(values.c_str());
                vector<double> row;
                row.push_back(x);
                row.push_back(y);
                points.push_back(row);
            }
        }
        fichier.close();
    }else{
        cout << "Erreur" <<endl;
    }
    int move_cursor=0, my_x=0, my_y=0, new_x=0, new_y=0;
            bool precisionMode = false, start = true;
    AXES_CENTER[0] = -1*(AXES_VALUES_X[0])*x_unity+AXES_MARGE;
    AXES_CENTER[1] = -1*(-1*(AXES_VALUES_Y[1])*y_unity-AXES_MARGE);
    while (continuer){  //EVENTS
        if( SDL_Flip( screen ) == -1 ) {
            return EXIT_FAILURE;
        }
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if( event.button.button == SDL_BUTTON_LEFT ){
                    move_cursor = 1;
                    my_x = event.motion.x;
                    my_y = event.motion.y;
                    break;
                }
                if( event.button.button == SDL_BUTTON_RIGHT ){
                    precisionMode = !precisionMode;
                    break;
                }
                if( event.button.button == SDL_BUTTON_WHEELUP ){
                    if(ZOOM < 2)ZOOM += 0.1;
                    break;
                }
                if( event.button.button == SDL_BUTTON_WHEELDOWN ){
                    if(ZOOM > 0.5)ZOOM -= 0.1;
                    break;
                }
            case SDL_MOUSEBUTTONUP:
                if( event.button.button == SDL_BUTTON_LEFT ){
                    move_cursor = 0;
                    break;
                }
            case SDL_MOUSEMOTION:
                start = false;
                new_x = event.motion.x;
                new_y = event.motion.y;
                if(move_cursor == 1){
                    AXES_CENTER[0] = AXES_CENTER[0] - my_x + new_x;
                    AXES_CENTER[1] = AXES_CENTER[1] - my_y + new_y;
                    my_x = new_x;
                    my_y = new_y;
                }
                break;
        }

        SDL_FillRect(screen, NULL, 0xf0f0f0); // 0xFFFFFF = white in RGB, NULL = full window
        set_axes(screen, AXES_CENTER[0], AXES_CENTER[1], AXES_COLOR, start);
        //set_point(screen,fonctions[1], AXES_COLOR, fonctions[1].size());
        if(precisionMode)pointer_precision(screen,AXES_COLOR);
        for(double i=0; i<fonctions.size()/2 ; i++){ // dégradé rouge -> bleu
            Uint8 coloration = 0xff;
            Uint8 pas = coloration/(fonctions.size());
            Uint32 coul = (coloration-pas*i)*0x010000+(pas*i)*0x000100;
            relierP(screen, fonctions[i],coul, fonctions[i].size());
        }
        for(double i=fonctions.size()/2; i<fonctions.size() ; i++){ // dégradé bleu -> rouge
            Uint8 coloration = 0xff;
            Uint8 pas = coloration/(fonctions.size());
            Uint32 coul = (coloration-pas*i)*0x000100+(pas*i)*0x000001;
            relierP(screen, fonctions[i],coul, fonctions[i].size());
        }
        debugg(screen, fonctions.size());
        //SDL_Flip(screen);
    }
	//Libération des surfaces
	SDL_FreeSurface( message );
	SDL_FreeSurface( background );

	//On quitte SDL
	TTF_CloseFont(police);
	TTF_Quit();
	SDL_Quit();
    return EXIT_SUCCESS;
}
