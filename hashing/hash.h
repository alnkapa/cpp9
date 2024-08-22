#ifndef HASH_H
#define HASH_H
#include <memory>
#include <openssl/evp.h>
#include <boost/crc.hpp>
#include "../global/hash.h"
#include "hash_type.h"

namespace hash
{
template <typename HashTypeImpl> class HasherBase
{
  public:
    virtual ~HasherBase() {};
    virtual HashTypeImpl hash(const HashTypeImpl &) const = 0;
    virtual Algorithm getAlgorithm() const noexcept = 0;
};

template <typename HashTypeImpl, Algorithm Alg> class HashImpl : public HasherBase<HashTypeImpl>
{
  private:
    // хеш-функция
    const EVP_MD *m_hash;

  public:
    HashImpl()
    {
        if constexpr (Alg == Algorithm::SHA256)
        {
            m_hash = EVP_sha256();
        }
        else if constexpr (Alg == Algorithm::MD5)
        {
            m_hash = EVP_md5();
        }
        else if constexpr (Alg == Algorithm::SHA1)
        {
            m_hash = EVP_sha1();
        }
        else
        {
            // https://stackoverflow.com/a/64354296
            []<bool flag = false>() { static_assert(flag, "Bad Algorithm"); }();
            // static_assert(Alg == Algorithm::SHA256 || Alg == Algorithm::MD5 || Alg == Algorithm::SHA1, "Bad
            // Algorithm");
        }
    };
    inline Algorithm getAlgorithm() const noexcept
    {
        return Alg;
    };
    HashTypeImpl hash(const HashTypeImpl &in) const
    {
        // хеш-значения
        value_type value[EVP_MAX_MD_SIZE];
        // хеш-длина
        size_type len{0};
        // Освобождаем контекст по выходу
        struct EVP_MD_CTX_Deleter
        {
            void operator()(EVP_MD_CTX *context) const
            {
                EVP_MD_CTX_free(context);
            }
        };
        // контекст для хеширования
        std::unique_ptr<EVP_MD_CTX, EVP_MD_CTX_Deleter> context(EVP_MD_CTX_new());
        if (!context)
        {
            throw std::runtime_error("Failed to create EVP_MD_CTX");
        }

        // инициализация
        if (EVP_DigestInit_ex(context.get(), m_hash, NULL) != 1)
        {
            throw std::runtime_error("Failed to initialize digest");
        }

        // хеширование
        if (EVP_DigestUpdate(context.get(), in.data(), in.size()) != 1)
        {
            throw std::runtime_error("Failed to update digest");
        }

        // результат
        if (EVP_DigestFinal_ex(
                context.get(), reinterpret_cast<unsigned char *>(&value), reinterpret_cast<unsigned int *>(&len)) != 1)
        {
            throw std::runtime_error("Failed to finalize digest");
        }
        return {value, len};
    };
};
template <> class HashImpl<HashTypeImpl, Algorithm::CRC32> : public HasherBase<HashTypeImpl>
{
  public:
    inline Algorithm getAlgorithm() const noexcept
    {
        return Algorithm::CRC32;
    };
    HashTypeImpl hash(const HashTypeImpl &in) const
    {
        boost::crc_32_type crc;
        crc.process_bytes(in.data(), in.size());
        const size_t size = sizeof(value_type);
        union
        {
            unsigned int rez;
            value_type buf[size];
        } val;
        val.rez = crc.checksum();
        return {val.buf, size};
    };
};

static_assert(Hasher<HashTypeImpl, HashImpl<HashTypeImpl, Algorithm::SHA256>>,
              "Hasher does not satisfy Hasher concept");
static_assert(Hasher<HashTypeImpl, HashImpl<HashTypeImpl, Algorithm::MD5>>, "Hasher does not satisfy Hasher concept");
static_assert(Hasher<HashTypeImpl, HashImpl<HashTypeImpl, Algorithm::SHA1>>, "Hasher does not satisfy Hasher concept");
static_assert(Hasher<HashTypeImpl, HashImpl<HashTypeImpl, Algorithm::CRC32>>, "Hasher does not satisfy Hasher concept");

// runtime only
template <typename HashTypeImpl> std::unique_ptr<HasherBase<HashTypeImpl>> CreateHasher(Algorithm al)
{
    switch (al)
    {
    case Algorithm::SHA256:
        return std::make_unique<HashImpl<HashTypeImpl, Algorithm::SHA256>>();
    case Algorithm::MD5:
        return std::make_unique<HashImpl<HashTypeImpl, Algorithm::MD5>>();
    case Algorithm::SHA1:
        return std::make_unique<HashImpl<HashTypeImpl, Algorithm::SHA1>>();
    case Algorithm::CRC32:
    default:
        return std::make_unique<HashImpl<HashTypeImpl, Algorithm::CRC32>>();
    }
};

} // namespace hash

#endif // HASH_H
