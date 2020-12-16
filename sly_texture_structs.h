#pragma once

#include <cstdint>
#include <ez_stream.h>

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
		record = new clut_record_t[header.numRecords];
		for (int i = 0; i < header.numRecords; i++) {
			record[i] = stream.read<clut_record_t>();
		}
	};
	//clut_meta_table_t(clut_meta_table_t&&) = default;

	struct clut_meta_header_t {
		short numRecords;
	} header;

	clut_record_t* record;

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

	//texture_record_t(texture_record_t&&) = default

	uint16_t flags;
	uint16_t id;
	uint32_t unk_color0;
	uint32_t unk_color1;
	uint32_t unk_i0;
	short unk_sh0;
	unsigned char unk5;
	unsigned char unk7;
	unsigned char unk_flag;

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