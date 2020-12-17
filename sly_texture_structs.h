#pragma once

#include <cstdint>
#include <ez_stream.h>
#include <main.h>

struct UNK_TABLE_t {
	uint16_t u0x0;
	uint16_t u0x2;
	uint16_t u0x4;
	uint16_t u0x6;
};

struct clut_record_t {
	int unk0;
	short num_colors;
	short size;
	int dataOffset;
};

struct clut_meta_table_t {

	clut_meta_table_t() {}
	clut_meta_table_t(ez_stream& stream) {
		header = stream.read<clut_meta_header_t>();
		record.resize(header.numRecords);
		for (int i = 0; i < header.numRecords; i++) {
			record[i] = stream.read<clut_record_t>();
		}
	};
	//clut_meta_table_t(clut_meta_table_t&&) = default;

	struct clut_meta_header_t {
		short numRecords;
	} header;

	std::vector<clut_record_t> record;

};

struct image_record_t {

	image_record_t() {}
	image_record_t(ez_stream& stream) {
		width = stream.read<short>();
		height = stream.read<short>();
		unk0 = stream.read<int>();
		psn = stream.read<char>();
		tbw = stream.read<char>();
		unk1[0] = stream.read<short>();
		unk1[1] = stream.read<short>();
		unk1[2] = stream.read<short>();
		dataOffset = stream.read<int>();
	}
	//image_record_t(image_record_t&&) = default;

	short width;
	short height;
	int unk0;
	char psn;
	char tbw;
	short unk1[3];
	int dataOffset;

};

struct image_meta_table_t {
	 
	image_meta_table_t() {}
	image_meta_table_t(ez_stream& stream) {
		header = stream.read<image_meta_header_t>();
		for (int i = 0; i < header.numRecords; i++) {
			texture.push_back(image_record_t(stream));
		}
	}
	//image_meta_table_t(image_meta_table_t&&) = default;

	~image_meta_table_t() {
	}

	struct image_meta_header_t {
		short numRecords;
	} header;

	std::vector<image_record_t> texture;
};

// TEXTURE_RECORD2
struct texture_record_t {

	texture_record_t() {}
	texture_record_t(ez_stream& stream) {
		flags = stream.read<uint16_t>();
		id = stream.read<uint16_t>();
		unk_color0 = stream.read<uint32_t>();
		unk_color1 = stream.read<uint32_t>();
		unk_i0 = stream.read<uint32_t>();
		unk_sh0 = stream.read<short>();
		unk5 = stream.read<byte>();
		unk7 = stream.read<byte>();
		unk_flag = stream.read<byte>();
		if (unk_flag) {
			union1.flag_on = stream.read<decltype(union1.flag_on)>();
		}
		else {
			union1.flag_off = stream.read<decltype(union1.flag_off)>();
		}
		numImages = stream.read<byte>();
		numCluts = stream.read<byte>();

		image_index.resize(numImages);
		for (int i = 0; i < numImages; i++) {
			image_index[i] = stream.read<short>();
		}

		clut_index.resize(numCluts);
		for (int i = 0; i < numCluts; i++) {
			clut_index[i] = stream.read<short>();
		}
	}

	const bool is_initialized() const { return m_initialized; }

	void make_texture(uint8_t* paletteBuf, uint8_t* imageBuf, int width, int height, uint8_t* csm1ClutIndices) {
		m_initialized = true;
		m_bitmap.resize(width * height * 4);
		for (int i = 0; i < width * height; i++) {
			const int idx = csm1ClutIndices[imageBuf[i]] * 4;
			const int line = ((i + 1) / width) + 1;

			// const alpha = palette_slice[idx + 3] / 256; // Paint alpha black
			m_bitmap[4 * (width * height - 1 - line * width + (i % width)) + 3] = 0xFF; //paletteBuf[idx + 3];
			m_bitmap[4 * (width * height - 1 - line * width + (i % width)) + 2] = paletteBuf[idx + 2];
			m_bitmap[4 * (width * height - 1 - line * width + (i % width)) + 1] = paletteBuf[idx + 1];
			m_bitmap[4 * (width * height - 1 - line * width + (i % width)) + 0] = paletteBuf[idx + 0];
		}

		glGenTextures(1, &gl_texture);
		glBindTexture(GL_TEXTURE_2D, gl_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_bitmap.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//texture_record_t(texture_record_t&&) = default

	unsigned int gl_texture;

	uint16_t flags;
	uint16_t id;
	uint32_t unk_color0;
	uint32_t unk_color1;
	uint32_t unk_i0;
	short unk_sh0;
	unsigned char unk5;
	unsigned char unk7;
	unsigned char unk_flag;

	bool m_initialized{false};

	union {
		struct {
			byte unk3[8 + 8];
			byte unk4[6];
			byte unk[11];
		} flag_on;
		struct {
			byte unk[1];
			byte unk2[4];
		} flag_off;
	} union1;

	char numImages;
	char numCluts;

	std::vector<short> image_index;
	std::vector<short> clut_index;

	std::vector<unsigned char> m_bitmap;

};

// TEXTURE_TABLE2_t
struct texture_table_t {

	texture_table_t() {}
	texture_table_t(ez_stream& stream) {
		header = stream.read<texture_header_t>();
		texture.reserve(header.numTextures);
		for (int i = 0; i < header.numTextures; i++) {
			texture.push_back(texture_record_t(stream));
		}
	}

	~texture_table_t() {
	}

	struct texture_header_t {
		uint16_t numTextures;
		uint16_t unk_0x2;
	} header;

	std::vector<texture_record_t> texture;
};