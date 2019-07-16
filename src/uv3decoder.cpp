#include "uv3decoder.h"
#include <vector>
#include <fstream>
#include <iostream>

#include <vector>
#include <memory>
#include <assert.h>

#include <algorithm>
#include <codecvt>
#include <string>

#include <string>
#include <iostream>
#include <clocale>
#include <locale>
#include <vector>

namespace freeUV3
{
/**
 * @class UV3Decoder
 * @brief This class represent a file  UV3File instance
 * 
 */

/**
 * @class UV3File
 * @brief This class represent a file contained by UV3Decoder instance
 * 
 */






#pragma pack(1)

enum layType
{
	l31 = 31,
	l32,
	l33,
	l34,
	l41 = 41,
	l43 = 43

};

struct UV3Node
{
	UV3Node() : parent{0}, ID{0}, index{0}, ref{0},l_{0,0,0,0,0,0,0,0,0,0} {}
	int ID;
	int index;
	UV3Node* parent; 
	UV3Node *ref;

	std::vector<UV3Node*> children_;
	std::vector<UV3Node*> references_;


	struct layout {

		int ID;
		int index;
		int repetitions;
		short parent;
		short unkownField;
		short concatenateWith; //en las 34?
		short otro;
		int reference;
		int kk;
		int kk2;
	} l_;


	void parse(const std::vector<std::unique_ptr<UV3Node>> &nodeList)
	{

		ID = l_.ID;
		index = l_.index;
		parent = nodeList[l_.parent].get();
		ref = nodeList[l_.reference].get();
		if (parent){
			parent->children_.emplace_back(this);
		}
		//nodeList[l_.reference]->references_.push_back(this);
		

	}

	virtual void parse_impl(const std::vector<std::unique_ptr<UV3Node>> &lista) {};
	virtual ~UV3Node() = default;
private:
	friend std::istream& operator>>(std::istream&,UV3Node&);

	std::istream& serialize(std::istream &is )
	{
		is.read((char*)&l_, sizeof(layout));
		serialize_impl(is);
		if (!is.good())
			assert(false);
		return is;
	}
	virtual std::istream& serialize_impl(std::istream &is) { return is; };
	
};

std::istream& operator>>(std::istream &is, UV3Node &node) {

	node.serialize(is);
	return is;
}


struct UV31 : UV3Node
{
	
	struct tmp
	{
        char16_t name[32];
		
	} kk;

	 std::istream& serialize_impl(std::istream &is) override 
	{
		
		is.read((char*)&kk, sizeof(tmp));
		return is;
	}


};

struct UV32 : UV3Node
{

	struct tmp
	{
		int reservedSize;

		short unknownStuff[20];
		int realSizeOfTheNode;
		int kk;

		unsigned char isDirectory;
		short kk2;
		unsigned char kkk;
		int kf;
		int indexStruct34;
		unsigned short kh;

		unsigned char unknownEnumerated;
	} kk;

	virtual std::istream& serialize_impl(std::istream &is) override
	{

		is.read((char*)&kk, sizeof(tmp));
		return is;
	}



};

struct UV33 : UV3Node
{

	std::vector<UV32*> data_;
    std::vector<std::u16string> names_;




	virtual void parse_impl(const std::vector<std::unique_ptr<UV3Node>> &lista) override
	{

        std::u16string str;
		int i = nodes_.size() - 1;
		
		for (auto it = children_.crbegin(); it != children_.crend();++it)
		{ 
			str.insert(0, nodes_[i].name_, nodes_[i].size_ > 22 ? 22 : nodes_[i].size_);
			
			if ((*it)->ID == 31) {
				UV31* k = (UV31*)(*it);
				str =  k->kk.name;
			}

			else {
				names_.push_back(str);
				UV32* dt = dynamic_cast<UV32*>(*it);
				assert(dt != nullptr);
				data_.push_back(dt);
				i--;
				str.clear();
			}
		}
	
	}

	virtual std::istream& serialize_impl(std::istream &is) override
	{

		internal n;
		is.read((char*)&n, sizeof(internal));
		while (n.indexNodeMaybe_ != 0) {
			nodes_.push_back(n);
			is.read((char*)&n, sizeof(internal));
		}
		return is;
	}


private:
	struct internal {
		int indexNodeMaybe_;
		int unknownField_;
		int isDirectory_; // 2 si ; 1 no
		int size_;
		int zero_;
		char16_t name_[22];
	};
	std::vector<internal> nodes_;


};

struct UV34 : UV3Node { };
struct UV41 : UV3Node { };
struct UV43 : UV3Node { };
struct UVNPI : UV3Node { };


class UV3FileDer : public UV3File
{

	class UV3FilePrivate
	{
	public:

		class char_array_buffer : public std::streambuf
		{
		public:
			explicit char_array_buffer(char *begin, char *end)
			{
				std::copy(begin, end, std::back_inserter(buffer_));
				setg(buffer_.data(), buffer_.data(), buffer_.data()+buffer_.size());
			}
			char_array_buffer(const char_array_buffer& rhs) = delete;
			std::vector<char> buffer_;
		};

		class UV34StreamBuf : public std::streambuf
		{

			std::vector<UV34*> nodes_;
			size_t total_size_;
			size_t current_pos_{ 0 };

		public:
			explicit UV34StreamBuf(std::shared_ptr<std::ifstream> &fptr, std::vector<UV34*> &nodes, size_t total_size, std::size_t buff_sz = 0x8000 - 32, std::size_t put_back = 0) :
				fptr_(fptr),
				put_back_(std::max(put_back, size_t(1))),
				buffer_(std::max(buff_sz, put_back_) + put_back_),
				nodes_(nodes),
				total_size_(total_size)
			{
				char *end = &buffer_.front() + buffer_.size();
				setg(end, end, end);

				fptr_->seekg((nodes_[0]->index * 0x8000) + 32);
			}


		private:

			size_t nodepos() {	return current_pos_ / (0x8000 - 32);	}



			std::streamsize showmanyc() override
			{
				//assert(std::less_equal<const char *>()(current_pos_, total_size_));
				return total_size_ - current_pos_;
			}

			int uflow() override {
				if (underflow() == EOF) return EOF;
				gbump(1);
				return gptr()[-1];
			}


			// overrides base class underflow()
            int_type underflow() override
			{
				if (gptr() < egptr()) // buffer not exhausted
					return traits_type::to_int_type(*gptr());

				char *base = &buffer_.front();
				char *start = base;

				std::streamsize n = 0;
				if (total_size_ == current_pos_)
					return traits_type::eof();

				if (nodepos() > nodes_.size())
					return traits_type::eof();

				fptr_->seekg(nodes_[nodepos()]->index * 0x8000 + 32);
				fptr_->read(start, std::min(buffer_.size() - (start - base + put_back_), total_size_ - current_pos_));
				if (fptr_)
				{
					n = fptr_->gcount();
				}
				current_pos_ += n;


				if (n == 0)
					return traits_type::eof();

				// Set buffer pointers
				setg(base, start, start + n);

				return traits_type::to_int_type(*gptr());
			}

			UV34StreamBuf(const UV34StreamBuf &) = delete;
			UV34StreamBuf &operator= (const UV34StreamBuf &) = delete;

		private:
			std::shared_ptr<std::ifstream> fptr_;
			const std::size_t put_back_;
			std::vector<char> buffer_;
		};
		
	};
	std::streambuf*  databuf_;
	std::istream stream;
	
public:
	~UV3FileDer()
	{
		assert(false);
	}
	UV3FileDer(std::shared_ptr<std::ifstream>& st ,UV32 *n, const std::string &s, const std::string &path ) :  UV3File{stream,s,path} ,stream(nullptr)
	{	
		//filename_ = s;
		//path_ = path;
		

		if (n->kk.realSizeOfTheNode < 0x8000 - 96) {
			std::vector<char> c;
			c.resize(n->kk.realSizeOfTheNode);
			st->seekg(n->index * 0x8000 + 96);
			st->read(c.data(), n->kk.realSizeOfTheNode);
			databuf_ =  new UV3FilePrivate::char_array_buffer(c.data(), c.data() + c.size());
			stream.rdbuf(databuf_);			
		}
		else {
			int tamanyo = n->kk.realSizeOfTheNode;
			std::vector<UV34*> nodos;
			UV3Node *nod = n;
			while (!nod->children_.empty())
			{
				for (auto k : nod->children_) {
					if (dynamic_cast<UV34*>(k)) {
						nodos.push_back(dynamic_cast<UV34*>(k));
						nod = k;
					}
				}
			};
			databuf_ = new UV3FilePrivate::UV34StreamBuf(st,nodos,tamanyo);
			stream.rdbuf(databuf_);
		}
	}
};


class UV3DecoderPrivate
{
public:

	std::vector<std::unique_ptr<UV3Node>> nodelist;
	std::vector<UV3File*> f;
	bool good{ false };

	std::shared_ptr<std::ifstream> ifs;

	void cleanup() { nodelist.clear(); f.clear(); good = false; ifs->clear(); }

	class UVFactory
	{
	public:
		static std::unique_ptr<UV3Node> New(layType l)
		{
			switch (l)
			{
            case l32: return  std::unique_ptr<UV3Node>(new UV32);
            case l31: return  std::unique_ptr<UV3Node>(new UV31);
            case l33: return  std::unique_ptr<UV3Node>(new UV33);
            case l34: return  std::unique_ptr<UV3Node>(new UV34);
            case l41: return  std::unique_ptr<UV3Node>(new UV41);
            case l43: return  std::unique_ptr<UV3Node>(new UV43);
			default:
				return nullptr;
			}
		}
	};
};

/*!
 * @class UV3Decoder
 * This object represent an instance of UVFile to be decoded.
 * 
 */

/**
 * @fn UV3Decoder::open
 * @brief opens a new instance
 * 
 * @param filename 
 */
void UV3Decoder::open(const char *filename)
{
	if (d_ptr->ifs == nullptr){
		d_ptr->ifs.reset(new  std::ifstream(filename,std::ifstream::binary));
	}
	else if ( d_ptr->ifs->is_open())
		return;
	d_ptr->cleanup();
	//d_ptr->ifs->open(filename, std::ifstream::binary);
	
	std::vector<UV33*> p33;

	d_ptr->nodelist.push_back(nullptr);
    d_ptr->nodelist.push_back(std::unique_ptr<UV3Node>(new UVNPI));

	for (int i = 2; !d_ptr->ifs->eof(); ++i) {
		d_ptr->ifs->seekg(0x8000 * i, d_ptr->ifs->beg);

		int kk = d_ptr->ifs->peek();
		if (!d_ptr->ifs->good())
			break;

		auto l = UV3DecoderPrivate::UVFactory::New(layType(kk));
		if (l) {
			*d_ptr->ifs >> *l.get();
		}
		else {
			l = nullptr;
		}

		if (l != nullptr) {

			l->parse(d_ptr->nodelist);
			if (l->ID == 33)
				p33.push_back((UV33*)l.get());
		}
		d_ptr->nodelist.emplace_back(std::move(l));

	}

	for (auto &k : d_ptr->nodelist) {
		if (k != nullptr)
			k->parse_impl(d_ptr->nodelist);
	}


    std::vector<std::u16string> cad;
	std::vector<UV3Node*> p32;
	for (auto n : p33) {

		for (auto i = 0U; i < n->data_.size(); ++i) {

			p32.push_back(n->data_[i]);
			cad.push_back(n->names_[i]);
            std::u16string path;

			UV3Node * pp = n->parent;
			while (UV32 *d = dynamic_cast<UV32*>(pp)) {
				auto it = std::find(p32.begin(), p32.end(), d);
				if (it != p32.end())
				{
					int k = std::distance(p32.begin(), it);
                    std::u16string p{ cad[k] };
                    p.append(u"/");
					path.insert(0, p);
					pp = (*it)->parent->parent;
					;
				}
				else
					break;

			};

			if (n->data_[i]->kk.isDirectory == 2) // ficheros
			{
                std::string ss(path.begin(),path.end());
               
				auto kk = new UV3FileDer(d_ptr->ifs, n->data_[i], std::string(n->names_[i].begin(), n->names_[i].end()), std::string(path.begin(), path.end()));
                d_ptr->f.push_back(kk);

			}

		}

	};
	d_ptr->good = true;


}

/**
 *  Construct a new UV3Decoder object
 * 
 * @param filename 
 * 		If present, UV3Decoder::open is atomatically called by .ctor
 */
UV3Decoder::UV3Decoder(const char *filename) : d_ptr{ new UV3DecoderPrivate }
{
	if (filename != nullptr)
		open(filename);

}
/**
 * @brief Destroy the UV3Decoder::UV3Decoder object
 * 
 */
UV3Decoder::~UV3Decoder() {}

/**
 * @brief Check if a file is opened and valid by the instance
 * 
 * @return true 
 * @return false 
 */
bool UV3Decoder::is_open() const {
	return d_ptr->ifs->is_open();
}

/**
 * @brief Check if file is conformant
 * 
 * @return true 
 * @return false 
 */
bool UV3Decoder::good() const {
	return d_ptr->good;
}

/*! Closes the file associated with the instance.
 * 
 */
void UV3Decoder::close() {
	d_ptr->ifs->close();
	d_ptr->cleanup();
}

/**
 * @brief blablabla
 * 
 * @return std::vector<UV3File*> 
 */
std::vector<UV3File*> UV3Decoder::get() { return d_ptr->f; }

}