#ifndef FREEUV3__H
#define FREEUV3__H

#include <istream>
#include <memory>
#include <string>
#include <vector>

namespace freeUV3 {
class UV3FilePrivate;
class UV3File
{
public:
    /** filename */
    const std::string filename_;
    /** relative folder */
    const std::string path_;
    /** stream for readin this */
    std::istream& buffer_;

    virtual ~UV3File() = default;
protected:
    UV3File(std::istream& buffer, const std::string& filename, const std::string& path)
            : filename_(filename)
            , path_(path)
            , buffer_(buffer){}
    

private:
    UV3File(const UV3File& rhs) = delete;
};

class UV3DecoderPrivate;
class UV3Decoder
{
public:
    explicit UV3Decoder(const char* filename = nullptr);
    ~UV3Decoder();

    void                  open(const char* filename);
    bool                  good() const;
    bool                  is_open() const;
    void                  close();
    std::vector<UV3File*> get();

private:
    std::unique_ptr<UV3DecoderPrivate> d_ptr;
};
} // namespace freeUV3
#endif // FREEUV3__H