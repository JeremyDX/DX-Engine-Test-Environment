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
		static unsigned __int64 begin_frame_index;

	public:
		void (*update)();
		void SetUpdateProc(int index);
};

void UpdateToWindow(void);
