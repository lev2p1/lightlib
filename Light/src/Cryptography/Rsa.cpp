#include "../../include/lightlib/vendor/Cryptography/Rsa.hpp"
#include <exception>

namespace lightlib::crypto {
    namespace euclid {
        crypt_int gcd(crypt_int a, crypt_int b) {
            while (b != 0) {
                crypt_int tempInt = b;
                b = a % b;
                a = tempInt;
            }
            return a;
        }

        void extendedEuclid(crypt_int a, crypt_int b, crypt_int* x, crypt_int* y) {
            if (b == 0) {
                *x = 1;
                *y = 0;
                return;
            }

            crypt_int x1, y1;
            extendedEuclid(b, a % b, &x1, &y1);
            *x = y1;
            *y = x1 - ( a / b) * y1;
        }
    }

    RSA::RSA(const crypt_int &p, const crypt_int &q, const crypt_int &r)
        : p(0), q(0), phi(0), m(0), r(0), private_key(nullptr), public_key(nullptr)
    {
        try {
            setParameters(p, q, r);
            evaluateKeys();
        } catch (std::exception &e) {
            throw std::runtime_error("RSA exception: Failed to set parameters or calculate keys. " + std::string(e.what()));
        }
    }

    RSA::~RSA() {
        delete private_key;
        delete public_key;
    }

    void RSA::setParameters(crypt_int p, crypt_int q, crypt_int r) {
        this->p = p;
        this->q = q;
        this->r = r;
        this->m = p * q;
        this->phi = evaluatePhi(p, q);

        if (!isPrime(p) || !isPrime(q)) {
            throw std::runtime_error("RSA exception: P or Q not prime.");
        } else if (r <1 || r >=phi || euclid::gcd(r, phi) != 1) {
            throw std::runtime_error("RSA exception: R invalid! Must be within 1 < R < ɸ(m) and co-prime with ɸ(m).");
        }
    }

    void RSA::evaluateKeys() {
        if (private_key != nullptr || public_key != nullptr) {
            throw std::runtime_error("RSA exception: Keys already exist.");
        }
        evaluatePublicKey();
        evaluatePrivateKey();
    }

    void RSA::evaluatePublicKey() {
        this->public_key = new publicKey{this->r, this->m};
    }

    void RSA::evaluatePrivateKey() {
        crypt_int a = this->phi;
        crypt_int b = this->r;
        crypt_int s = 0;
        crypt_int x = 0;

        euclid::extendedEuclid(a, b, &x, &s);
        s = (s < 0) ? makePositive(s, this->phi) : s;

        this->private_key = new privateKey{s, this->p, this->q};
    }

    publicKey *RSA::getPublicKey() const { return public_key; }
    privateKey *RSA::getPrivateKey() const { return private_key; }

    crypt_int RSA::evaluatePhi(crypt_int a, crypt_int b) {
        if (!isPrime(a) || !isPrime(b)) return 0;
        return (a - 1) * (b - 1);
    }

    crypt_int RSA::makePositive(crypt_int n, crypt_int mod) {
        crypt_int result = n;
        while (result < 0) {
            result += mod;
        }
        return result;
    }

    crypt_str RSA::encrypt(const std::string& plaintext) {
        crypt_str result;
        for (char plainchar : plaintext) {
            result.push_back(encryptChar(plainchar));
        }
        return result;
    }

    std::string RSA::decrypt(const crypt_str& ciphertext) {
        std::string result;
        for (size_t i = 0; i < ciphertext.size(); i++) {
            result[i] = decryptChar(ciphertext[i]);
        }
        return result;
    }

    crypt_int RSA::encryptChar(const char& plainchar) const {
        return powm(static_cast<crypt_int>(plainchar), public_key->r, public_key->m);
    }

    char RSA::decryptChar(const crypt_int& cipherchar) const {
        const crypt_int result = powm(cipherchar, private_key->s, private_key->q * private_key->p);
        return static_cast<char>(result);
    }

}