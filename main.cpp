
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>

#include "image.h"
#include "types.h"

using namespace std;

template <typename T>
ostream& stream_write(ostream& os, T& val, size_t n=sizeof(T)) {
	
	return os.write(reinterpret_cast<char*>(&val), n);
}

struct de_tag{
	uint16_t _group_num;
	uint16_t _elem_num;

	de_tag(): _group_num(0), _elem_num(0) {}
	de_tag(uint16_t gn, uint16_t en): _group_num(gn), _elem_num(en) {}
};

struct data_element_US{
	de_tag _tag;
	string _vr = "US";
	uint16_t _vl_us = 2;
	uint16_t _vf_us;

	data_element_US(uint16_t gn, uint16_t en, uint16_t vf_us) : _tag({gn, en}), _vl_us(2), _vf_us(vf_us) {}

	ostream& write(ostream& os){
		stream_write(os, _tag);
		os << _vr;
		stream_write(os, _vl_us);
		stream_write(os, _vf_us);
		return os;
	}
};

struct data_element_UL {
	de_tag _tag;
	string _vr = "UL";
	uint16_t _vl_ul;
	uint32_t _vf_ul;

	data_element_UL(uint16_t gn, uint16_t el, uint32_t vf_ul) : _tag({gn, el}), _vl_ul(4), _vf_ul(vf_ul) {}

	ostream& write(ostream& os){
		
		stream_write(os, _tag);
		os << _vr;
		stream_write(os, _vl_ul);
		stream_write(os, _vf_ul);
		return os;
	}
};

struct data_element_CS{
	de_tag _tag;
	string _vr = "CS";
	uint16_t _vl_cs;
	string _vf_cs;

	data_element_CS(uint16_t gn, uint16_t el, uint16_t vl_cs, string vf_cs) : _tag({gn, el}), _vl_cs(0), _vf_cs(vf_cs) {
		_vl_cs = _vf_cs.size();

		if ((_vl_cs % 2) != 0) {
			_vf_cs.push_back(0x20);
			_vl_cs++;
		}
	}

	ostream& write(ostream& os) {

		stream_write(os, _tag);
		os << _vr;
		stream_write(os, _vl_cs);
		os << _vf_cs;
		return os;
	}

};

struct data_element_UI{
	de_tag _tag;
	string _vr = "UI";
	uint16_t _vl_ui;
	string _vf_ui;

	data_element_UI(uint16_t gn, uint16_t el, uint16_t vl_ui, string vf_ui) : _tag({gn,el}), _vl_ui(0), _vf_ui(vf_ui) {
		_vl_ui = _vf_ui.size();
		if ((_vl_ui % 2) != 0) {
			_vf_ui.push_back(0x00);
			_vl_ui++;
		}
	}

	ostream& write(ostream& os){
		stream_write(os, _tag);
		os << _vr;
		stream_write(os, _vl_ui);
		os << _vf_ui;
		return os;
	}
};

struct data_element_OB{
	
		de_tag _tag;
		string _vr = "OB";
		uint32_t _vl_ob;
		vector<uint8_t> _vf_ob;

		data_element_OB(uint16_t gn, uint16_t el, vector<uint8_t> vf_ob) : _tag({gn, el}), _vl_ob(0), _vf_ob(vf_ob) {
			_vl_ob = _vf_ob.size();
			if ((_vl_ob % 2) != 0) {
				_vf_ob.push_back(0x00);
				_vl_ob++;
			}
		}

		ostream& write(ostream& os, const size_t& elem_n){
			stream_write(os, _tag);
			os << _vr;
			os << '\0' << '\0';

			stream_write(os, _vl_ob);
			for (size_t i = 0; i < elem_n; ++i)
				os << _vf_ob[i];
			return os;
		}

};

struct data_element_IMG {
	de_tag _tag;
	string _vr = "OB";
	uint32_t _vl_im;
	char* _vf_im;

	data_element_IMG(uint16_t gn, uint16_t el, uint32_t vl_im, char* vf_im) : _tag({gn, el}), _vl_im(vl_im), _vf_im(vf_im) {}

	ostream& write(ostream& os, const size_t& elem_n) {
		stream_write(os, _tag);
		os << _vr;
		os << '\0' << '\0';
		stream_write(os, _vl_im);
		for (size_t i = 0; i < elem_n; ++i)
			os << _vf_im[i];
		return os;
	}
};

int main(int argc, char **argv) {

	if (argc != 3)
		exit(EXIT_FAILURE);

	string inFileName(argv[1]);
	string outFileName(argv[2]);


	string s0("1.2.840.10008.5.1.4.1.1.77.1.4"),
		s1("1.2.392.200036.9125.0.19950720112207"), 
		s2("1.2.840.10008.1.2.1"), 
		s3("1.2.392.200036.9125.0.1234567890"), 
		s4("ORIGINAL\PRIMARY");

	vector<size_t> len;
	len.push_back(s0.size());
	len.push_back(s1.size());
	len.push_back(s2.size());
	len.push_back(s3.size());
	len.push_back(s4.size());

	for (size_t i = 0; i < len.size(); ++i) {
		if ((len[i] % 2) != 0)
			len[i] += 1;
	}

	size_t sum_len = len[0] + len[1] + len[2] + len[3] + 2;

	vector<uint8_t> v(2);
	v[0] = 0;
	v[1] = 1;

	// gruppo 2
	data_element_UL g20(0x0002,0x0000, sum_len);
	data_element_OB g21(0x0002,0x0001, v);
	data_element_UI g22(0x0002,0x0002, s0.size(), s0);
	data_element_UI g23(0x0002,0x0003, s1.size(), s1);
	data_element_UI g24(0x0002,0x0010, s2.size(), s2);
	data_element_UI g25(0x0002,0x0012, s3.size(), s3);
	
	// gruppo 8
	data_element_CS g80(0x0008, 0x0008, s4.size(), s4);
	data_element_UI g81(0x0008, 0x0016, s0.size(), s0);
	data_element_UI g82(0x0008, 0x0018, s1.size(), s1);

	
	ofstream os(outFileName, ios::binary);

	
	//header 
	//128 bytes a 0, seguiti dai caratteri DICM, seguiti poi da una serie di Data Element.
	for (size_t i = 0; i < 128; ++i) {
		os << '\0';
		/*
		char zero = 0;
		os.write(&zero, 1);
		*/
	}
	os << "DICM";

	g20.write(os);
	g21.write(os, v.size());
	g22.write(os);
	g23.write(os);
	g24.write(os);
	g25.write(os);

	g80.write(os);
	g81.write(os);
	g82.write(os);



	ifstream is(inFileName, ios::binary);
	if (!is)
		exit(EXIT_FAILURE);

	//leggo il magic number
	string magic;
	getline(is, magic);

	bool binary;
	if (magic == "P6")
		binary = true;
	else if (magic == "P3")
		binary = false;
	else
		exit(EXIT_FAILURE);



	//leggo il commento
	string comment;
	getline(is, comment);
	if (comment.empty() || comment[0] != '#')
		exit(EXIT_FAILURE);

	//leggo w, h, maxval
	unsigned w, h, maxval;
	is >> w >> h >> maxval;

	if (maxval != 255)
		exit(EXIT_FAILURE);

	//leggo l'ultimo a capo
	is.get();

	image<vec3b> img(w, h);

	//leggo l'immagine
	if (binary) {
		is.read(img.data(), img.data_size());
	}
	else {
		for (int y = 0; y < img.height(); ++y) {
			for (int x = 0; x < img.width(); ++x) {
				int val;
				is >> val;
				img(x, y) = val;
			}
		}
	}

	data_element_US gd0(0x0028, 0x0002, 3);
	data_element_CS gd1(0x0028, 0x0004, 3, "RGB");
	data_element_US gd2(0x0028, 0x0006, 0);
	data_element_US gd3(0x0028, 0x0010, 266);
	data_element_US gd4(0x0028, 0x0011, 375);
	data_element_US gd5(0x0028, 0x0100, 8);
	data_element_US gd6(0x0028, 0x0101, 8);
	data_element_US gd7(0x0028, 0x0102, 7);
	data_element_US gd8(0x0028, 0x0103, 0);
	data_element_CS gd9(0x0028, 0x2110, 2, "00");

	gd0.write(os);
	gd1.write(os);
	gd2.write(os);
	gd3.write(os);
	gd4.write(os);
	gd5.write(os);
	gd6.write(os);
	gd7.write(os);
	gd8.write(os);
	gd9.write(os);


	data_element_IMG gp(0x7FE0, 0x0010, img.size()*3, img.data());
	gp.write(os,img.size()*3);


}