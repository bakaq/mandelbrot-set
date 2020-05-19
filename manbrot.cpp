#include <complex>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>

const int SCREEN_W = 640;
const int SCREEN_H = 480;

double zoom_factor = 0.01;

double scale = 150.0;

double x_offset = 0;
double y_offset = 0;

std::vector<unsigned int> pallete {
	0xFFFFFF, 0xDDDDDD, 0xBBBBBB, 0x999999,
	0x777777, 0x555555, 0x333333, 0x111111,
	0x111111, 0x333333, 0x555555, 0x777777,
	0x999999, 0xBBBBBB, 0xDDDDDD, 0xFFFFFF
};

unsigned int mandelbrot(std::complex<double> c, int max_itt = 64){
	std::complex<double> z {0,0};
	int itt = 0;
	while(norm(z) < 4.0 && itt < max_itt){
		z = z*z + c;
		++itt;
	}
	if(itt == max_itt){
		return 0x000000;
	}
	
	return pallete[(itt-1) % pallete.size()];
}


void world2screen(double xw, double yw, int *xs, int *ys){
	*xs = (int)((xw + x_offset)*scale) + SCREEN_W/2;
	*ys = -(int)((yw + y_offset)*scale) + SCREEN_H/2;
}

void screen2world(int xs, int ys, double *xw, double *yw){
	*xw = (double)(xs - SCREEN_W/2)/scale - x_offset;
	*yw = -(double)(ys - SCREEN_H/2)/scale - y_offset;
}

int main(){
	
	// Initializations
	int max_itt = 2;

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	unsigned int *pixels = new unsigned int[SCREEN_W*SCREEN_H];
	memset(pixels, 255, SCREEN_W*SCREEN_H*sizeof(unsigned int));

	SDL_Init(SDL_INIT_VIDEO);
	
	window = SDL_CreateWindow("Mandelbrot Set",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0xFF,0xFF,0xFF,0xFF);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);


	// Main loop
	bool running = true;
	SDL_Event e;
	int x_fix = 0;
	int y_fix = 0;
	bool panning = false;
	enum {frozen, in, out} zoom_state = frozen;
	while(running){
	
		while(SDL_PollEvent(&e) != 0){
			switch(e.type){
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					switch(e.key.keysym.sym){
						case SDLK_UP: max_itt += 1; break;
						case SDLK_DOWN: max_itt -= 1; break;
						// Zoom
						case SDLK_q:
							zoom_state = in;
							break;
						case SDLK_a:
							zoom_state = out;
							break;
						case SDLK_w:
							zoom_factor *= 1.5;
							break;
						case SDLK_s:
							zoom_factor /= 1.5;
							break;
					}
					break;
				case SDL_KEYUP:
					switch(e.key.keysym.sym){
						case SDLK_q:
							zoom_state = frozen;
							break;
						case SDLK_a:
							zoom_state = frozen;
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if(e.button.button == SDL_BUTTON_LEFT){	
						SDL_GetMouseState(&x_fix, &y_fix);
						panning = true;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if(e.button.button == SDL_BUTTON_LEFT){
						panning = false;
					}
					break;
			}
		}

		if(max_itt <= 0) max_itt = 1;
	
		// Panning
		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
	
		if(panning){
			x_offset += (mouse_x - x_fix)/scale;
			y_offset -= (mouse_y - y_fix)/scale;
			x_fix = mouse_x;
			y_fix = mouse_y;
		}
		
		// Zooming
		if(zoom_state != frozen){
			double z = 0;
			if(zoom_state == in){
				z = 1+zoom_factor;
			}else{
				z= 1-zoom_factor;
			}
			
			double mouse_xi, mouse_yi;
			screen2world(mouse_x, mouse_y, &mouse_xi, &mouse_yi);

			scale *= z;

			double mouse_xf, mouse_yf;
			screen2world(mouse_x, mouse_y, &mouse_xf, &mouse_yf);
		
			printf("scale: %f\n", scale);
			printf("dxw: %f dyw: %f\n", mouse_xf-mouse_xi, mouse_yf-mouse_yi);
			printf("dx:  %f dy:  %f\n\n", (mouse_xf-mouse_xi)*scale, (mouse_yf-mouse_yi)*scale);
			x_offset += (mouse_xf - mouse_xi);
			y_offset += (mouse_yf - mouse_yi);
		}

		// Calculate for every point
		for(int y = 0; y < SCREEN_H; ++y){
			for(int x = 0; x < SCREEN_W; ++x){
				double xw, yw;
				screen2world(x, y, &xw, &yw);
				std::complex<double> c {xw, yw};
				pixels[SCREEN_W*y + x] = mandelbrot(c, max_itt);
			}
		}

		// Update texture
		SDL_UpdateTexture(texture, NULL, pixels, SCREEN_W*sizeof(unsigned int));

		// Render
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	
		//SDL_Delay(1000/60);	
	}

	// Clean up
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
