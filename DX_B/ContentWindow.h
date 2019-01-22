#pragma once

class ContentWindow
{
	public:
		ContentWindow();
		~ContentWindow();

	public:	
		unsigned __int32 background_color;	
		  signed __int16 background_shader_id; 
	
	public:
		signed __int32 state_change_alias[4];
		signed __int16 state_vertex_offsets[4];
		signed __int16 state_vertex_sizes[4];
		signed  __int8 state_changes;

	public:
		void (*update)();
		void (*children)();
		void UpdateLeftRight();
		void SetUpdateProc(int index);
		void SetChildUpdateProc(int index, int size, int disabled_bits);

	public:
		int menu_index;
		int menu_size;
		int disabled_menu_bits;
};
