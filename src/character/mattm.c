/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mattm.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//mattm character structure
enum
{
	mattm_ArcMain_Idle0,
	mattm_ArcMain_Idle1,
	mattm_ArcMain_Left,
	mattm_ArcMain_Down,
	mattm_ArcMain_Up,
	mattm_ArcMain_Right,
	
	mattm_Arc_Max,
};

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main;
	IO_Data arc_ptr[mattm_Arc_Max];
	
	Gfx_Tex tex;
	u8 frame, tex_id;
} Char_mattm;

//mattm character definitions
static const CharFrame char_mattm_frame[] = {
	{mattm_ArcMain_Idle0, {  0,   0, 106, 192}, { 45, 105}}, //0 idle 1
	{mattm_ArcMain_Idle0, {107,   0, 108, 190}, { 49, 103}}, //1 idle 2 
	{mattm_ArcMain_Idle1, {  0,   0, 107, 190}, { 50, 104}}, //2 idle 3 
	{mattm_ArcMain_Idle1, {108,   0, 105, 192}, { 53, 104}}, //3 idle 4
	
	{mattm_ArcMain_Left, {  0,   0,  112, 196}, { 50, 105}}, //4 left 1 
	{mattm_ArcMain_Left, { 112,   0,  112, 196}, { 54, 104}}, //5 left 2 
	
	{mattm_ArcMain_Down, {  0,   0, 118, 183}, { 39, 96}}, //6 down 1 
	{mattm_ArcMain_Down, {119,   0, 117, 183}, { 37, 96}}, //7 down 2 
	
	{mattm_ArcMain_Up, {  0,   0, 102, 205}, { 40, 118}}, //8 up 1 
	{mattm_ArcMain_Up, {103,   0, 103, 203}, { 42, 116}}, //9 up 2 
	
	{mattm_ArcMain_Right, {  0,   0, 117, 199}, { 37, 110}}, //10 right 1 
	{mattm_ArcMain_Right, {118,   0, 114, 199}, { 44, 110}}, //11 right 2
};

static const Animation char_mattm_anim[CharAnim_Max] = {
	{2, (const u8[]){ 1,  2,  3,  0, ASCR_BACK, 1}}, //CharAnim_Idle
	{2, (const u8[]){ 4,  5, ASCR_BACK, 1}},         //CharAnim_Left
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
	{2, (const u8[]){ 6,  7, ASCR_BACK, 1}},         //CharAnim_Down
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
	{2, (const u8[]){ 8,  9, ASCR_BACK, 1}},         //CharAnim_Up
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
	{2, (const u8[]){10, 11, ASCR_BACK, 1}},         //CharAnim_Right
	{0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//mattm character functions
void Char_mattm_SetFrame(void *user, u8 frame)
{
	Char_mattm *this = (Char_mattm*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_mattm_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_mattm_Tick(Character *character)
{
	Char_mattm *this = (Char_mattm*)character;
	
	if (stage.stage_id == StageId_2_1)
	{
		this->character.focus_x = FIXED_DEC(35,1);
		this->character.focus_y = FIXED_DEC(5,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}

	if (stage.stage_id == StageId_2_2)
	{	
		this->character.focus_x = FIXED_DEC(35,1);
		this->character.focus_y = FIXED_DEC(5,1);
		this->character.focus_zoom = FIXED_DEC(1,1);
	}


	//Perform idle dance
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
		Character_PerformIdle(character);
	
	//Animate and draw
	Animatable_Animate(&character->animatable, (void*)this, Char_mattm_SetFrame);
	Character_Draw(character, &this->tex, &char_mattm_frame[this->frame]);
}

void Char_mattm_SetAnim(Character *character, u8 anim)
{
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_mattm_Free(Character *character)
{
	Char_mattm *this = (Char_mattm*)character;
	
	//Free art
	Mem_Free(this->arc_main);
}

Character *Char_mattm_New(fixed_t x, fixed_t y)
{
	//Allocate mattm object
	Char_mattm *this = Mem_Alloc(sizeof(Char_mattm));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_mattm_New] Failed to allocate mattm object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_mattm_Tick;
	this->character.set_anim = Char_mattm_SetAnim;
	this->character.free = Char_mattm_Free;
	
	Animatable_Init(&this->character.animatable, char_mattm_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = 0;
	
	this->character.health_i = 1;
	
	this->character.focus_x = FIXED_DEC(35,1);
	this->character.focus_y = FIXED_DEC(10,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	this->arc_main = IO_Read("\\CHAR\\MATTM.ARC;1");
	
	const char **pathp = (const char *[]){
		"idle0.tim", //mattm_ArcMain_Idle0
		"idle1.tim", //mattm_ArcMain_Idle1
		"left.tim",  //mattm_ArcMain_Left
		"down.tim",  //mattm_ArcMain_Down
		"up.tim",    //mattm_ArcMain_Up
		"right.tim", //mattm_ArcMain_Right
		NULL
	};
	IO_Data *arc_ptr = this->arc_ptr;
	for (; *pathp != NULL; pathp++)
		*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	return (Character*)this;
}
