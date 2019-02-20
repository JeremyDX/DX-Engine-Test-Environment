#pragma once

struct ContentOverlay
{
		void(*update)();

		uint16_t total_components = 0;
		uint32_t *components = NULL;

		uint16_t texture_index[2];
		uint16_t offsets[2];
		uint8_t total_textures;

		void Insert(uint32_t value)
		{
			if (components == NULL)
			{
				total_components = 1;
				components = new uint32_t[1]{ value };
				return;
			}

			if ((total_components & (total_components - 1)) == 0)
			{
				uint32_t *tmp = new uint32_t[total_components << 1];
				memcpy(tmp, components, total_components * sizeof(uint32_t));
				delete[] components;
				components = tmp;
			}

			components[total_components++] = value;
		}

		void SetUpdateProc(int index);
};
