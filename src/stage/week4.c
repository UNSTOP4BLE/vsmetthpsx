/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week4.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"
#include "../random.h"
#include "../timer.h"
#include "../animation.h"

//Week 4 background structure
typedef struct
{
	//Stage background base structure
	StageBack back;
	
	Gfx_Tex tex_back0; //bg
	Gfx_Tex tex_back1; //wireframething
	Gfx_Tex tex_back2; //bg

} Back_Week4;

//Week 4 background functions
void Back_Week4_DrawFG(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;
};

void Back_Week4_DrawMD(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;

};

void Back_Week4_DrawBG(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	fixed_t fx, fy;

	fixed_t beat_bop;
	if ((stage.song_step & 0x3) == 0)
		beat_bop = FIXED_UNIT - ((stage.note_scroll / 24) & FIXED_LAND);
	else
		beat_bop = 0;

	//Draw wire frame thing
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT wire_src = {0, 0, 256, 256};
	RECT_FIXED wire_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(500 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Stage_DrawTex(&this->tex_back1, &wire_src, &wire_dst, stage.camera.bzoom);

	//draw boppers
	fx = stage.camera.x;
	fy = stage.camera.y;
	static const struct Back_Week4_LowerBop
	{
		RECT src;
		RECT_FIXED dst;
	} lbop_piece[] = {
	{{0, 0, 256, 67}, {FIXED_DEC(-140,1), FIXED_DEC(-80,1), FIXED_DEC(256,1), FIXED_DEC(67,1)}}, 
	{{0, 67, 256, 64}, {FIXED_DEC(80,1), FIXED_DEC(-75,1), FIXED_DEC(256,1), FIXED_DEC(64,1)}}, 
	};
	
	const struct Back_Week4_LowerBop *lbop_p = lbop_piece;
	for (size_t i = 0; i < COUNT_OF(lbop_piece); i++, lbop_p++)
	{
		RECT_FIXED lbop_dst = {
			lbop_p->dst.x - fx - (beat_bop << 1),
			lbop_p->dst.y - fy + (beat_bop << 3),
			lbop_p->dst.w + (beat_bop << 2),
			lbop_p->dst.h - (beat_bop << 3),
		};
		Stage_DrawTex(&this->tex_back0, &lbop_p->src, &lbop_dst, stage.camera.bzoom);
	}

	//Draw bg
	fx = stage.camera.x;
	fy = stage.camera.y;
	
	RECT sunset_src = {0, 0, 256, 256};
	RECT_FIXED sunset_dst = {
		FIXED_DEC(-165 - SCREEN_WIDEOADD2,1) - fx,
		FIXED_DEC(-140,1) - fy,
		FIXED_DEC(500 + SCREEN_WIDEOADD,1),
		FIXED_DEC(260,1)
	};
	
	Stage_DrawTex(&this->tex_back2, &sunset_src, &sunset_dst, stage.camera.bzoom);
}

void Back_Week4_Free(StageBack *back)
{
	Back_Week4 *this = (Back_Week4*)back;
	
	//Free structure
	Mem_Free(this);
}

StageBack *Back_Week4_New(void)
{
	//Allocate background structure
	Back_Week4 *this = (Back_Week4*)Mem_Alloc(sizeof(Back_Week4));
	if (this == NULL)
		return NULL;
	
	//Set background functions
	this->back.draw_fg = Back_Week4_DrawFG;
	this->back.draw_md = Back_Week4_DrawMD;
	this->back.draw_bg = Back_Week4_DrawBG;
	this->back.free = Back_Week4_Free;
	
	//Load background textures
	IO_Data arc_back = IO_Read("\\WEEK4\\BACK.ARC;1");
	Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
	Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
	Gfx_LoadTex(&this->tex_back2, Archive_Find(arc_back, "back2.tim"), 0);

	Mem_Free(arc_back);
	
	return (StageBack*)this;
}
