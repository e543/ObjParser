#pragma once


#include  "Declares.h"

#define digit_offset (int)'0'
#define line_ind_const 9
#define model_ind_const 3
#define dec -1


struct SimpleVertex {
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
	XMFLOAT3 Normal;
};




class Parser
{
	enum State
	{
		pos,
		tex,
		normal,
		index
	};

	UniqueModel* current_model;
	State current_state;
	XMFLOAT3* array_pos;
	XMFLOAT2* array_tex;
	XMFLOAT3* array_normal;
	WORD* array_ind = NULL;

	float line_floats[3];
	int line_words[9];
	size_t float_ind, vert_v, vert_vt, vert_vn, ind_i, model_ind;
	size_t vertices_count, tex_count, normal_count, indices_count;
	size_t total_count;

	std::ifstream source;
	std::string str;

	void modelMalloc();
	bool setSrc(const std::string path);
	void calcMem();
	bool setModel();
	bool setFloats();
	void recordLine();
	void resetParser();
public:
	Parser();
	~Parser();
	void parse(std::string path, UniqueModel* model);
};

inline Parser::Parser() {
	vert_v = vert_vt = vert_vn = ind_i = model_ind = 0;
	vertices_count = tex_count = normal_count = indices_count = 0;
}

inline Parser::~Parser() {
	delete array_pos, array_tex, array_normal, array_ind;
}


inline void Parser::parse(std::string path, UniqueModel* model) {
	if (setSrc(path))
	{
		current_model = model;
		modelMalloc();
		setModel();
		resetParser();
	}
}

inline void Parser::modelMalloc() {
	total_count = vertices_count + tex_count + normal_count;

	array_pos = new XMFLOAT3[vertices_count];
	array_tex = new XMFLOAT2[tex_count];
	array_normal = new XMFLOAT3[normal_count];
	array_ind = new WORD[indices_count * line_ind_const];

	current_model->vertices = new SimpleVertex[indices_count * model_ind_const];
	current_model->indices = new WORD[indices_count * model_ind_const];
	current_model->vertices_size = indices_count * model_ind_const;
	current_model->indices_size = indices_count * model_ind_const;

	size_t i = 0;
	while (i < indices_count * model_ind_const) {
		current_model->indices[i] = (WORD)i;
		i++;
	}
}

inline bool Parser::setSrc(std::string path)
{
	source.open(path);
	if (!source.is_open()) {
		std::cout << "Cannot open the file!" << std::endl;
	}
	else {
		calcMem();
	}
}
inline void Parser::calcMem() {
	vertices_count = tex_count = normal_count = indices_count = 0;
	while (!source.eof()) {
		getline(source, str);

		if (str[0] == 'v' && str[1] == ' ')
			vertices_count++;
		else if (str[0] == 'v' && str[1] == 't')
			tex_count++;
		else if (str[0] == 'v' && str[1] == 'n')
			normal_count++;
		else if (str[0] == 'f' && str[1] == ' ')
			indices_count++;
	}
	source.seekg(0, std::ios::beg);
}


inline bool Parser::setModel() {
	if (source.is_open()) {
		while (!source.eof()) {
			getline(source, str);
			if (setFloats())
				recordLine();
		}
		source.seekg(0, std::ios::beg);

		ind_i = vert_v = 0;
		while (vert_v < indices_count * model_ind_const) {

			current_model->vertices[vert_v] =
			{
				array_pos[array_ind[ind_i + 0]],
				array_tex[array_ind[ind_i + 1]],
				array_normal[array_ind[ind_i + 2]]

			};

			vert_v++;
			ind_i += model_ind_const;
		}

		return true;
	}
	else
		return false;
}


inline bool Parser::setFloats() {
	if (str[0] == 'v' && str[1] == ' ') {
		current_state = pos;
		sscanf_s(str.c_str(), "v %f %f %f ", &line_floats[0], &line_floats[1], &line_floats[2]);
	}
	else if (str[0] == 'v' && str[1] == 't') {
		current_state = tex;
		sscanf_s(str.c_str(), "vt %f %f", &line_floats[0], &line_floats[1]);
	}
	else if (str[0] == 'v' && str[1] == 'n') {
		current_state = normal;
		sscanf_s(str.c_str(), "vn %f %f %f ", &line_floats[0], &line_floats[1], &line_floats[2]);
	}
	else if (str[0] == 'f' && str[1] == ' ') {
		current_state = index;
		sscanf_s(str.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i ",
			&line_words[0], &line_words[1], &line_words[2],
			&line_words[3], &line_words[4], &line_words[5],
			&line_words[6], &line_words[7], &line_words[8]);
	}
	else return false;
	return true;
}

inline void Parser::recordLine() {
	switch (current_state) {
	case pos:
		array_pos[vert_v++] = XMFLOAT3(line_floats[0], line_floats[1], line_floats[2]);
		break;
	case tex:
		array_tex[vert_vt++] = XMFLOAT2(line_floats[0], line_floats[1]);
		break;
	case normal:
		array_normal[vert_vn++] = XMFLOAT3(line_floats[0], line_floats[1], line_floats[2]);
		break;
	case index:

		array_ind[ind_i + 0] = (WORD)line_words[0] + dec;
		array_ind[ind_i + 1] = (WORD)line_words[1] + dec;
		array_ind[ind_i + 2] = (WORD)line_words[2] + dec;
		array_ind[ind_i + 3] = (WORD)line_words[3] + dec;
		array_ind[ind_i + 4] = (WORD)line_words[4] + dec;
		array_ind[ind_i + 5] = (WORD)line_words[5] + dec;
		array_ind[ind_i + 6] = (WORD)line_words[6] + dec;
		array_ind[ind_i + 7] = (WORD)line_words[7] + dec;
		array_ind[ind_i + 8] = (WORD)line_words[8] + dec;
		ind_i += line_ind_const;
		break;
	}
}

inline void Parser::resetParser()
{
	delete array_pos, array_tex, array_normal, array_ind;

	vert_v = vert_vt = vert_vn = ind_i = model_ind = 0;
	vertices_count = tex_count = normal_count = indices_count = 0;

	source.close();
}
